/**
 * 命令行播放视频
 * Simplest FFmpeg Decoder
 * Rjn 2019.6.30
 *
 * This software is a simplest decoder based on FFmpeg.
 * It decodes video to YUV pixel data.
 * It uses libavcodec and libavformat.
 * Suitable for beginner of FFmpeg.
 *
 */

#include <stdio.h>

#define __STDC_CONSTANT_MACROS

#ifdef _WIN32
//Windows
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
};
#else
//Linux...
#ifdef __cplusplus
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include "cframe.h"
#include <unistd.h>
#ifdef __cplusplus
};
#endif
#endif


int main(int argc, char* argv[])
{
    AVFormatContext *pFormatCtx;
    int i, videoindex;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    AVFrame *pFrame,*pFrameYUV;
    unsigned char *out_buffer;
    AVPacket *packet;
    int y_size;
    int ret, got_picture;
    struct SwsContext *img_convert_ctx;

    char* filepath;
    frame f;

    if(argc > 1){
        filepath = argv[1];
    }
    else{
        filepath = "Titanic.mkv";
    }

    //FILE *fp_yuv=fopen("output.yuv","wb+");

    av_register_all();
    avformat_network_init();
    pFormatCtx = avformat_alloc_context();
    printf("%s\r\n",filepath);
    if(avformat_open_input(&pFormatCtx,filepath,NULL,NULL)!=0){
        printf("Couldn't open input stream.\n");
        return -1;
    }
    if(avformat_find_stream_info(pFormatCtx,NULL)<0){
        printf("Couldn't find stream information.\n");
        return -1;
    }
    videoindex=-1;
    for(i=0; i<pFormatCtx->nb_streams; i++)
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO){
            videoindex=i;
            break;
        }

    if(videoindex==-1){
        printf("Didn't find a video stream.\n");
        return -1;
    }

    pCodecCtx=pFormatCtx->streams[videoindex]->codec;
    pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec==NULL){
        printf("Codec not found.\n");
        return -1;
    }
    if(avcodec_open2(pCodecCtx, pCodec,NULL)<0){
        printf("Could not open codec.\n");
        return -1;
    }

    pFrame=av_frame_alloc();
    pFrameYUV=av_frame_alloc();
    out_buffer=(unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P,  pCodecCtx->width, pCodecCtx->height,1));
    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize,out_buffer,
            AV_PIX_FMT_YUV420P,pCodecCtx->width, pCodecCtx->height,1);

    int w = pCodecCtx->width/2;
    int h = pCodecCtx->height/2;
    f = cf_malloc(&w, &h);

    packet=(AVPacket *)av_malloc(sizeof(AVPacket));
    //Output Info-----------------------------
    printf("--------------- File Information ----------------\n");
    av_dump_format(pFormatCtx,0,filepath,0);
    printf("-------------------------------------------------\n");
    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
            pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

    while(av_read_frame(pFormatCtx, packet)>=0){
        if(packet->stream_index==videoindex){
            ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
            if(ret < 0){
                printf("Decode Error.\n");
                return -1;
            }
            if(got_picture){
                sws_scale(img_convert_ctx, (const unsigned char* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
                        pFrameYUV->data, pFrameYUV->linesize);

                y_size=pCodecCtx->width*pCodecCtx->height;
                system("clear");
                unsigned char t = cf_threadholding_lt(f, pFrameYUV->data[0], pCodecCtx->width, pCodecCtx->height);
                cf_draw_av_Y_lt(f, pFrameYUV->data[0], pCodecCtx->width, pCodecCtx->height, t);
                cf_render(f);
                printf("012345678901234567890123456789012345678901234567890123456789  threshold =  %d\n", (int)t);
                cf_paint(f);
                usleep(50000);
                //fwrite(pFrameYUV->data[0],1,y_size,fp_yuv);    //Y 
                //printf("Succeed to decode 1 frame!\n");

            }
        }
        av_free_packet(packet);
    }
    //flush decoder
    //FIX: Flush Frames remained in Codec
    while (1) {
        ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
        if (ret < 0)
            break;
        if (!got_picture)
            break;
        sws_scale(img_convert_ctx, (const unsigned char* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, 
                pFrameYUV->data, pFrameYUV->linesize);

        int y_size=pCodecCtx->width*pCodecCtx->height;
        system("clear");
        cf_draw_av_Y_lt(f, pFrameYUV->data[0], pCodecCtx->width, pCodecCtx->height, 110);
        cf_render(f);
        printf("012345678901234567890123456789012345678901234567890123456789\n");
        cf_paint(f);
        usleep(50000);
        //fwrite(pFrameYUV->data[0],1,y_size,fp_yuv);    //Y 

        //printf("Flush Decoder: Succeed to decode 1 frame!\n");
    }

    sws_freeContext(img_convert_ctx);

    av_frame_free(&pFrameYUV);
    av_frame_free(&pFrame);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);

    return 0;
}
