#pragma once
#include <opencv2/core.hpp>
#include <queue>
#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>

typedef std::pair<cv::Mat, double> basic_item;

struct mutex_struct{
    std::mutex mtx;
};
class threadManage
{
public:
    
    typedef std::queue<basic_item> basic_queue;
    int add_queue();
    int insert_queue(int id, basic_item insert_item);
    bool get_queue(int id, basic_item &get_item);
    int get_queue_size(int id);
    threadManage();

private:
    std::vector<basic_queue> m_queues;
    std::mutex m_mutexs[10];
    std::condition_variable global_cond;
};

extern threadManage g_thread_manager;

