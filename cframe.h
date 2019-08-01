#ifndef _CFRAME_H_
#define _CFRAME_H_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "cfconst.h"

char* pc = " ⠁⠂⠃⠄⠅⠆⠇⡀⡁⡂⡃⡄⡅⡆⡇⠈⠉⠊⠋⠌⠍⠎⠏⡈⡉⡊⡋⡌⡍⡎⡏⠐⠑⠒⠓⠔⠕⠖⠗⡐⡑⡒⡓⡔⡕⡖⡗⠘⠙⠚⠛⠜⠝⠞⠟⡘⡙⡚⡛⡜⡝⡞⡟⠠⠡⠢⠣⠤⠥⠦⠧⡠⡡⡢⡣⡤⡥⡦⡧⠨⠩⠪⠫⠬⠭⠮⠯⡨⡩⡪⡫⡬⡭⡮⡯⠰⠱⠲⠳⠴⠵⠶⠷⡰⡱⡲⡳⡴⡵⡶⡷⠸⠹⠺⠻⠼⠽⠾⠿⡸⡹⡺⡻⡼⡽⡾⡿⢀⢁⢂⢃⢄⢅⢆⢇⣀⣁⣂⣃⣄⣅⣆⣇⢈⢉⢊⢋⢌⢍⢎⢏⣈⣉⣊⣋⣌⣍⣎⣏⢐⢑⢒⢓⢔⢕⢖⢗⣐⣑⣒⣓⣔⣕⣖⣗⢘⢙⢚⢛⢜⢝⢞⢟⣘⣙⣚⣛⣜⣝⣞⣟⢠⢡⢢⢣⢤⢥⢦⢧⣠⣡⣢⣣⣤⣥⣦⣧⢨⢩⢪⢫⢬⢭⢮⢯⣨⣩⣪⣫⣬⣭⣮⣯⢰⢱⢲⢳⢴⢵⢶⢷⣰⣱⣲⣳⣴⣵⣶⣷⢸⢹⢺⢻⢼⢽⢾⢿⣸⣹⣺⣻⣼⣽⣾⣿";

struct frame_struct{
    int* data;
    char* image;
    int width;
    int height;
    int pixel_count;
    int data_width;
    int data_height;
    int data_count;
};
typedef struct frame_struct* frame;

int cf_empty(frame f)
{
    if( f == NULL)
        return ERROR_NULL_POINTER;
    int i;
    for(i = 0; i < f->data_count; i++)
    {
        f->data[i] = 0;
    }
    return SUCCESS;
}

frame cf_malloc(int* width_ptr, int* height_ptr)
{
    if (*width_ptr < 2 || *height_ptr < 4)
        return NULL;
    frame f = (frame)malloc(sizeof(struct frame_struct));
    *width_ptr = *width_ptr%2 == 0 ? *width_ptr : *width_ptr-1;
    *height_ptr = *height_ptr%4 == 0 ? *height_ptr : *height_ptr/4*4;

    f->width = *width_ptr;
    f->height = *height_ptr;
    f->pixel_count = f->width*f->height;
    f->data_width = f->width / 2;
    f->data_height = f->height / 4;
    f->data_count = f->pixel_count/8;
    f->data = (int*)malloc(sizeof(int)*f->data_count);
    f->image = (char*)malloc(f->data_count*3 + f->height/4 + 1);
    cf_empty(f);   
    return f;
}

int cf_render(frame f)
{
    if( f == NULL)
        return ERROR_NULL_POINTER;
    int i;
    int ptr = 0;
    for(i = 0; i < f->data_count; i++)
    {
        if(f->data[i] == 0){
            f->image[ptr] = ' ';
            ptr += 1;
        }else{
            memcpy(f->image+ptr, pc+f->data[i]*3-2, 3);
            ptr += 3;
        }
        if( (i+1)%f->data_width == 0) // reach the end of line
        {
            f->image[ptr] = '\n';
            ptr += 1;
        }
    }
    f->image[ptr] = '\0';
    return SUCCESS;
}

void cf_paint(frame f)
{
    printf("%s", f->image);
}

int cf_edge(frame f)
{
    if( f == NULL)
        return ERROR_NULL_POINTER;
    int i;
    int x, y;
    for(i = 0; i < f->data_count; i++)
    {
        x = i%f->data_width;
        y = i/f->data_width;
        if(x == 0){
            if( y == 0)
                f->data[i] = f->data[i] | 31;
            else if(y == f->data_height-1)
                f->data[i] = f->data[i] | 143;
            else
                f->data[i] = f->data[i] | 15;
        }else if(x == f->data_width-1){
            if( y == 0)
                f->data[i] = f->data[i] | 241;
            else if(y == f->data_height-1)
                f->data[i] = f->data[i] | 248;
            else
                f->data[i] = f->data[i] | 240;
        }else if(y == 0){
            f->data[i] = f->data[i] | 17;
        }else if(y == f->data_height-1){
            f->data[i] = f->data[i] | 136;
        }else{
            /* nothing to do */
        }
    }
    return SUCCESS;
}

int cf_fill(frame f)
{
    if( f == NULL)
        return ERROR_NULL_POINTER;
    int i;
    for(i=0; i < f->data_count; i++)
        f->data[i] = 255;
    return SUCCESS;
}

void cf_draw_pixel(frame f, int px, int py)
{
    if(px >= f->width || py >= f->height || px < 0 || py < 0)
        return;
    int data_x = px/2;
    int data_y = py/4;
    int index = data_y*f->data_width + data_x;
    int offset = 1;
    offset = offset << ( py & 3 ) << ( (px & 1)<<2 );
    f->data[index] = f->data[index] | offset;
}

void cf_draw_line(frame f, int px_s, int py_s, int px_d, int py_d)
{
    double slope = ((double)py_d - py_s)/(px_d-px_s);
    int px = px_s;
    double py = (double)py_s;
    px_d = px_d < f->width?px_d:f->width-1;
    for(; px <= px_d; px++)
    {
        py += slope;
        cf_draw_pixel(f, px, (int)(py+0.5));
    }
}

int cf_erase_pixel(frame f, int px, int py)
{
    if(px >= f->width || py >= f->height || px < 0 || py < 0)
        return ERROR_OUT_OF_BOUNDARY;
    int data_x = px/2;
    int data_y = py/4;
    int index = data_y*f->data_width + data_x;
    int offset = 1;
    offset = offset << ( py & 3 ) << ( (px & 1)<<2 );
    f->data[index] = f->data[index] & ~offset;
    return SUCCESS;
}

int cf_draw_av_Y_lt(frame f, unsigned char* av_data_y, int av_width, int av_height, unsigned char threshold)
{
    if(av_width <= 0 || av_height <= 0)
        return ERROR_INVAILABLE_BOUNDARY;
    int i, j;
    int blk_w = av_width/f->width;
    int blk_h = (av_height/f->height) << 2;
    int px = 0;
    int py = 0;
    int data_l;
    int data_h;
    int data_x = 0;
    for(i = 0; i < f->data_count; i++)
    {
        f->data[i] = 0;
        data_l = 1;
        data_h = 16;
        int data_index = px + py*av_width;
        for(j = 0; j < 4; j++)
        {
            if(av_data_y[data_index] > threshold)
                f->data[i] = f->data[i] | data_l;
            if(av_data_y[data_index+blk_w] > threshold)
                f->data[i] = f->data[i] | data_h;
            data_index += av_width;
            data_l = data_l << 1;
            data_h = data_h << 1;
        }
        data_x ++;
        px += blk_w << 1;
        if(data_x == f->data_width)
        {
            data_x = 0;
            px = 0;
            py += blk_h;
        }
    }
}

unsigned char cf_threadholding_lt(frame f, unsigned char* av_data_y, int av_width, int av_height)
{
    int blk_w = av_width/f->width;
    int blk_h = av_height/f->height;
    int tail = av_width - f->width * blk_w;
    int histograme[256];
    memset(histograme, '\0', 256*sizeof(int));
    int y, x, data_index;
    data_index = 0;
    for(y = 0; y < f->height; y ++)
    {
        for( x = 0; x < f->width; x ++)
        {
            histograme[av_data_y[data_index]] ++;
            data_index += blk_w;
        }
        data_index += tail;
    }

    int amount = 0;
    int pixel_back = 0, pixel_fore = 0;
    int pixel_integral_back = 0, pixel_integral_fore = 0, pixel_integral = 0;
    double omega_back, omega_fore, micro_back, micro_fore, sigmab, sigma;              // 类间方差;
    int min_value, max_value;
    int threshold = 0;

    for (min_value = 0; min_value < 256 && histograme[min_value] == 0; min_value++) ;
    for (max_value = 255; max_value > min_value && histograme[max_value] == 0; max_value--) ;
    if (max_value == min_value) return (unsigned char)max_value;          // 图像中只有一个颜色             
    if (min_value + 1 == max_value) return (unsigned char)min_value;      // 图像中只有二个颜色

    for (y = min_value; y <= max_value; y++) amount += histograme[y];        //  像素总数

    pixel_integral = 0;
    for (y = min_value; y <= max_value; y++) pixel_integral += histograme[y] * y;
    sigmab = -1;
    for (y = min_value; y < max_value; y++)
    {
        pixel_back = pixel_back + histograme[y];
        pixel_fore = amount - pixel_back;
        omega_back = (double)pixel_back / amount;
        omega_fore = (double)pixel_fore / amount;
        pixel_integral_back += histograme[y] * y;
        pixel_integral_fore = pixel_integral - pixel_integral_back;
        micro_back = (double)pixel_integral_back / pixel_back;
        micro_fore = (double)pixel_integral_fore / pixel_fore;
        sigma = omega_back * omega_fore * (micro_back - micro_fore) * (micro_back - micro_fore);
        if (sigma > sigmab)
        {
            sigmab = sigma;
            threshold = y;
        }
    }
    return (unsigned char)threshold;
}

#endif
