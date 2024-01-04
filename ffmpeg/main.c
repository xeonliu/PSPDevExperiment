#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <fcntl.h>
// FFmpeg Libs
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/avutil.h>

// A temparary fix. stdc in PSP seems do not support dup.
// int dup(int oldfd)
// {
//     return 1;
// }

#define printf pspDebugScreenPrintf

PSP_MODULE_INFO("ffmpeg Example", 0, 1, 0);
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

    // FFmpeg desciptor "file:" + Absolute path
    const char *input_file = "file:ms0:/VIDEO/2.mp4";

    // Allocate AVFormatContext
    AVFormatContext *format_ctx = avformat_alloc_context();

    int err = 0;
    char errbuf[AV_ERROR_MAX_STRING_SIZE];
    if ((err = avformat_open_input(&format_ctx, input_file, NULL, NULL)) != 0)
    {
        av_strerror(err, errbuf, AV_ERROR_MAX_STRING_SIZE);
        printf("ERROR CODE: %s\n", errbuf);
        return 0;
    }
    else
    {
        printf("Read Success.\n");
    }

    if (err = (avformat_find_stream_info(format_ctx, NULL)) < 0)
    {
        av_strerror(err, errbuf, AV_ERROR_MAX_STRING_SIZE);
        printf("ERROR CODE: %s\n", errbuf);
        avformat_close_input(&format_ctx);
        return 0;
    }

    printf("find steam info.\n");

    int videoStreamIndex = -1;
    for (int i = 0; i < format_ctx->nb_streams; i++)
    {
        if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoStreamIndex = i;
            break;
        }
    }

    printf("Video Codec: %s\n", avcodec_get_name(format_ctx->streams[videoStreamIndex]->codecpar->codec_id));

    AVCodec *pCodec = NULL;
    if (pCodec = avcodec_find_decoder(format_ctx->streams[videoStreamIndex]->codecpar->codec_id) == NULL)
    {
        printf("Codec not found\n");
        return -1;
    }

    printf("FFMpeg Version: %s\n", av_version_info());

    while (1)
    {
    }

    return 0;
}