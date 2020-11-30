
#include <math.h>
#include <unistd.h>
#include <iostream>

#include <gen_c.h>
#include <tic_toc.h>
#include <image_process.h>
#include <cp_framebuffer.h>
#include <defines.h>
#include <thread_manage.h>

threadManage g_thread_manager;

int main()
{
    // 测试区域
    videoObtain video_handle("/mnt/g/project/imageQuickOffset_zntk/build/test.mp4");
    
    frameHandle frame_handle(video_handle.queue_id);
    int screen_rows;
    int screen_cols;
    int screen_channels;
    frameBufferHandle buffer_handle(frame_handle.queue_id);
    buffer_handle.get_info(screen_rows, screen_cols, screen_channels);
    frame_handle.screen_setting(screen_rows, screen_cols, screen_channels);
    

    video_handle.start();
    // get_image_data(ori_image);
    // // 加载畸变矩阵
    // load_remap_mat(AFTER_TRANS_ROWS, AFTER_TRANS_COLS);
    // TicToc tic;
    // trans_remap(ori_image, new_image, BEFORE_TRANS_ROWS, BEFORE_TRANS_COLS, AFTER_TRANS_ROWS, AFTER_TRANS_COLS);
    // tic.toc_print("trans");
    
    // // 生成畸变
    // // tran_img(ori_image, new_image);
    // // 复制图像
    // gen_copy_img(new_image, show_image, AFTER_TRANS_ROWS, AFTER_TRANS_COLS, FINAL_ROWS, FINAL_COLS);
    // save_image_data(show_image, FINAL_ROWS, FINAL_COLS);
    
    // // show_image_data(show_image, FINAL_ROWS, FINAL_COLS);
    // init_framebuffer();
    // tic.restart_toc();
    // copy_image_to_framebuffer(show_image, FINAL_ROWS, FINAL_COLS);
    
    // // while (true)
    // // {
    // //     copy_image_to_framebuffer(show_image, FINAL_ROWS, FINAL_COLS);
    // //     std::cout << "reflash" << std::endl;
    // //     usleep(10000);
    // // }
    
    // tic.toc_print("show");
    // close_framebuffer();

    while (true)
    {
        ;
    }
    
    return 0;
}