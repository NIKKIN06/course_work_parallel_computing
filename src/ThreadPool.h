#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>

class ThreadPool {
   private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queue_mutex;
    std::condition_variable cv;

    bool stop;
    bool paused;

    int active_tasks{0};
    std::condition_variable cv_active_tasks_finished;

    void worker_routine();

   public:
    explicit ThreadPool(size_t num_threads = 8);
    ~ThreadPool();

    void pause();
    void resume();
    void stop_immediately();

    size_t get_threads_count() const;
    void wait_all();

    template <class F, class... Args>
    void add_task(F&& f, Args&&... args);
};

template <class F, class... Args>
void ThreadPool::add_task(F&& f, Args&&... args) {
    auto task = [func = std::forward<F>(f), ... args = std::forward<Args>(args)]() mutable {
        func(std::move(args)...);
    };

    {
        std::unique_lock<std::mutex> lock(queue_mutex);

        if (stop) {
            throw std::runtime_error("Adding task to ThreadPool has been stopped");
        }

        tasks.push(std::function<void()>(task));
        active_tasks++;
    }

    cv.notify_one();
}