#include <pspkernel.h>

PSP_MODULE_INFO("supervisor", PSP_MODULE_KERNEL, 1, 1);
PSP_NO_CREATE_MAIN_THREAD();

int module_start(SceSize args, void *argp)
{
    return 0;
}

int module_stop(SceSize args, void *argp)
{
    return 0;
}

const char *sv_check_permissions(int client)
{
    const char *p;

    sceKernelDelayThread(10000);
    if (client == 123)
    {
        p = "yes";
    }
    else
    {
        p = "no";
    }
    return p;
}