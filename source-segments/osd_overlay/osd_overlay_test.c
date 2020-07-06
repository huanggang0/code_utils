#include <stdio.h>
#include <string.h>
#include <osd_overlay.h>

#define WIDTH	   720
#define HEIGHT	   576
#define YUYV_SIZE  (WIDTH * HEIGHT * 2)

#define OSD_WIDTH	   200
#define OSD_HEIGHT	   30
#define OSD_SIZE      (OSD_WIDTH * OSD_HEIGHT)

void osd_save_file(uint8_t*filename,uint8_t *buffer,uint32_t size);

static uint8_t g_yuyv[WIDTH*HEIGHT*2];
static uint8_t g_osd[OSD_SIZE];
pic_src_t pic = {
	.data = g_yuyv,
	.width = WIDTH,
	.height = HEIGHT,
	.bpp = 2,
};

text_src_t text = {
	.text = (uint8_t *)"SDI",
	.overlay_type = OSD_M_COVER,
	.color_type = OSD_C_YUYV,
	.color = 0x00aa11cc,
	.x = 10,
	.y = 10,
};

pic_osd_t osd = {
	.data = g_osd,
};

int main(void){
#if 0
	while(1){
		memset(g_yuyv,0x00,YUYV_SIZE);
		osd_text_overlay(&pic,&text);
		sleep(2);
		//osd_save_file((uint8_t *)"osd_overlay_test.yuyv",g_yuyv,YUYV_SIZE);
	}
#endif
	osd_mk_osd(&osd,&text);
	osd_pic_overlay(&pic,&osd);
	osd_save_file((uint8_t *)"osd_overlay_test.yuyv",g_yuyv,YUYV_SIZE);
	return 0;
}
