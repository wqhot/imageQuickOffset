#pragma once
#include <queue>
#include <vector>
#include <map>
#include <thread>
#include <mutex>

class threadManage
{
public:
    typedef std::pair<unsigned char *, double> basic_item;
    typedef std::queue<basic_item> basic_queue;
    int add_queue();
    int insert_queue(int id, basic_item insert_item);
    int get_queue(int id, basic_item &get_item);
    int get_queue_size(int id);
    int register_thread();
    threadManage();

private:
    std::vector<basic_queue> m_queues;
    std::vector<std::mutex> m_mutexs;
    std::vector<std::thread> m_threads;
};

