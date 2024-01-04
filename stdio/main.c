#include <stdio.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>

#define printf pspDebugScreenPrintf

PSP_MODULE_INFO("StdIO Example", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);

int exit_callback(int arg1, int arg2, void *common)
{
    sceKernelExitGame();
    return 0;
}

int callback_thread(SceSize args, void *argp)
{
    int cbid = sceKernelCreateCallback("Exit Callback",
                                       exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return 0;
}

int setup_callbacks(void)
{
    int thid = sceKernelCreateThread("update_thread",
                                     callback_thread, 0x11, 0xFA0, 0, 0);

    if (thid >= 0)
        sceKernelStartThread(thid, 0, 0);
    return thid;
}

int main(void)
{
    // Use above functions to make exiting possible
    setup_callbacks();
    pspDebugScreenInit();
    pspDebugScreenSetXY(0, 0);
    FILE *filePointer;

    // Format:
    // absolute: "ms0:/"
    // relative: "./"
    filePointer = fopen("ms0:/VIDEO/example.txt", "w");

    if (filePointer == NULL)
    {
        printf("Unable to open file\n");
    }

    fprintf(filePointer, "Hello World!");

    while (1)
    {
    }

    return 0;
}