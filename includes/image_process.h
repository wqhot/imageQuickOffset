#pragma once

#include <functional>
#include <iostream>
#include <vector>
#include <timer.h>
#include <thread_manage.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

void get_image_data(unsigned char *img_data);

void show_image_data(unsigned char *img_data, int rows, int cols);

void save_image_data(unsigned char *img_ptr, int rows, int cols);

int gen_test_image(unsigned char *img_ptr, int rows, int cols);

int load_remap_mat(int rows, int cols);

void trans_remap(unsigned char *src_ptr, unsigned char *dst_ptr, int rows, int cols, int final_rows, int final_cols);

int gen_copy_img(unsigned char *src_ptr, unsigned char *dst_ptr, int rows, int cols, int final_rows, int final_cols);

int add_alpha(unsigned char *src_ptr, unsigned char *dst_ptr, int rows, int cols);

int rgb888_to_rgb565(unsigned char *src_ptr, unsigned char *dst_ptr, int rows, int cols);

class timerVideo : public Timer
{
public:
    void start(int interval, std::function<void()> task)
    {
        // is started, do not start again
        if (_expired == false)
            return;

        // start async timer, launch thread and wait in that thread
        _expired = false;
        std::thread([this, interval, task]() {
            while (!_try_to_expire)
            {
                // sleep every interval and do the task again and again until times up
                std::this_thread::sleep_for(std::chrono::milliseconds(interval));
                task();
            }

            {
                // timer be stopped, update the condition variable expired and wake main thread
                std::lock_guard<std::mutex> locker(_mutex);
                _expired = true;
                _expired_cond.notify_one();
            }
        }).detach();
    }
};

class videoObtain
{
private:
    cv::VideoCapture m_capture;
    typedef std::function<void(unsigned char *)> callback_f;
    int m_frame_delay;
    callback_f m_cb_func;
    // 线程
    static int get_frame_from_capture(videoObtain *video_obtain);

public:
    videoObtain(std::string file_name, callback_f cb_func);
    int load_video_from_file(std::string file_name);
};