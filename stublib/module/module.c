/*
Adapted from https://github.com/mallgrab/psp-profiler-prx
*/

#include <pspkernel.h>
#include <pspdebug.h>
#include <stdio.h>

PSP_MODULE_INFO("Module", PSP_MODULE_KERNEL, 1, 1);
// PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);

static int a = 1;

int add()
{
    return a++;
}

#define WELCOME_MESSAGE "\n"

int module_start(SceSize args, void *argp)
{
    printf(WELCOME_MESSAGE); // Essential??? Otherwise fixup-import error.
    return 0;
}

int module_stop()
{
    return 0;
}