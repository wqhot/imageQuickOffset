#pragma once

void get_image_data(unsigned char *img_data);

void show_image_data(unsigned char *img_data, int rows, int cols);

void save_image_data(unsigned char *img_ptr, int rows, int cols);

int load_remap_mat(int rows, int cols);

void trans_remap(unsigned char *src_ptr, unsigned char *dst_ptr, int rows, int cols, int final_rows, int final_cols);

int gen_copy_img(unsigned char *src_ptr, unsigned char *dst_ptr, int rows, int cols, int final_rows, int final_cols);

int add_alpha(unsigned char *src_ptr, unsigned char *dst_ptr, int rows, int cols);

int rgb888_to_rgb565(unsigned char *src_ptr, unsigned char *dst_ptr, int rows, int cols);