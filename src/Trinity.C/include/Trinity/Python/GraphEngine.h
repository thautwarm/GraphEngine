#pragma once
#include "TrinityCommon.h"
#include "Storage/MTHash/MTHash.h"
#include "Storage/MemoryTrunk/MemoryTrunk.h"
#include "Storage/LocalStorage/LocalMemoryStorage.h"
#include "Storage/LocalStorage/GCTask.h"
#include <iostream>
#include <diagnostics>
#include <Trinity/Hash/MD5.h>
DLL_EXPORT VOID __stdcall gpy_initialize(const char* storage_root, int32_t TrunkCount = 256);