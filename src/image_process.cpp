#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <defines.h>

struct object_rect {
    int x;
    int y;
    int width;
    int height;
};

int resize_uniform(cv::Mat &src, cv::Mat &dst, cv::Size dst_size, object_rect &effect_area)
{
    int w = src.cols;
    int h = src.rows;
    int dst_w = dst_size.width;
    int dst_h = dst_size.height;
    std::cout << "src: (" << h << ", " << w << ")" << std::endl;
    dst = cv::Mat(cv::Size(dst_w, dst_h), CV_8UC3, cv::Scalar(0));

    float ratio_src = w*1.0 / h;
    float ratio_dst = dst_w*1.0 / dst_h;

    int tmp_w=0;
    int tmp_h=0;
    if (ratio_src > ratio_dst) {
        tmp_w = dst_w;
        tmp_h = floor((dst_w*1.0 / w) * h);
    } else if (ratio_src < ratio_dst){
        tmp_h = dst_h;
        tmp_w = floor((dst_h*1.0 / h) * w);
    } else {
        resize(src, dst, dst_size);
        effect_area.x = 0;
        effect_area.y = 0;
        effect_area.width = dst_w;
        effect_area.height = dst_h;
        return 0;
    }

    std::cout << "tmp: (" << tmp_h << ", " << tmp_w << ")" << std::endl;
    cv::Mat tmp;
    resize(src, tmp, cv::Size(tmp_w, tmp_h));

    if (tmp_w != dst_w) { //高对齐，宽没对齐
        int index_w = floor((dst_w - tmp_w) / 2.0);
        std::cout << "index_w: " << index_w << std::endl;
        for (int i=0; i<dst_h; i++) {
            memcpy(dst.data+i*dst_w*3 + index_w*3, tmp.data+i*tmp_w*3, tmp_w*3);
        }
        effect_area.x = index_w;
        effect_area.y = 0;
        effect_area.width = tmp_w;
        effect_area.height = tmp_h;
    } else if (tmp_h != dst_h) { //宽对齐， 高没有对齐
        int index_h = floor((dst_h - tmp_h) / 2.0);
        std::cout << "index_h: " << index_h << std::endl;
        memcpy(dst.data+index_h*dst_w*3, tmp.data, tmp_w*tmp_h*3);
        effect_area.x = 0;
        effect_area.y = index_h;
        effect_area.width = tmp_w;
        effect_area.height = tmp_h;
    } else {
        printf("error\n");
    }
    return 0;
}

void get_image_data(unsigned char *img_ptr)
{
    cv::Mat image = cv::imread("2.png");
    cv::Mat dst;

    object_rect res_area;
    (void)resize_uniform(image, dst, cv::Size(BEFORE_TRANS_COLS, BEFORE_TRANS_ROWS), res_area);
    memcpy(img_ptr, dst.data, dst.cols * dst.rows * dst.channels());
}

void show_image_data(unsigned char *img_ptr, int rows, int cols)
{
    cv::Mat image(cv::Size(cols, rows), CV_8UC3, img_ptr);
    cv::imshow("bbb", image);
    cv::waitKey(0);
}

int add_alpha(unsigned char *src_ptr, unsigned char *dst_ptr, int rows, int cols)
{
	cv::Mat src(cv::Size(cols, rows), CV_8UC3, src_ptr);
    cv::Mat alpha(cv::Size(cols, rows), CV_8UC1, cv::Scalar(0));
	// cv::Mat alpha = cv::Mat::ones(src.rows, src.cols, CV_8UC1);
	cv::Mat dst = cv::Mat(src.rows, src.cols, CV_8UC4);
 
	std::vector<cv::Mat> src_channels;
	std::vector<cv::Mat> dst_channels;
	//分离通道
	cv::split(src, src_channels);
 
	dst_channels.push_back(src_channels[0]);
	dst_channels.push_back(src_channels[1]);
	dst_channels.push_back(src_channels[2]);
	//添加透明度通道
	dst_channels.push_back(alpha);
	//合并通道
	cv::merge(dst_channels, dst);
    memcpy(dst_ptr, dst.data, dst.cols * dst.rows * dst.channels());
	return 0;
}

int gen_copy_img(unsigned char *src_ptr, unsigned char *dst_ptr, int rows, int cols, int final_rows, int final_cols)
{
    cv::Mat src(cv::Size(cols, rows), CV_8UC3, src_ptr);
    cv::Mat src_90, src_270;   
    cv::transpose(src, src_90); // 90度
    cv::flip(src_90, src_270, cv::ROTATE_180); // 270度
    cv::Mat dst(cv::Size(final_cols, final_rows), CV_8UC3);
    // 左右模式拼接
    cv::resize(src_90, src_90, cv::Size(int(final_cols / 2), final_rows));
    cv::resize(src_270, src_270, cv::Size(int(final_cols / 2), final_rows));
    src_270.copyTo(dst.colRange(0, int(final_cols / 2)));
    src_90.copyTo(dst.colRange(int(final_cols / 2), final_cols));
    memcpy(dst_ptr, dst.data, dst.cols * dst.rows * dst.channels());
    return 0;
}