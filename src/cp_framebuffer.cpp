#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <image_process.h>
 
 
//14byte文件头
typedef struct
{
	char cfType[2];//文件类型，"BM"(0x4D42)
	long cfSize;//文件大小（字节）
	long cfReserved;//保留，值为0
	long cfoffBits;//数据区相对于文件头的偏移量（字节）
}__attribute__((packed)) BITMAPFILEHEADER;
//__attribute__((packed))的作用是告诉编译器取消结构在编译过程中的优化对齐
 
//40byte信息头
typedef struct
{
	char ciSize[4];//BITMAPFILEHEADER所占的字节数
	long ciWidth;//宽度
	long ciHeight;//高度
	char ciPlanes[2];//目标设备的位平面数，值为1
	int ciBitCount;//每个像素的位数
	char ciCompress[4];//压缩说明
	char ciSizeImage[4];//用字节表示的图像大小，该数据必须是4的倍数
	char ciXPelsPerMeter[4];//目标设备的水平像素数/米
	char ciYPelsPerMeter[4];//目标设备的垂直像素数/米
	char ciClrUsed[4]; //位图使用调色板的颜色数
	char ciClrImportant[4]; //指定重要的颜色数，当该域的值等于颜色数时（或者等于0时），表示所有颜色都一样重要
}__attribute__((packed)) BITMAPINFOHEADER;
 
typedef struct
{
	unsigned short blue;
	unsigned short green;
	unsigned short red;
	unsigned short reserved;
}__attribute__((packed)) PIXEL;//颜色模式RGB
 
BITMAPFILEHEADER FileHead;
BITMAPINFOHEADER InfoHead;
 
static char *fbp = 0;
static int xres = 0;
static int yres = 0;
static int bits_per_pixel = 0;
static long int screensize = 0;
static int fbfd = 0;

int show_bmp();

int init_framebuffer()
{
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	
	struct fb_bitfield red;
	struct fb_bitfield green;
	struct fb_bitfield blue;

    //打开显示设备
	fbfd = open("/dev/fb0", O_RDWR);
	if (!fbfd)
	{
		printf("Error: cannot open framebuffer device.\n");
		exit(1);
	}
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo))
	{
		printf("Error：reading fixed information.\n");
		exit(2);
	}
 
	if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo))
	{
		printf("Error: reading variable information.\n");
		exit(3);
	}
 
	printf("R:%d,G:%d,B:%d \n", vinfo.red, vinfo.green, vinfo.blue );
    printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel );

    xres = vinfo.xres;
	yres = vinfo.yres;
	bits_per_pixel = vinfo.bits_per_pixel;

    //计算屏幕的总大小（字节）
	screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
	printf("screensize=%d byte\n",screensize);

    //对象映射
	fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	if ((int)*fbp == -1)
	{
		printf("Error: failed to map framebuffer device to memory.\n");
		exit(4);
	}

    return 0;
}

int close_framebuffer()
{
    //删除对象映射
	munmap(fbp, screensize);
	close(fbfd);
	return 0;
}

int copy_image_to_framebuffer(unsigned char *image, int rows, int cols)
{
	// 增加透明度
	unsigned char *dst_ptr = new unsigned char[rows * cols * 4];
	add_alpha(image, dst_ptr, rows, cols);
	memcpy(fbp, dst_ptr, rows * cols * 4);
	delete[] dst_ptr;
	return 0;
}

