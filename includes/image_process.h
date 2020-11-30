#pragma once

#include <functional>
#include <iostream>
#include <atomic>
#include <vector>
#include <timer.h>
#include <thread_manage.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

struct object_rect
{
    int x;
    int y;
    int width;
    int height;
};

void get_image_data(unsigned char *img_data);

void show_image_data(unsigned char *img_data, int rows, int cols);

void save_image_data(unsigned char *img_ptr, int rows, int cols);

int gen_test_image(unsigned char *img_ptr, int rows, int cols);

int load_remap_mat(int rows, int cols);

void trans_remap(unsigned char *src_ptr, unsigned char *dst_ptr, int rows, int cols, int final_rows, int final_cols);

int gen_copy_img(unsigned char *src_ptr, unsigned char *dst_ptr, int rows, int cols, int final_rows, int final_cols);

int add_alpha(unsigned char *src_ptr, unsigned char *dst_ptr, int rows, int cols);

int rgb888_to_rgb565(unsigned char *src_ptr, unsigned char *dst_ptr, int rows, int cols);

class frameHandle
{
private:
    int m_frame_delay;
    // 线程
    static int process_image(frameHandle *handle);

public:
    int queue_id;
    int screen_rows;
    int screen_cols;
    int screen_channels;
    int get_queue_id;
    std::vector<cv::Mat> remap_matrix;
    std::atomic<bool> _expired;       // 关闭线程
    std::atomic<bool> _try_to_expire; // 下次循环关闭线程
    frameHandle(int get_queue);
    void screen_setting(int screen_rows, int screen_cols, int screen_channels);
    int resize_uniform(cv::Mat &src, cv::Mat &dst, cv::Size dst_size, object_rect &effect_area);
    int load_remap_mat(int rows, int cols);
    void trans_remap(cv::Mat src, cv::Mat &dst);
    int add_alpha(cv::Mat src, cv::Mat &dst);
    int rgb888_to_rgb565(cv::Mat src, cv::Mat &dst);
    int gen_copy_img(cv::Mat src, cv::Mat &dst, int final_rows, int final_cols);
};

class videoObtain
{
private:
    cv::VideoCapture m_capture;
    int m_frame_delay;
    // 线程
    static int get_frame_from_capture(videoObtain *video_obtain);

public:
    Timer timer;
    int queue_id;
    videoObtain(std::string file_name);
    void start();
    bool load_video_from_file(std::string file_name);
};