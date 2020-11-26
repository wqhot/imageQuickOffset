
#include <math.h>
#include <unistd.h>
#include <iostream>

#include <gen_c.h>
#include <tic_toc.h>
#include <image_process.h>
#include <cp_framebuffer.h>
#include <defines.h>

int main()
{
    unsigned char *ori_image = new unsigned char[BEFORE_TRANS_ROWS * BEFORE_TRANS_COLS * 3];
    unsigned char *new_image = new unsigned char[AFTER_TRANS_ROWS * AFTER_TRANS_COLS * 3];
    unsigned char *show_image = new unsigned char[FINAL_ROWS * FINAL_COLS * 3];
    get_image_data(ori_image);
    // 加载畸变矩阵
    load_remap_mat(AFTER_TRANS_ROWS, AFTER_TRANS_COLS);
    TicToc tic;
    trans_remap(ori_image, new_image, BEFORE_TRANS_ROWS, BEFORE_TRANS_COLS, AFTER_TRANS_ROWS, AFTER_TRANS_COLS);
    tic.toc_print("trans");
    save_image_data(new_image, AFTER_TRANS_ROWS, AFTER_TRANS_COLS);
    // 生成畸变
    // tran_img(ori_image, new_image);
    // 复制图像
    gen_copy_img(new_image, show_image, AFTER_TRANS_ROWS, AFTER_TRANS_COLS, FINAL_ROWS, FINAL_COLS);

    
    // show_image_data(show_image, FINAL_ROWS, FINAL_COLS);
    init_framebuffer();
    tic.restart_toc();
    // while (true)
    // {
    //     copy_image_to_framebuffer(show_image, FINAL_ROWS, FINAL_COLS);
    //     std::cout << "reflash" << std::endl;
    //     usleep(10000);
    // }
    
    tic.toc_print("show");
    close_framebuffer();

    delete[] ori_image;
    delete[] new_image;
    delete[] show_image;
    return 0;
}