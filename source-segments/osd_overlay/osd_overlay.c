#include "osd_overlay.h"

/**************changed part**************/
extern const GUI_FONT GUI_Fontfont_kaiti_30;
static const GUI_FONT *g_font_ctx = &GUI_Fontfont_kaiti_30;

/**************fixed part****************/
static uint8_t g_osd_gray[OSD_STR_SIZE * OSD_CHAR_SIZE1] = {0};
static uint8_t g_osd_rgb[OSD_STR_SIZE * OSD_CHAR_SIZE2]  = {0};
static uint8_t g_osd_yuyv[OSD_STR_SIZE * OSD_CHAR_SIZE3] = {0};
static charinfo_t g_osd_chars[OSD_STR_SIZE];
static uint16_t g_osd_char_cnt = 0,g_osd_width = 0;

/**************rgb2yuv*******************/
static void osd_rgb2yuyv_pixel(uint8_t b,uint8_t g,uint8_t r,uint8_t *y,uint8_t *u,uint8_t *v){
    assert(y != NULL && u != NULL && v != NULL);

    *y = 0.29900 * r + 0.5870 * g + 0.1140 * b;
    *u = -0.1687 * r - 0.3313 * g + 0.5000 * b + 128;
    *v = 0.50000 * r - 0.4187 * g - 0.0813 * b + 128;

}

static void osd_rgb2yuv(uint8_t *pRgb, uint8_t *pYuv, int width, int height){
    assert(pRgb != NULL && pYuv != NULL);

    uint8_t *ptr1 = NULL,*ptr2 = NULL;
    for (int h = 0; h < height; h++)
    {
        ptr1 = pRgb + h * width * 3;
        ptr2 = pYuv + h * width * 2;

        for (int w = 0; w < width; w += 2)
        {
            uint8_t y1, u1, v1, y2, u2, v2;
            osd_rgb2yuyv_pixel(ptr1[0], ptr1[1], ptr1[2], &y1, &u1, &v1);
            osd_rgb2yuyv_pixel(ptr1[3], ptr1[4], ptr1[5], &y2, &u2, &v2);
            uint8_t u = (u1 + u2) / 2;
            uint8_t v = (v1 + v2) / 2;
            *ptr2++ = y1;
            *ptr2++ = u;
            *ptr2++ = y2;
            *ptr2++ = v;
            ptr1 += 6;
        }
    }
}

static const GUI_CHARINFO *osd_get_charinfo(uint8_t code){
    uint16_t off = 0;
    const GUI_FONT_PROP  *pProp = g_font_ctx->p.pProp;
    while(pProp != NULL){
        if(code >= (pProp->First) && code <= (pProp->Last)){
            off = code - (pProp->First);
            return &pProp->paCharInfo[off];

        }
        pProp = pProp->pNext;
    }
    return NULL;
}

#if OSD_PRINT_DEBUG
/**************for debug*****************/
/* 
 * print the dot in console
 * param : data the start address of dot
 * param : width the width of data
 * param : height the heigth of data
 */
static void osd_print_str(uint8_t *data,int width,int height){
    int i = 0;
    while(i < (width * height)){
        if(data[i] == 0x00){
            printf("_");

        }else
            printf("x");
        i++;

        if(i % width == 0){
            printf("\n");
        }
    }
}
#endif

/* 
 * save the file
 */
void osd_save_file(uint8_t*filename,uint8_t *buffer,uint32_t size){
    int fd;
    fd = open((char *)filename,O_CREAT|O_RDWR|O_TRUNC,0666);
    if(fd < 0){
        printf("osd : open %s failed\n",filename);
        return;
    }
    write(fd,(char *)buffer,(size_t)size);
    close(fd);
}

/*
 * query the dots of str in font file
 * then save in g_osd_chars
 */
static int osd_search_char(uint8_t *str){
    const GUI_CHARINFO *char_info;
    int i = 0;
    int osd_off = 0;
    while(*str){
        char_info = osd_get_charinfo(*str);
        if(char_info == NULL){
            return -1;
        }
        g_osd_chars[i].char_width = char_info->BytesPerLine;
        g_osd_chars[i].char_height = g_font_ctx->YSize;
        /* input buffer : character dot info */
        g_osd_chars[i].line_data_src = char_info->pData;
        /* record the first byte addr in output buffer */
        g_osd_chars[i].line_data_dst = g_osd_gray + osd_off;
        g_osd_width += g_osd_chars[i].char_width;
        osd_off = g_osd_width * 8;
        str++;
        i++;
    }
    g_osd_char_cnt = i;
    return 0;
}

/*
 * construct a line of a character
 */
static void osd_make_char_line(uint8_t *dst,const uint8_t*src,int width){
    int i = 0, j = 0;
    for(i = 0; i < width; i++){
        /* a byte represent 8 dot */
        for(j = 0; j < 8;j++){
            if((src[i]>>(7-j)) & 0x01){
                dst[i*8+j] = 0xff;
            }
        }
    }
}

/*
 * construct gray osd
 * param : str want to transfer
 */
static int32_t osd_make_gray_str(uint8_t *str){
    uint8_t font_height;
    uint8_t char_width,*line_data_dst;
    uint8_t const *line_data_src;
    uint16_t i,j;
    int32_t ret;

    font_height = g_font_ctx->YSize;
    memset(g_osd_gray,0x00,sizeof(g_osd_gray));

    ret = osd_search_char(str);
    if(ret < 0){
        printf("osd : can not find dot in fontlib\n");
        return -1;
    }

    for(j = 0; j < g_osd_char_cnt; j++)
        for(i = 0 ; i < font_height; i++){
            /* read 1 line of all characters */
            char_width    = g_osd_chars[j].char_width;
            /* the line start addr from source */
            line_data_src = g_osd_chars[j].line_data_src + i * char_width;
            /* the line start addr from dest buffer */
            line_data_dst = g_osd_chars[j].line_data_dst + i * g_osd_width * 8;
            osd_make_char_line(line_data_dst,line_data_src,char_width);

        }
    /* print string in console */
#if OSD_PRINT_DEBUG
    osd_print_str(g_osd_gray,g_osd_width*8,font_height);
#endif
    return g_osd_width;
}

/*
 * construct rgb osd
 * param : str want to transfer
 * param : color of osd
 */
static int32_t osd_make_rgb_str(uint8_t *str,uint32_t color){
    uint8_t font_height,r,g,b;
    int i;

    font_height = g_font_ctx->YSize;

    r = (color>>16) & 0xff;
    g = (color>>8)  & 0xff;
    b = (color>>0)  & 0xff;

    memset(g_osd_rgb,0x00,sizeof(g_osd_rgb));

    /* make gray text */
    osd_make_gray_str(str);

    for(i = 0 ; i < (g_osd_width * font_height * 8) ; i++){
        if(g_osd_gray[i] == 0xff){
            g_osd_rgb[i*3+0] = r;
            g_osd_rgb[i*3+1] = g;
            g_osd_rgb[i*3+2] = b;
        }
    }
#if OSD_FILE_DEBUG	
    osd_save_file((uint8_t *)"osd_test.rgb",g_osd_rgb,g_osd_width * font_height * 8 * 3);
	printf("osd debug : rgb osd (w,h) = (%d,%d)\n",g_osd_width*8,font_height);
#endif
    return 0;
}

/*
 * construct yuyv osd
 * param : str want to transfer
 * param : color of osd
 */
static int32_t osd_make_yuv_str(uint8_t *str,uint32_t color){;
    uint16_t osd_height = g_font_ctx->YSize;
    osd_make_rgb_str(str,color);
    osd_rgb2yuv(g_osd_rgb,g_osd_yuyv,g_osd_width * 8,osd_height);
#if OSD_FILE_DEBUG
    osd_save_file((uint8_t *)"osd_test.yuyv",g_osd_yuyv,g_osd_width * 8 * osd_height * 2);
	printf("osd debug : yuyv osd (w,h) = (%d,%d)\n",g_osd_width*8,osd_height);
#endif
    return 0;
}

/*
 * user api
 * overlay the text and source picture
 * pic : point the source picture
 * text : point the text want to transfer
 */
int osd_text_overlay(pic_src_t *pic,text_src_t *text){
    int32_t pic_w,i;
    int32_t bytesline_pic = 0,bytesline_osd = 0;
    int32_t osd_h,x,y;
    uint8_t *pic_start,*osd_start,*osd_base,*pic_base;
    uint8_t color_type,overlay_type;

    assert(pic != NULL && text != NULL);
    color_type = text->color_type;
    pic_w = pic->width;

    g_osd_width = 0;
    g_osd_char_cnt = 0;

    switch(color_type){
        case OSD_C_GRAY :
            osd_make_gray_str(text->text);
            bytesline_osd = g_osd_width * 8;
            bytesline_pic = pic_w;
            osd_base = g_osd_gray;
            break;

        case OSD_C_RGB :
            osd_make_rgb_str(text->text,text->color);
            bytesline_osd = g_osd_width * 8 * 3;
            bytesline_pic = pic_w * 3;
            osd_base = g_osd_rgb;
            break;

        case OSD_C_YUYV :
            osd_make_yuv_str(text->text,text->color);
            bytesline_osd = g_osd_width * 8 * 2;
            bytesline_pic = pic_w * 2;
            osd_base = g_osd_yuyv;
            break;
        default:
            printf("osd : invalid color type\n");
            return -1;
    }

    osd_h = g_font_ctx->YSize;
    pic_base = pic->data;
    overlay_type = text->overlay_type;
    x = text->x;
    y = text->y;

    /* overlay line by line */
    for(i = 0 ; i < osd_h; i++){
        pic_start = pic_base + bytesline_pic*(y+i) + x * pic->bpp;
        osd_start = osd_base + bytesline_osd*i;

        if(overlay_type == OSD_M_COVER){
            memcpy(pic_start,osd_start,bytesline_osd);

        }else{
            printf("osd : just support cover mode\n");
        }
    }
    return 0;
}


/*
 * user api
 * transfer text to osd picture
 * pic : point the picture represent the text
 * text : point the text want to transfer
 */
int osd_mk_osd(pic_osd_t *pic,text_src_t *text){
    uint8_t *osd_start,*pic_start;
    uint8_t color_type;

	g_osd_width = 0;
    g_osd_char_cnt = 0;

    assert(pic != NULL && text != NULL);
    color_type = text->color_type;
    switch(color_type){
        case OSD_C_GRAY :
            osd_make_gray_str(text->text);
            pic->width = g_osd_width * 8;
            osd_start = g_osd_gray;
            break;

        case OSD_C_RGB :
            osd_make_rgb_str(text->text,text->color);
            pic->width = g_osd_width * 8 * 3;
            osd_start = g_osd_rgb;
            break;

        case OSD_C_YUYV :
            osd_make_yuv_str(text->text,text->color);
            pic->width = g_osd_width * 8 * 2;
            osd_start = g_osd_yuyv;
            break;
        default:
            printf("osd : invalid color type\n");
            return -1;
    }

    pic_start = pic->data;
    pic->height = g_font_ctx->YSize;
    pic->x = text->x;
    pic->y = text->y;
    memcpy(pic_start,osd_start,pic->height*pic->width);
#if OSD_FILE_DEBUG
    osd_save_file((uint8_t *)"text_osd.yuyv",pic_start,pic->height*pic->width);
	printf("osd debug : yuyv text osd (w,h) = (%d,%d)\n",pic->width,pic->height);
#endif
    return 0;
}


/*
 * user api
 * overlay the osd picture and source picture
 * pic : point the source picture
 * osd : point the osd picture
 */
void osd_pic_overlay(pic_src_t *pic,pic_osd_t *osd){
    int i = 0,x,y;
    char *pic_line,*osd_line;
    x = osd->x;
    y = osd->y;
    pic_line = (char *)pic->data + pic->width * pic->bpp * y + pic->bpp * x;
    osd_line = (char *)osd->data;
    /* copy line by line */
    for(i = 0; i < osd->height;i++){
        memcpy(pic_line,osd_line,osd->width);
        osd_line += osd->width;
        pic_line += pic->width * pic->bpp;
    }
}
