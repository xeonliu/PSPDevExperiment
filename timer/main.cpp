#include "app.h"

PSP_MODULE_INFO("Timer", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

int exitCallback(int, int, void *)
{
    sceKernelExitGame();
    return 0;
}

int callbackThread(SceSize argc, void *argv)
{
    int cbid = sceKernelCreateCallback("CB", exitCallback, NULL);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return 0;
}

int setupCallbacks()
{
    int thid = sceKernelCreateThread("CBThread", callbackThread, 0x11, 0xFA0, 0, 0);
    sceKernelStartThread(thid, 0, 0);
    return thid;
}

int main(int argc, char *argv[])
{
    setupCallbacks();
    App app;
    app.run();
}