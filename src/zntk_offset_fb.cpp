
#include <math.h>

#include <iostream>

#include <gen_c.h>
#include <tic_toc.h>
#include <image_process.h>

int main()
{
    unsigned char *ori_image = new unsigned char[720 * 1920 * 3];
    unsigned char *new_image = new unsigned char[1080 * 1920 * 3];
    get_image_data(ori_image);
    TicToc tic;
    tran_img(ori_image, new_image);
    tic.toc_print("trans");
    show_image_data(new_image, 1080, 1920);
    delete[] ori_image;
    delete[] new_image;
    return 0;
}