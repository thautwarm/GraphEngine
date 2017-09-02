#include "Trinity/Python/GraphEngine.h"

DLL_EXPORT VOID __stdcall gpy_initialize(const char* storage_root, int32_t TrunkCount)
{
	TrinityConfig::SetTrunkCount(TrunkCount);
	TrinityConfig::SetStorageRoot(storage_root);
	Storage::LocalMemoryStorage::Initialize();
}

