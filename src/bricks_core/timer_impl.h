#pragma once
#include "timer.h"

using namespace std;

namespace bricks
{
	class timer_impl_t : public bricks::timer_t
	{
	public:
	
		timer_impl_t();

		~timer_impl_t();

		virtual bricks_error_code_e init(cb_queue_t* queue, const xtree_t* options) override;

		virtual bricks_error_code_e schedule_timer(const callback_t& callback, const duration_t& delay, int& handle) override;

		virtual bricks_error_code_e start() override;

		virtual bricks_error_code_e cancel_timer(int&) override;

		virtual void worker();

		virtual void release() override { delete this; };

	protected:

		struct timer_info_t {

			time_point_t expiration;

			callback_t callback;

			int handle;

			bool canceled;

			bool operator>(const timer_info_t& other) const {
				return expiration > other.expiration;
			}
		};

		std::list<timer_info_t> timer_queue;

		std::mutex mtx;

		std::condition_variable cv;

		std::thread worker_thread;

		atomic<bool> stop;

		cb_queue_t* cb_queue = nullptr;

		atomic<bool> initiated = false;

		atomic<bool> started = false;

		int handle_counter = 0;

	};

}


