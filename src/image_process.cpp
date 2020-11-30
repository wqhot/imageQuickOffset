
#include <defines.h>
#include <gen_c_v2.h>
#include <image_process.h>
#include <clock.h>

int frameHandle::resize_uniform(cv::Mat &src, cv::Mat &dst, cv::Size dst_size, object_rect &effect_area)
{
    int w = src.cols;
    int h = src.rows;
    int dst_w = dst_size.width;
    int dst_h = dst_size.height;
    std::cout << "src: (" << h << ", " << w << ")" << std::endl;
    dst = cv::Mat(cv::Size(dst_w, dst_h), CV_8UC3, cv::Scalar(0));

    float ratio_src = w * 1.0 / h;
    float ratio_dst = dst_w * 1.0 / dst_h;

    int tmp_w = 0;
    int tmp_h = 0;
    if (ratio_src > ratio_dst)
    {
        tmp_w = dst_w;
        tmp_h = floor((dst_w * 1.0 / w) * h);
    }
    else if (ratio_src < ratio_dst)
    {
        tmp_h = dst_h;
        tmp_w = floor((dst_h * 1.0 / h) * w);
    }
    else
    {
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

    if (tmp_w != dst_w)
    { //高对齐，宽没对齐
        int index_w = floor((dst_w - tmp_w) / 2.0);
        std::cout << "index_w: " << index_w << std::endl;
        for (int i = 0; i < dst_h; i++)
        {
            memcpy(dst.data + i * dst_w * 3 + index_w * 3, tmp.data + i * tmp_w * 3, tmp_w * 3);
        }
        effect_area.x = index_w;
        effect_area.y = 0;
        effect_area.width = tmp_w;
        effect_area.height = tmp_h;
    }
    else if (tmp_h != dst_h)
    { //宽对齐， 高没有对齐
        int index_h = floor((dst_h - tmp_h) / 2.0);
        std::cout << "index_h: " << index_h << std::endl;
        memcpy(dst.data + index_h * dst_w * 3, tmp.data, tmp_w * tmp_h * 3);
        effect_area.x = 0;
        effect_area.y = index_h;
        effect_area.width = tmp_w;
        effect_area.height = tmp_h;
    }
    else
    {
        printf("error\n");
    }
    return 0;
}

int frameHandle::load_remap_mat(int rows, int cols)
{
    cv::Mat offset_matrix(cv::Size(cols, rows), CV_32FC2, offset_mat);
    remap_matrix.clear();
    //分离通道
    cv::split(offset_matrix, remap_matrix);
    return 0;
}

void frameHandle::trans_remap(cv::Mat src, cv::Mat &dst)
{
    int rows = src.rows;
    int cols = src.cols;
    cv::Mat src_extend(cv::Size(cols, rows + 1), CV_8UC3, cv::Scalar(0, 0, 0));
    src.copyTo(src_extend.rowRange(0, rows));
    cv::remap(src_extend, dst, remap_matrix[1], remap_matrix[0], cv::INTER_NEAREST);
}

int frameHandle::add_alpha(cv::Mat src, cv::Mat &dst)
{
    int rows = src.rows;
    int cols = src.cols;
    cv::Mat alpha(cv::Size(cols, rows), CV_8UC1, cv::Scalar(255));
    dst = cv::Mat(src.rows, src.cols, CV_8UC4);

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
    return 0;
}

int frameHandle::rgb888_to_rgb565(cv::Mat src, cv::Mat &dst)
{
    int rows = src.rows;
    int cols = src.cols;
    cv::cvtColor(src, dst, cv::COLOR_BGRA2BGR565);
    return 0;
}

int frameHandle::gen_copy_img(cv::Mat src, cv::Mat &dst, int final_rows, int final_cols)
{
    int rows = src.rows;
    int cols = src.cols;
    cv::Mat src_90, src_270;
    cv::transpose(src, src_90);                // 90度
    cv::flip(src_90, src_270, cv::ROTATE_180); // 270度
    dst = cv::Mat(cv::Size(final_cols, final_rows), CV_8UC3);
    // 左右模式拼接
    cv::resize(src_90, src_90, cv::Size(int(final_cols / 2), int(final_rows)));
    cv::resize(src_270, src_270, cv::Size(int(final_cols / 2), int(final_rows)));
    src_90.copyTo(dst.colRange(0, int(final_cols / 2)));
    src_270.copyTo(dst.colRange(int(final_cols / 2), final_cols));
    // cv::line(dst, cv::Point2i(0, int(final_rows / 2)), cv::Point2i(0, int(final_rows / 2)), cv::Scalar(128, 128, 128));
    return 0;
}

bool videoObtain::load_video_from_file(std::string file_name)
{
    m_capture.open(file_name);
    if (!m_capture.isOpened())
    {
        return false;
    }

    double rate = m_capture.get(CV_CAP_PROP_FPS);
    m_frame_delay = 1000 / rate;
    return true;
}

videoObtain::videoObtain(std::string file_name)
{
    if (!load_video_from_file(file_name))
    {
        return;
    }
    queue_id = g_thread_manager.add_queue();    
}

void videoObtain::start()
{
    timer.start_test(m_frame_delay, get_frame_from_capture, this);
}

int videoObtain::get_frame_from_capture(videoObtain *video_obtain)
{
    cv::Mat frame;
    if (!video_obtain->m_capture.read(frame))
    {
        video_obtain->timer.stop();
        return -1;
    }
    double stamp = get_current_time();
    g_thread_manager.insert_queue(
        video_obtain->queue_id,
        basic_item(frame, stamp));
    return 0;
}

frameHandle::frameHandle(int get_queue)
{
    queue_id = g_thread_manager.add_queue();
    // 加载畸变矩阵
    load_remap_mat(AFTER_TRANS_ROWS, AFTER_TRANS_COLS);
    _try_to_expire = false;
    std::thread frame_thread(process_image, this);
    frame_thread.detach();
}

void frameHandle::screen_setting(int screen_rows, int screen_cols, int screen_channels)
{
    frameHandle::screen_rows = screen_rows;
    frameHandle::screen_cols = screen_cols;
    frameHandle::screen_channels = screen_channels;
}

int frameHandle::process_image(frameHandle *handle)
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
        cv::Mat image;
        // 缩放 填黑边
        object_rect res_area;
        handle->resize_uniform(item.first, image, cv::Size(BEFORE_TRANS_COLS, BEFORE_TRANS_ROWS), res_area);
        // 畸变
        handle->trans_remap(image, image);
        //  复制
        handle->gen_copy_img(image, image, FINAL_ROWS, FINAL_COLS);
        
        if (handle->screen_channels == 4)
        {
            // 增加透明度
            handle->add_alpha(image, image);
        }
        else if (handle->screen_channels == 2)
        {
            handle->rgb888_to_rgb565(image, image);
        }
        // cv::imwrite("output.png", image);
        break;
    }
}