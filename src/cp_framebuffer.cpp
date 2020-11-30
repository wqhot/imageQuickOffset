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
#include <tic_toc.h>
#include <cp_framebuffer.h>
#include <thread>

static char *fbp = 0;
static int xres = 0;
static int yres = 0;
static int bits_per_pixel = 0;
static int line_length = 0;
static long int screensize = 0;
static int fbfd = 0;

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

    printf("fb_info_t: red(%d %d) green(%d %d) blue(%d %d)\n", vinfo.red.offset, vinfo.red.length,
           vinfo.green.offset, vinfo.green.length, vinfo.blue.offset, vinfo.blue.length);
    printf("xoffset:%d, yoffset:%d \n", vinfo.xoffset, vinfo.yoffset);
    printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);
    printf("line_length=%d\n", finfo.line_length);

    xres = vinfo.xres;
    yres = vinfo.yres;
    bits_per_pixel = vinfo.bits_per_pixel;
    line_length = finfo.line_length * 8 / bits_per_pixel;

    //计算屏幕的总大小（字节）
    screensize = line_length * vinfo.yres * vinfo.bits_per_pixel / 8;
    printf("screensize=%d byte\n", screensize);

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
    int max_cols = cols < xres ? cols : xres;
    int max_rows = rows < yres ? rows : yres;
    TicToc tic;
    for (int i = 0; i < max_rows; ++i)
    {
        size_t start_pos_screen = (line_length * i) * bits_per_pixel / 8;
        size_t start_pos_image = (cols * i) * bits_per_pixel / 8;
        memcpy(fbp + start_pos_screen, image + start_pos_image, max_cols * bits_per_pixel / 8);
    }
    // memcpy(fbp, dst_ptr, rows * cols * bits_per_pixel / 8);
    tic.toc_print("copy to screen");

    return 0;
}

frameBufferHandle::frameBufferHandle(int get_queue_id) : get_queue_id(get_queue_id)
{
    init_framebuffer();
    _try_to_expire = false;
    std::thread framebuffer_thread(cp_to_screen, this);
    framebuffer_thread.detach();
}

frameBufferHandle::~frameBufferHandle()
{
    _try_to_expire = true;
    close_framebuffer();
}

int frameBufferHandle::cp_to_screen(frameBufferHandle *handle)
{
    while (!handle->_try_to_expire)
    {
        basic_item item;
        if (!g_thread_manager.get_queue(handle->get_queue_id, item))
        {
            continue;
        }
        std::cout << item.first.size() << std::endl;
        std::cout << item.second << std::endl;
        copy_image_to_framebuffer(item.first.data, item.first.rows, item.first.cols);
    }
    return 0;
}
void frameBufferHandle::get_info(int &screen_rows, int &screen_cols, int &screen_channels)
{
    screen_rows = yres;
    screen_cols = xres;
    screen_channels = bits_per_pixel / 8;
}
