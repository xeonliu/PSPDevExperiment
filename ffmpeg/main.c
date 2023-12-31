#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>

#define printf pspDebugScreenPrintf
#define LIBAVCODEC_VERSION avcodec_version()
#define LIBAVUTIL_VERSION avutil_version()
#define LIBAVFORMAT_VERSION avformat_version()
// PSP_MODULE_INFO is required
PSP_MODULE_INFO("Hello World", 0, 1, 0);
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

    // Print Hello World! on a debug screen on a loop
    pspDebugScreenInit();
    av_register_all();
    AVFormatContext *ctx;
    avformat_alloc_context();
    while (1)
    {
        pspDebugScreenSetXY(0, 0);
        // printf("libavcodec version: %d\n", avcodec_version());
        // printf("libavutil version: %d\n", avutil_version());
        // printf("libavformat version: %d\n", avformat_version());

        printf("libavcodec version: %d.%d.%d\n",
               LIBAVCODEC_VERSION >> 16,
               (LIBAVCODEC_VERSION >> 8) & 0xFF,
               LIBAVCODEC_VERSION & 0xFF);

        printf("libavutil version: %d.%d.%d\n",
               LIBAVUTIL_VERSION >> 16,
               (LIBAVUTIL_VERSION >> 8) & 0xFF,
               LIBAVUTIL_VERSION & 0xFF);

        printf("libavformat version: %d.%d.%d\n",
               LIBAVFORMAT_VERSION >> 16,
               (LIBAVFORMAT_VERSION >> 8) & 0xFF,
               LIBAVFORMAT_VERSION & 0xFF);

        sceDisplayWaitVblankStart();
    }

    return 0;
}