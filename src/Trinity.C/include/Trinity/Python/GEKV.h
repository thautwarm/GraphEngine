#pragma once
#include "TrinityCommon.h"
#include "Storage/MTHash/MTHash.h"
#include "Storage/MemoryTrunk/MemoryTrunk.h"
#include "Storage/LocalStorage/LocalMemoryStorage.h"
#include "Storage/LocalStorage/GCTask.h"
#include <iostream>
#include <diagnostics>
#include <Trinity/Hash/MD5.h>
DLL_EXPORT TrinityErrorCode __stdcall gpy_initialize(const char* storage_root, int32_t TrunkCount);
DLL_EXPORT TrinityErrorCode __stdcall gpy_save_cell(cellid_t cellId, char* buff, int32_t size, uint16_t cellType);
DLL_EXPORT TrinityErrorCode __stdcall gpy_update_cell(cellid_t cellId, char* buff, int32_t size);
DLL_EXPORT TrinityErrorCode __stdcall gpy_remove_cell(cellid_t cellId);
DLL_EXPORT TrinityErrorCode __stdcall gpy_get_locked_cell_info(cellid_t cellId, int32_t &size, uint16_t &type, char* &cellPtr, int32_t &entryIndex);
DLL_EXPORT VOID __stdcall gpy_release_cell_lock(cellid_t cellId, int32_t entryIndex);
DLL_EXPORT VOID __stdcall gpy_save_storage();
DLL_EXPORT VOID __stdcall gpy_load_storage();
DLL_EXPORT VOID __stdcall gpy_reset_storage();