#include <pspsdk.h>
#include <stdlib.h>
#include <string.h>
#include <pspkernel.h>

#define PSPDEBUG
#define printf pspDebugScreenPrintf

#include "include/module_man.h"

SceModule *pspModuleLoadStartInKernelPart(const char *modpath)
{
    SceKernelLMOption option = {
        .size = sizeof(SceKernelLMOption),
        .mpidtext = (SceUID)1, // SceUID mpid = 1 Kernel Partition
        .mpiddata = (SceUID)1,
        .position = 0,
        .access = 1,
    };

    SceUID modid = sceKernelLoadModule(modpath, 0, &option);
    if (modid < 0)
    {
        dbg_log("sceKernelLoadModule(%s) failed with : %x\n", modpath, modid);
        return NULL;
    }

    int status = 0;
    int res = sceKernelStartModule(modid, 0, NULL, &status, NULL);
    (void)status;

    if (res < 0)
    {
        dbg_log("sceKernelStartModule(%s) failed with : %x\n", modpath, res);
        sceKernelUnloadModule(modid);
        return NULL;
    }

    return sceKernelFindModuleByUID(modid);
}

void print_module(SceModule *mod)
{
    if (!mod)
    {
        return;
    }

    if (strncmp(mod->modname, "sce", 3) != 0)
    {
        dbg_log("Module Name: %s\n", mod->modname);
        dbg_log("Module UID: %x\n", mod->modid);
        dbg_log("Module Start: %x\n", mod->text_addr);
        dbg_log("Module End: %x\n", mod->text_addr + mod->text_size);
        dbg_log("Module Segment Address: %x\n", mod->segmentaddr[0]);
    }
}

void pspForEachLoadedModule(void (*callback)(SceModule *))
{
    SceUID ids[512];
    memset(ids, 0, 512 * sizeof(SceUID));

    int count = 0;
    sceKernelGetModuleIdList(ids, 512, &count);

    int p;
    for (p = 0; p < count; p++)
    {
        SceModule *mod = sceKernelFindModuleByUID(ids[p]);
        if (mod)
        {
            callback(mod);
        }
    }
}
