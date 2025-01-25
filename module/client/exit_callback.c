#include "include/exit.h"

static int _app_exit_callback(int arg1, int arg2, void *common)
{
    sceKernelExitGame();
    return 0;
}

static int _app_ExitCallbackThreadProc(SceSize args, void *argp)
{
    int cbid = sceKernelCreateCallback("Exit Callback", _app_exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return 0;
}

int AppSetupExitCallback(void)
{
    int thid = sceKernelCreateThread("Exit Callback Thread", _app_ExitCallbackThreadProc, 0x11, 0xFA0, THREAD_ATTR_USER, 0);
    if (thid >= 0)
    {
        sceKernelStartThread(thid, 0, 0);
    }
    return thid;
}