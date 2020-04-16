#ifndef __OSD_OVERLAY_H__
#define __OSD_OVERLAY_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>

#include "./fonts/GUI.h"

#define OSD_STR_SIZE    64
#define OSD_CHAR_SIZE3   (100*100*2) /* 100*100 YUYV */
#define OSD_CHAR_SIZE2   (100*100*3) /* 100*100 RGB */
#define OSD_CHAR_SIZE1   (100*100*1) /* 100*100 gray */

#define OSD_M_TRANS	  0x00           /* transparent */
#define OSD_M_COVER   0x01           /* cover */
#define OSD_M_INVALID 0x02

#define OSD_C_GRAY	  0x00
#define OSD_C_RGB     0x01
#define OSD_C_YUYV    0x02
#define OSD_C_INVALID 0x03

#define OSD_FILE_DEBUG	0
#define OSD_PRINT_DEBUG 0

typedef struct charinfo{
	uint8_t char_width;
	uint8_t char_height;
	const uint8_t *line_data_src;
	uint8_t *line_data_dst;
}charinfo_t;


typedef struct pic_src{
	uint8_t *data;
	uint16_t width;
	uint16_t height;
	uint8_t bpp;
}pic_src_t;

typedef struct pic_osd {
	uint8_t *data;
	uint16_t width;
	uint16_t height;
	uint16_t x;
	uint16_t y;
	
}pic_osd_t;

typedef struct src_text{
	uint8_t  *text;
	uint8_t  overlay_type;
	uint8_t  color_type;
	uint32_t color;
	uint16_t x;
	uint16_t y;
}text_src_t;


int osd_text_overlay(pic_src_t *pic,text_src_t *osd);
int osd_mk_osd(pic_osd_t *pic,text_src_t *text);
void osd_pic_overlay(pic_src_t *pic,pic_osd_t *osd);


#endif /* __OSD_OVERLAY_H__ */
