#pragma once
#include "defs.h"

SceModule *pspModuleLoadStartInKernelPart(const char *modpath);
void print_module(SceModule *mod);
void pspForEachLoadedModule(void (*callback)(SceModule *));