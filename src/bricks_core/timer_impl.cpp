#include "pch.h"
#include "timer_impl.h"
#include "utils.h"


using namespace bricks;

BRICKS_API bricks::timer_t* bricks::create_timer()
{
    return new timer_impl_t();
}

timer_impl_t::timer_impl_t(): 
    stop(false) {
    
}

timer_impl_t::~timer_impl_t() {
    stop = true;
    cv.notify_one(); // Wake up the worker thread to exit
    if (worker_thread.joinable()) {
        worker_thread.join();
    }
}

bricks_error_code_e 
timer_impl_t::init(cb_queue_t* queue, const xtree_t* options = nullptr)
{
    ASSERT_NOT_INITIATED;
    ASSERT_NOT_STARTED;

    this->cb_queue = queue;

    initiated = true;

    return BRICKS_SUCCESS;
}

bricks_error_code_e 
timer_impl_t::start()
{
    ASSERT_INITIATED;
    ASSERT_NOT_STARTED;

    worker_thread = std::thread(&timer_impl_t::worker, this);

    started = true;

    return BRICKS_SUCCESS;
}


bricks_error_code_e
timer_impl_t::schedule_timer(const callback_t& callback, const duration_t& delay, int& handle)
{
    ASSERT_INITIATED;
    ASSERT_STARTED;

    {
        time_point_t expiration = clock_t::now() + delay;

        handle = handle_counter++;

        std::unique_lock<std::mutex> lock(mtx);
        timer_queue.push_back(timer_info_t{ expiration, callback, handle, false });
    }
    cv.notify_one(); // Notify the worker thread about the new time

    return BRICKS_SUCCESS;
}

bricks_error_code_e 
timer_impl_t::cancel_timer(int& handle)
{
    std::unique_lock<std::mutex> lock(mtx);
    for (auto& t : timer_queue)
    {
        if (t.handle == handle)
        {
            t.canceled = true;
            cv.notify_one();
            return BRICKS_SUCCESS;
        }
    }

    return BRICKS_INVALID_PARAM;
}

void timer_impl_t::worker() {

    while (!stop) {
        std::unique_lock<std::mutex> lock(mtx);

        if (timer_queue.empty()) {
            cv.wait(lock);
        }
        else
        {
            auto time_info = timer_queue.front();
            if (time_info.canceled)
            {
                timer_queue.pop_front();
                continue;
            }
            auto next_expiration = time_info.expiration;
            if (cv.wait_until(lock, next_expiration) == std::cv_status::timeout) {
                    cb_queue->enqueue(time_info.callback);
                    timer_queue.pop_front();
            }
            else
            {
                if (time_info.canceled)
                    timer_queue.pop_front();
            }
        }
    }
}
