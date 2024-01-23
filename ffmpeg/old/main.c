// PSP Libs
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <fcntl.h>
// FFmpeg Libs
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/avutil.h>
// SDL3 Libs
#include <SDL3/SDL.h>
// #include <SDL3/SDL_thread.h>

#define printf pspDebugScreenPrintf

PSP_MODULE_INFO("ffmpeg0.5", 0, 1, 0);
// Important! Support for VFPU
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER | PSP_THREAD_ATTR_VFPU);

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
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow(
        "window",
        480,
        272,
        0);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL, SDL_RENDERER_ACCELERATED);

    // FFmpeg desciptor "file:" + Absolute path
    // const char *input_file = "file:ms0:/VIDEO/2.mp4";
    const char *input_file = "file:ms0:/VIDEO/2.mp4";
    // const char *input_file = "file:host0:/eva.mp4";
    av_register_all();
    AVFormatContext *pFormatCtx = NULL;

    // Open video file
    if (av_open_input_file(&pFormatCtx, input_file, NULL, 0, NULL) != 0)
    {
        printf("open error");
    }

    if (av_find_stream_info(pFormatCtx) < 0)
    {
        printf("Stream info error");
    }

    int videoStream = -1;
    for (int i = 0; i < pFormatCtx->nb_streams; i++)
    {
        if (pFormatCtx->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO)
        {
            videoStream = i;
            break;
        }
    }

    if (videoStream == -1)
    {
        printf("video stream not found");
    }

    AVCodecContext *pCodecCtx = pFormatCtx->streams[videoStream]->codec;
    AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec == NULL)
    {
        printf("Unsupported codec\n");
        return -1;
    }

    if (avcodec_open(pCodecCtx, pCodec) < 0)
    {
        printf("Error opening codec\n");
        return -1;
    }

    AVFrame *pFrame = avcodec_alloc_frame();
    AVPacket packet;
    int frameFinished;

    SDL_Texture *texture = NULL;
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING, 480, 272);
    printf(SDL_GetError());

    struct SwsContext *sws_ctx = NULL;
    // frame transform...
    sws_ctx = sws_getContext(
        pCodecCtx->width,
        pCodecCtx->height,
        pCodecCtx->pix_fmt,
        480,
        272,
        PIX_FMT_YUV420P,
        SWS_BILINEAR,
        NULL,
        NULL,
        NULL);

    // create an image for display.

    // Calculate the size in bytes that a picture of the given width and height would occupy if stored in the given picture format.
    int numBytes = avpicture_get_size(PIX_FMT_YUV420P, 480, 272);

    // allocate the buffer.
    uint8_t *buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));

    // RGB frame.
    AVFrame *pFrameRGB = avcodec_alloc_frame();

    avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_YUV420P, 480, 272);

    while (av_read_frame(pFormatCtx, &packet) >= 0)
    {
        if (packet.stream_index == videoStream)
        {
            avcodec_decode_video(pCodecCtx, pFrame, &frameFinished, packet.data, packet.size);
            if (frameFinished)
            {
                printf(
                    "Frame %c (%d)",
                    av_get_pict_type_char(pFrame->pict_type),
                    pCodecCtx->frame_number);

                // pFrame now contains the decoded frame
                // Do something with the frame
                // printf("%d\n", pFrame->coded_picture_number);

                sws_scale(sws_ctx, (const uint8_t *const *)pFrame->data, pFrame->linesize,
                          0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);

                SDL_UpdateYUVTexture(
                    texture,
                    NULL,
                    pFrame->data[0],
                    pFrame->linesize[0],
                    pFrame->data[1],
                    pFrame->linesize[1],
                    pFrame->data[2],
                    pFrame->linesize[2]);
                SDL_FRect rect = {0, 0, 480, 272};
                SDL_RenderClear(renderer);
                SDL_RenderTexture(renderer, texture, NULL, NULL);
                SDL_RenderPresent(renderer);
            }
        }
    }

    av_free_packet(&packet);

    while (1)
        ;
    return 0;
}
