#include "Trinity/Python/GEKV.h"

DLL_EXPORT TrinityErrorCode __stdcall gpy_initialize(const char* storage_root, int32_t TrunkCount)
{
	TrinityConfig::SetTrunkCount(TrunkCount);
	TrinityConfig::SetStorageRoot(storage_root);
	return Storage::LocalMemoryStorage::Initialize();
}

DLL_EXPORT TrinityErrorCode __stdcall gpy_save_cell(cellid_t cellId, char* buff, int32_t size, uint16_t cellType)
{
	return Storage::LocalMemoryStorage::SaveCell(cellId, buff, size, cellType);
}

DLL_EXPORT TrinityErrorCode __stdcall gpy_update_cell(cellid_t cellId, char* buff, int32_t size)
{
	return Storage::LocalMemoryStorage::UpdateCell(cellId, buff, size);
}

DLL_EXPORT TrinityErrorCode __stdcall gpy_remove_cell(cellid_t cellId)
{
	return Storage::LocalMemoryStorage::RemoveCell(cellId);
}

DLL_EXPORT TrinityErrorCode __stdcall gpy_get_locked_cell_info(cellid_t cellId, int32_t &size, uint16_t &type, char* &cellPtr, int32_t &entryIndex)
{
	return Storage::LocalMemoryStorage::CGetLockedCellInfo4CellAccessor(cellId, size, type, cellPtr, entryIndex);
}

DLL_EXPORT VOID __stdcall gpy_release_cell_lock(cellid_t cellId, int32_t entryIndex)
{
	Storage::LocalMemoryStorage::ReleaseCellLock(cellId, entryIndex);
}

DLL_EXPORT VOID __stdcall gpy_save_storage()
{
	Storage::LocalMemoryStorage::SaveStorage();
}

DLL_EXPORT VOID __stdcall gpy_load_storage()
{
	Storage::LocalMemoryStorage::LoadStorage();
}

DLL_EXPORT VOID __stdcall gpy_reset_storage()
{
	Storage::LocalMemoryStorage::ResetStorage();
}
