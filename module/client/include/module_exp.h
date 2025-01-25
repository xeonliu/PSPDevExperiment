#pragma once
#include <pspsdk.h>
#include <stdlib.h>
#include <string.h>
#include <pspkernel.h>

#include "defs.h"

MemAddress pspModuleFindExportedNidAddr(SceModule *mod, const char *lib, u32 nid);
MemAddress pspModuleSetExportedNidToAddr(SceModule *mod, const char *lib, u32 nid, MemAddress newProcAddr);