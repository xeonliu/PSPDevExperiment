#include <pspsdk.h>
#include <stdlib.h>
#include <string.h>
#include <pspkernel.h>

#include "include/module_exp.h"

static MemAddress _pspModuleExportHelper(SceModule *mod, const char *lib, u32 nid, u32 newProcAddr);

MemAddress pspModuleFindExportedNidAddr(SceModule *mod, const char *lib, u32 nid)
{
    return _pspModuleExportHelper(mod, lib, nid, (u32)NULL);
}

MemAddress pspModuleSetExportedNidToAddr(SceModule *mod, const char *lib, u32 nid, MemAddress newProcAddr)
{
    return _pspModuleExportHelper(mod, lib, nid, (u32)newProcAddr);
}

static MemAddress _pspModuleExportHelper(SceModule *mod, const char *lib, u32 nid, u32 newProcAddr)
{
    if (!mod)
    {
        return (MemAddress)NULL;
    }

    MemAddress ent_next = (MemAddress)mod->ent_top;
    // ent_size is in bytes
    MemAddress ent_end = (MemAddress)mod->ent_top + (mod->ent_size >> 2);

    while (ent_next < ent_end)
    {
        SceLibraryEntryTable *ent = (SceLibraryEntryTable *)ent_next;

        if (ent->libname && strcmp(ent->libname, lib) == 0)
        {
            int count = ent->stubcount + ent->vstubcount;
            PspNID *nid_table = (PspNID *)ent->entrytable;
            int i;
            for (i = 0; i < count; ++i)
            {
                if (nid_table[i] == nid)
                {
                    MemAddress procAddr = (MemAddress)nid_table[count + i];
                    if (newProcAddr)
                    {
                        // Overwrite the address
                        nid_table[count + i] = (PspNID)newProcAddr;
                    }
                    return (MemAddress)procAddr;
                }
            }
            return (MemAddress)NULL;
        }
        ent_next += (ent->len);
    }
    return (MemAddress)NULL;
}
