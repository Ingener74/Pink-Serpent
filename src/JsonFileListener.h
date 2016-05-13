#pragma once

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

class JsonFileListener {
public:
    JsonFileListener(const std::function<void(int)>& func = {});
    virtual ~JsonFileListener();

private:
    std::atomic<bool> m_work;
    std::mutex m_mutex;
    std::condition_variable m_cond;
    std::thread m_thread;
};

