#include "GraphEngine.Jit.Native.h"
#include "Trinity.h"
#include <cstring>
#include <algorithm>
#include <functional>

extern "C" char* tsl_getstring(int32_t* trinity_string_ptr)
{
    auto len = *trinity_string_ptr;
    auto str = Trinity::String::FromWcharArray((u16char*)(trinity_string_ptr + 1), len / 2);
    return _strdup(str.c_str());
}

extern "C" char* tsl_getu8string(int32_t* trinity_string_ptr)
{
    int32_t len = 1 + *trinity_string_ptr;
    char* buf = (char*)malloc(len);
    if (strcpy_s(buf, len, (char*)(trinity_string_ptr + 1))) return nullptr;
    else return buf;
}

extern "C" void* tsl_copy(char* ptr, int32_t size)
{
    auto buf = malloc(size);
    return memcpy(buf, ptr, size);
}

extern "C" void* tsl_copy_dynamic(int32_t* ptr)
{
    auto len = sizeof(int) + *ptr;
    return tsl_copy((char*)ptr, len);
}

extern "C" void tsl_assign(CellAccessor* accessor, char* dst, char* src, int32_t size_dst, int32_t size_src)
{
    if (size_dst != size_src)
    {
        auto offset = reinterpret_cast<int64_t>(dst) - accessor->cellPtr;
        char* cellPtr;
        ::CResizeCell(accessor->cellId, accessor->entryIndex, offset, size_src - size_dst, cellPtr);
        accessor->cellPtr = reinterpret_cast<int64_t>(cellPtr);
        dst = cellPtr + offset;
    }

    memcpy(dst, src, size_src);
}

extern "C" void tsl_setstring(CellAccessor* accessor, int32_t* p, u16char* str)
{
    auto tlen = *p;
    auto slen = wcslen(str) * sizeof(u16char);

    *p = slen;
    tsl_assign(accessor, (char*)(p + 1), (char*)str, tlen, slen);
}

extern "C" void tsl_setu8string(CellAccessor* accessor, int32_t* p, char* trinity_string_ptr)
{
    Trinity::String str(trinity_string_ptr);
    int32_t tlen = *p;
    int32_t slen = str.Length();

    *p = slen;
    tsl_assign(accessor, (char*)(p + 1), (char*)str.c_str(), tlen, slen);
}

// type-safe call
template <class TRET, class... TARGS>
CCFuncCall* safecall(X86Compiler &cc, TRET(*func)(TARGS...))
{
    return cc.call(imm_ptr(func), FuncSignatureT<TRET, TARGS...>());
}

void push(X86Compiler &cc, X86Gp& reg, std::vector<int32_t>& plan)
{
    for (auto i : plan)
    {
        if (i > 0)
        {
            cc.add(reg, i);
        }
        else
        {
            //TODO optional
            auto tmp = cc.newGpq("tmp");
            cc.movsxd(tmp, x86::ptr(reg, 0, sizeof(int32_t)));
            cc.add(reg, tmp);
            cc.add(reg, sizeof(int32_t));
        }
    }
}

std::vector<int32_t> walk(TypeDescriptor* type, MemberDescriptor* this_member = nullptr)
{
    std::vector<int32_t> plan;
    auto tc = type->get_TypeCode();
    auto tid = _get_typeid(type);

    if (tc == TypeCode::TC_STRING || tc == TypeCode::TC_U8STRING || tc == TypeCode::TC_LIST)
    {
        // jump
        plan.push_back(-1);
    }
    else if (tc == TypeCode::TC_CELL || tc == TypeCode::TC_STRUCT)
    {
        // walk
        auto members = type->get_Members();
        int32_t accumulated_size = 0;

        for (auto *m : *members)
        {
            // arrived at this member?
            if (m == this_member) break;

            //TODO optional
            auto sub_walk = walk(&m->Type);
            plan.insert(plan.cend(), sub_walk.begin(), sub_walk.end());
        }

        delete members;
    }
    else
    {
        plan.push_back(TypeId::sizeOf(tid));
    }

    return plan;
}

JitRoutine(BGet)
{
    auto tid = JitTypeId();
    auto tc = seq.CurrentType()->get_TypeCode();
    auto retreg = cc.newGpReg(tid);

    if (tc == TypeCode::TC_STRING)
    {
        auto call = safecall(cc, tsl_getstring);
        call->setArg(0, ctx.cellPtr);
        call->setRet(0, retreg);
    }
    else if (tc == TypeCode::TC_U8STRING)
    {
        auto call = safecall(cc, tsl_getu8string);
        call->setArg(0, ctx.cellPtr);
        call->setRet(0, retreg);
    }
    else if (tid != TypeId::kUIntPtr)
    {
        auto atomsize = asmjit::TypeId::sizeOf(tid);
        cc.mov(retreg, x86::ptr(ctx.cellPtr, 0, atomsize));
    }
    else
    {
        auto plan = walk(seq.CurrentType());

        if (plan.size() == 0) return; // empty struct
        else if (plan.size() == 1 && plan[0] == -1) // jump
        {
            auto call = safecall(cc, tsl_copy_dynamic);
            call->setArg(0, ctx.cellPtr);
            call->setRet(0, retreg);
        }
        else if (plan.size() > 1) // walking
        {
            // void* p = cellPtr;
            auto p = cc.newGpq("p");
            cc.mov(p, ctx.cellPtr);

            push(cc, p, plan);

            // p at end, calculate len
            cc.sub(p, ctx.cellPtr);
            auto call = safecall(cc, tsl_copy);
            call->setArg(0, ctx.cellPtr);
            call->setArg(1, p.as<X86Gpd>());
            call->setRet(0, retreg);
        }
        else //fixed copying
        {
            auto call = safecall(cc, tsl_copy);
            call->setArg(0, ctx.cellPtr);
            call->setArg(1, imm(plan[0]));
            call->setRet(0, retreg);
        }
    }

    ctx.ret(retreg);
}

JitRoutine(BSet)
{
    auto tid = JitTypeId();
    auto tc = seq.CurrentType()->get_TypeCode();
    auto src = cc.newGpReg(tid);
    ctx.addArg(src);

    if (tc == TypeCode::TC_STRING)
    {
        auto call = safecall(cc, tsl_setstring);
        call->setArg(0, ctx.cellAccessor);
        call->setArg(1, ctx.cellPtr);
        call->setArg(2, src);
    }
    else if (tc == TypeCode::TC_U8STRING)
    {
        auto call = safecall(cc, tsl_setu8string);
        call->setArg(0, ctx.cellAccessor);
        call->setArg(1, ctx.cellPtr);
        call->setArg(2, src);
    }
    else if (tid != TypeId::kUIntPtr)
    {
        // simple atom
        auto atomsize = asmjit::TypeId::sizeOf(tid);
        cc.mov(x86::ptr(ctx.cellPtr, 0, atomsize), src);
    }
    else
    {
        auto plan = walk(seq.CurrentType());

        if (plan.size() == 0) return; // empty struct
        else if (plan.size() == 1 && plan[0] == -1) // jump
        {
            auto src_len_gp = cc.newGpd("src_len");
            auto dst_len_gp = cc.newGpd("dst_len");

            //TODO optional

            // load length headers
            cc.mov(src_len_gp, x86::dword_ptr(src));
            cc.mov(dst_len_gp, x86::dword_ptr(ctx.cellPtr));
            // set length header of dst
            cc.mov(x86::dword_ptr(dst_len_gp), src_len_gp);
            cc.add(ctx.cellPtr, 4);
            cc.add(src, 4);

            auto call = safecall(cc, tsl_assign);
            call->setArg(0, ctx.cellAccessor);
            call->setArg(1, ctx.cellPtr);
            call->setArg(2, src);
            call->setArg(3, dst_len_gp);
            call->setArg(4, src_len_gp);
        }
        else if (plan.size() > 1) // walking
        {
            // void* p = src;
            // void* q = cellPtr
            auto p = cc.newGpq("p");
            auto q = cc.newGpq("q");
            cc.mov(p, src);
            cc.mov(q, ctx.cellPtr);
            push(cc, p, plan);
            push(cc, q, plan);
            // p & q at end, calculate len
            cc.sub(p, src);
            cc.sub(q, ctx.cellPtr);

            auto call = safecall(cc, tsl_assign);

            call->setArg(0, ctx.cellAccessor);
            call->setArg(1, ctx.cellPtr);
            call->setArg(2, src);
            call->setArg(3, q.r32());
            call->setArg(4, p.r32());
        }
        else //fixed copying
        {
            auto call = safecall(cc, memcpy);
            call->setArg(0, ctx.cellPtr);
            call->setArg(1, src);
            call->setArg(2, imm(plan[0]));
        }
    }

    ctx.ret();
}

JitRoutine(SGet)
{
    auto plan = walk(seq.ParentType(), seq.CurrentMember());
    push(cc, ctx.cellPtr, plan);
}

JitRoutine(SSet)
{
    SGet(cc, ctx, seq);
    BSet(cc, ctx, seq);
}

JitRoutine(GSGet)
{
    //TODO reflection layer for generic interfaces
    //TODO type converter
}

JitRoutine(GSSet)
{

}

int32_t calculate_shift(int32_t size)
{
    int bits = 0;
    int shift = 0;
    for (int i = 0; i < 32; ++i)
    {
        if (size & 1)
        {
            ++bits;
            shift = i;
        }

        size >>= 1;
    }

    if (bits != 1) return -1;
    else return shift;
}

JitRoutine(LGet)
{
    auto index = cc.newGpd("index");
    auto l = cc.newLabel();
    ctx.addArg(index);
    // jump over length header
    cc.add(ctx.cellPtr, sizeof(int32_t));

    auto plan = walk(seq.CurrentType());

    if (plan.size() == 1 && plan[0] > 0)
    {
        //fixed element type
        auto shift = calculate_shift(plan[0]);
        if (shift >= 0)
        {
            cc.lea(ctx.cellPtr, x86::ptr(ctx.cellPtr, index, shift));
        }
        //TODO else if (bits == 2)
        //maybe an opt pass for such things
        else
        {
            cc.imul(index, plan[0]);
            cc.lea(ctx.cellPtr, x86::ptr(index));
        }
    }
    else
    {
        //keep walking
        auto loop = cc.newGpd("loop");
        cc.xor_(loop, loop);

        cc.bind(l);
        push(cc, ctx.cellPtr, plan);
        cc.inc(loop);
        cc.cmp(loop, index);
        cc.jb(l);
    }
}

JitRoutine(LSet)
{
    LGet(cc, ctx, seq);
    BSet(cc, ctx, seq);
}

JitRoutine(LInlineGet)
{

}

JitRoutine(LInlineSet)
{

}

JitRoutine(LContains)
{

}

JitRoutine(LCount)
{
    auto plan = walk(seq.CurrentType());
    auto len = cc.newGpd("list_len");
    cc.mov(len, x86::dword_ptr(ctx.cellPtr));

    print("LCount element plan:");
    debug(plan.size());
    for (auto i : plan) debug(i);

    if (plan.size() == 1 && plan[0] > 0)
    {
        //fixed element type
        auto shift = calculate_shift(plan[0]);
        if (shift == 0)
        {
            //just return len
        }
        else if (shift > 0)
        {
            cc.shr(len, shift);
        }
        else
        {
            auto dummy   = cc.newInt32("dummy");
            auto divisor = cc.newInt32("size");
            cc.xor_(dummy, dummy);
            cc.mov(divisor, plan[0]);
            cc.idiv(dummy, len, divisor);
        }
    }
    else
    {
        //keep walking
        auto l = cc.newLabel();
        auto pend = cc.newGpq("pend");
        cc.lea(pend, x86::byte_ptr(ctx.cellPtr, len, /*no shift*/ 0, /*offset*/ sizeof(int32_t)));
        cc.xor_(len, len);
        cc.bind(l);
        push(cc, ctx.cellPtr, plan);
        cc.inc(len);
        cc.cmp(ctx.cellPtr, pend);
        cc.jb(l);
    }

    cc.ret(len);
}
