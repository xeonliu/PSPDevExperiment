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
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>

#include "packet.h"

#define printf pspDebugScreenPrintf

PacketQueue audioq;

/// @brief
/// @param aCodecCtx
/// @param audio_buf
/// @param buf_size size of the audio buffer
/// @return
static int audio_decode_frame(AVCodecContext *aCodecCtx, uint8_t *audio_buf, int buf_size)
{
    static AVPacket pkt;
    static uint8_t *audio_pkt_data = NULL;
    static int audio_pkt_size = 0;
    static AVFrame frame;
    // AVCodecContext *dec = is->audio_st->codec;   //aCodecCtx
    int n, len1, data_size;

    for (;;)
    {
        /* NOTE: the audio packet can contain several frames */
        while (audio_pkt_size > 0)
        {
            data_size = buf_size;
            // Read from packet to audio_buf
            // If no frame could be decompressed, frame_size_ptr is zero. Otherwise, it is the decompressed frame size in bytes.
            len1 = avcodec_decode_audio2(aCodecCtx,
                                         (int16_t *)audio_buf, &data_size,
                                         audio_pkt_data, audio_pkt_size);
            if (len1 < 0)
            {
                /* if error, we skip the frame */
                audio_pkt_size = 0;
                break;
            }

            audio_pkt_data += len1;
            audio_pkt_size -= len1;
            if (data_size <= 0)
                continue;
            return data_size;
        }
        if (pkt.data)
        {
            av_free_packet(&pkt);
        }
        if (packet_queue_get(&audioq, &pkt, 1) < 0)
        {
            return -1;
        }
        audio_pkt_data = pkt.data;
        audio_pkt_size = pkt.size;
    }
}

void audio_callback(void *userdata, Uint8 *stream, int len)
{

    AVCodecContext *aCodecCtx = (AVCodecContext *)userdata;
    int len1, audio_size;

    // audio buffer created here
    static uint8_t audio_buf[(AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2];
    static unsigned int audio_buf_size = 0;
    static unsigned int audio_buf_index = 0;

    while (len > 0)
    {
        if (audio_buf_index >= audio_buf_size)
        {
            /* We have already sent all our data; get more */
            // decodes one audio frame and returns its uncompressed size.
            audio_size = audio_decode_frame(aCodecCtx, audio_buf,
                                            sizeof(audio_buf));
            if (audio_size < 0)
            {
                /* If error, output silence */
                audio_buf_size = 1024;
                memset(audio_buf, 0, audio_buf_size);
            }
            else
            {
                audio_buf_size = audio_size;
            }
            audio_buf_index = 0;
        }
        len1 = audio_buf_size - audio_buf_index;
        if (len1 > len)
            len1 = len;
        memcpy(stream, (uint8_t *)audio_buf + audio_buf_index, len1);
        len -= len1;
        stream += len1;
        audio_buf_index += len1;
    }
}

int main(int argc, char **argv)
{
    pspDebugScreenInit();
    pspDebugScreenSetXY(0, 0);
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow(
        "window",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        480, 272, 0);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

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

    /// Get Video & Audio Stream.

    int videoStream = -1;
    int audioStream = -1;
    for (int i = 0; i < pFormatCtx->nb_streams; i++)
    {
        // Get video stream.
        if (pFormatCtx->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO)
        {
            videoStream = i;
        }

        // Get audio stream.
        if (pFormatCtx->streams[i]->codec->codec_type == CODEC_TYPE_AUDIO)
        {
            audioStream = i;
        }
    }

    if (videoStream == -1)
    {
        printf("video stream not found");
    }

    if (audioStream == -1)
    {
        printf("Audio Stream not found.");
    }

    // Get Video & Audio's Codec Context
    AVCodecContext *pCodecCtx = pFormatCtx->streams[videoStream]->codec;
    AVCodecContext *aCodecCtx = pFormatCtx->streams[audioStream]->codec;

    // Open Video Codec
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

    // Open Audio Codec
    AVCodec *aCodec = avcodec_find_decoder(aCodecCtx->codec_id);
    if (!aCodec)
    {
        printf("Unsupported Audio Codec");
    }

    if (avcodec_open(aCodecCtx, aCodec) < 0)
    {
        printf("Error opening audio coedc\n");
    }

    // Set audio specs.
    SDL_AudioSpec wanted_spec;
    wanted_spec.freq = aCodecCtx->sample_rate;
    wanted_spec.format = AUDIO_S16;
    wanted_spec.channels = aCodecCtx->channels;
    wanted_spec.callback = audio_callback;
    wanted_spec.userdata = aCodecCtx;
    wanted_spec.silence = 0;

    SDL_AudioSpec real_spec;
    SDL_AudioDeviceID devid; // How to get the device id before start decoding?

    // Callback func `audio_callback`
    // SDL_PauseAudioDevice(devid);
    devid = SDL_OpenAudioDevice(NULL, 0, &wanted_spec, &real_spec, 0);
    printf("Audio Device Open %d", devid);

    // printf(SDL_GetError());
    /* Frame transform thing*/
    // struct SwsContext *sws_ctx = NULL;
    // // frame transform...
    // sws_ctx = sws_getContext(
    //     pCodecCtx->width,
    //     pCodecCtx->height,
    //     pCodecCtx->pix_fmt,
    //     480,
    //     272,
    //     PIX_FMT_YUV420P,
    //     SWS_BILINEAR,
    //     NULL,
    //     NULL,
    //     NULL);
    // create an image for display.
    // Calculate the size in bytes that a picture of the given width and height would occupy if stored in the given picture format.
    // int numBytes = avpicture_get_size(PIX_FMT_YUV420P, 480, 272);
    // allocate the buffer.
    // uint8_t *buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));
    // AVFrame *pFrameRGB = avcodec_alloc_frame();
    // avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_YUV420P, 480, 272);

    AVFrame *pFrame = avcodec_alloc_frame();
    int frameFinished;

    SDL_Texture *texture = NULL;
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING, 480, 272);

    AVPacket packet;
    packet_queue_init(&audioq);
    SDL_PauseAudioDevice(devid, 0);
    while (av_read_frame(pFormatCtx, &packet) >= 0)
    {
        // Video Packet????
        if (packet.stream_index == videoStream)
        {
            avcodec_decode_video(pCodecCtx, pFrame, &frameFinished, packet.data, packet.size);
            if (frameFinished)
            {
                // printf(
                //     "Frame %c (%d): ",
                //     av_get_pict_type_char(pFrame->pict_type),
                //     pCodecCtx->frame_number);

                // sws_scale(sws_ctx, (const uint8_t *const *)pFrame->data, pFrame->linesize,
                //           0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);

                // SDL_UpdateYUVTexture(
                //     texture,
                //     NULL,
                //     pFrame->data[0],
                //     pFrame->linesize[0],
                //     pFrame->data[1],
                //     pFrame->linesize[1],
                //     pFrame->data[2],
                //     pFrame->linesize[2]);
                // // SDL_FRect rect = {0, 0, 480, 272};
                // SDL_RenderClear(renderer);
                // SDL_RenderCopy(renderer, texture, NULL, NULL);
                // SDL_RenderPresent(renderer);
            }
        }
        else if (packet.stream_index == audioStream)
        {
            // we don't free the packet after we put it in the queue. We'll free it later when we decode it.
            packet_queue_put(&audioq, &packet);
            printf("%d\n", audioq.nb_packets);
        }
        else
        {
            av_free_packet(&packet);
        }
    }

    while (1)
        ;
    return 0;
}
