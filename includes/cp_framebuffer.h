#pragma once

#include <atomic>

int init_framebuffer();
int close_framebuffer();
int copy_image_to_framebuffer(unsigned char *image, int rows, int cols);

class frameBufferHandle 
{ 
public:
    int get_queue_id;
    std::atomic<bool> _try_to_expire;
    frameBufferHandle(int get_queue_id);
    ~frameBufferHandle();
    static int cp_to_screen(frameBufferHandle *handle);
    void get_info(int &screen_rows, int &screen_cols, int &screen_channels);
};