#include "ThreadPool.h"

#include <algorithm>

ThreadPool::ThreadPool(size_t num_threads) : stop(false), paused(false) {
    for (size_t i = 0; i < num_threads; ++i) {
        workers.emplace_back(&ThreadPool::worker_routine, this);
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }

    cv.notify_all();

    for (std::thread& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPool::worker_routine() {
    while (true) {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(queue_mutex);

            cv.wait(lock, [this]() { return stop || (!tasks.empty() && !paused); });

            if (stop && tasks.empty()) {
                return;
            }

            task = std::move(tasks.front());
            tasks.pop();
        }

        task();

        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            active_tasks--;

            if (active_tasks == 0) {
                cv_active_tasks_finished.notify_all();
            }
        }
    }
}

void ThreadPool::pause() {
    std::unique_lock<std::mutex> lock(queue_mutex);
    paused = true;
}

void ThreadPool::resume() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        paused = false;
    }

    cv.notify_all();
}

void ThreadPool::stop_immediately() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;

        active_tasks = std::max(0, active_tasks - static_cast<int>(tasks.size()));

        std::queue<std::function<void()>> empty_queue;
        std::swap(tasks, empty_queue);
    }

    cv.notify_all();
}

size_t ThreadPool::get_threads_count() const {
    return workers.size();
}

void ThreadPool::wait_all() {
    std::unique_lock<std::mutex> lock(queue_mutex);
    cv_active_tasks_finished.wait(lock, [this]() { return active_tasks == 0; });
}