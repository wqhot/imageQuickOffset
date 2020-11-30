#include <thread_manage.h>
#include <iostream>

threadManage::threadManage()
{
    m_queues.clear();
}

int threadManage::add_queue()
{
    int queue_id = m_queues.size();
    m_queues.push_back(basic_queue());
    return queue_id;
}

int threadManage::insert_queue(int id, basic_item insert_item)
{
    std::unique_lock<std::mutex> lock(m_mutexs[id]);
    while (m_queues[id].size() > 10)
    {
        m_queues[id].pop();
        std::cout << "img drop!" << std::endl;
    }
    m_queues[id].push(insert_item);
    global_cond.notify_all();
    lock.unlock();
    return 0;
}

bool threadManage::get_queue(int id, basic_item &get_item)
{
    std::unique_lock<std::mutex> lock(m_mutexs[id]);
    if (m_queues[id].empty())
    {
        global_cond.wait(lock);
    }
    if (m_queues[id].empty())
    {
        return false;
    }
    // m_queues[id].front().first.copyTo(get_item.first);
    // get_item.second = m_queues[id].front().second;
    get_item = m_queues[id].front();
    m_queues[id].pop();
    lock.unlock();
    return true;
}

int threadManage::get_queue_size(int id)
{
    std::unique_lock<std::mutex> lock(m_mutexs[id]);
    if (m_queues[id].empty())
    {
        global_cond.wait(lock);
    }
    if (m_queues[id].empty())
    {
        return 0;
    }
    int length = m_queues[id].size();
    lock.unlock();
    return length;
}