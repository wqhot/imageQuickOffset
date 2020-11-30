
#include <defines.h>
#include <gen_c_v2.h>
#include <image_process.h>


static std::vector<cv::Mat> remap_matrix;

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

int load_remap_mat(int rows, int cols)
{
    cv::Mat offset_matrix(cv::Size(cols, rows), CV_32FC2, offset_mat);
    remap_matrix.clear();
	//分离通道
	cv::split(offset_matrix, remap_matrix);
    return 0;
}

void trans_remap(unsigned char *src_ptr, unsigned char *dst_ptr, int rows, int cols, int final_rows, int final_cols)
{
    cv::Mat src(cv::Size(cols, rows), CV_8UC3, src_ptr);
    cv::Mat src_extend(cv::Size(cols, rows + 1), CV_8UC3, cv::Scalar(0, 0, 0));
    src.copyTo(src_extend.rowRange(0, rows));
    cv::Mat dst(cv::Size(final_cols, final_rows), CV_8UC3);
    cv::remap(src_extend, dst, remap_matrix[1], remap_matrix[0], cv::INTER_NEAREST);
    memcpy(dst_ptr, dst.data, dst.cols * dst.rows * dst.channels());
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
    cv::imwrite("output.png", image);
    cv::waitKey(0);
}

void save_image_data(unsigned char *img_ptr, int rows, int cols)
{
    cv::Mat image(cv::Size(cols, rows), CV_8UC3, img_ptr);
    cv::imwrite("output.png", image);
}

int add_alpha(unsigned char *src_ptr, unsigned char *dst_ptr, int rows, int cols)
{
	cv::Mat src(cv::Size(cols, rows), CV_8UC3, src_ptr);
    cv::Mat alpha(cv::Size(cols, rows), CV_8UC1, cv::Scalar(255));
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
    memcpy(dst_ptr, dst.data, dst.elemSize1() * dst.cols * dst.rows * dst.channels());
    cv::imwrite("alpha.png", dst);
	return 0;
}

int rgb888_to_rgb565(unsigned char *src_ptr, unsigned char *dst_ptr, int rows, int cols)
{
    cv::Mat src(cv::Size(cols, rows), CV_8UC3, src_ptr);
    cv::Mat dst = cv::Mat(src.rows, src.cols, CV_16UC1);
    cv::cvtColor(src, dst, cv::COLOR_BGRA2BGR565);
    
    memcpy(dst_ptr, dst.data, dst.elemSize1() * dst.cols * dst.rows * dst.channels());
	return 0;
}

int gen_test_image(unsigned char *img_ptr, int rows, int cols)
{
    cv::Mat src(cv::Size(cols, rows), CV_8UC4);
    cv::Mat green(cv::Size(cols, rows / 2), CV_8UC4, cv::Scalar(0, 255, 0, 0));
    cv::Mat red(cv::Size(cols, rows / 2), CV_8UC4, cv::Scalar(255, 0, 0, 0));
    green.copyTo(src.rowRange(0, rows / 2));
    red.copyTo(src.rowRange(rows / 2, rows));
    memcpy(img_ptr, src.data, src.cols * src.rows * src.channels());
    return 0;
}

int gen_copy_img(unsigned char *src_ptr, unsigned char *dst_ptr, int rows, int cols, int final_rows, int final_cols)
{
    cv::Mat src(cv::Size(cols, rows), CV_8UC3, src_ptr);
    cv::Mat src_90, src_270;   
    cv::transpose(src, src_90); // 90度
    cv::flip(src_90, src_270, cv::ROTATE_180); // 270度
    cv::Mat dst(cv::Size(final_cols, final_rows), CV_8UC3);
    cv::Mat test = cv::imread("3.png");
    // 左右模式拼接
    cv::resize(src_90, src_90, cv::Size(int(final_cols / 2), int(final_rows)));
    cv::resize(src_270, src_270, cv::Size(int(final_cols / 2), int(final_rows)));
    src_90.copyTo(dst.colRange(0, int(final_cols / 2)));
    src_270.copyTo(dst.colRange(int(final_cols / 2), final_cols));
    // cv::line(dst, cv::Point2i(0, int(final_rows / 2)), cv::Point2i(0, int(final_rows / 2)), cv::Scalar(128, 128, 128));
    memcpy(dst_ptr, dst.data, dst.cols * dst.rows * dst.channels());
    return 0;
}


int videoObtain::load_video_from_file(std::string file_name)
{
    m_capture.open(file_name);
    if (!m_capture.isOpened())
    {
        return -1;
    }

    double rate = m_capture.get(CV_CAP_PROP_FPS);
    m_frame_delay = 1000 / rate;
    return 0;
}


videoObtain::videoObtain(std::string file_name, callback_f cb_func)
{
    if (!load_video_from_file(file_name))
    {
        return;
    }
    m_cb_func = cb_func;

}
