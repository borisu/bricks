#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <optional>
#include <functional>
#include <memory>
#include <list>
#include <chrono>
#include <future>

#ifdef WIN32
 #ifdef BRICKS_EXPORTS
 	 #define BRICKS_API __declspec(dllexport)
 #else
	 #define BRICKS_API __declspec(dllimport)
 #endif
#else
   #define BRICKS_API
#endif

using namespace std;

namespace bricks
{
	typedef uintptr_t bricks_handle_t;
	enum bricks_error_code_e
	{
		BRICKS_SUCCESS = 0,
		BRICKS_FAILURE_GENERIC = 1,
		BRICKS_INVALID_PARAM = 2,
		BRICKS_INVALID_STATE = 3,
		BRICKS_3RD_PARTY_ERROR = 4,
		BRICKS_TIMEOUT = 5,
		BRICKS_NOT_SUPPORTED = 6,
		BRICKS_REMOTE_ERROR = 7,
		BRICKS_OBJECT_DESTROYED = 8,
	};

	class brick_t
	{
	public:
		virtual void release() = 0;
	};

	class BRICKS_API startable_brick_t : public brick_t
	{
	public:
		virtual bricks_error_code_e start() { return BRICKS_SUCCESS; };
	};

	void brick_destroy(brick_t* ptr);

	template<class T>
	auto make_shared_brick(T* ptr) { return std::shared_ptr<T>(ptr, brick_destroy); }

	// for unique ptr
	struct bricks_destroyer
	{
		void operator()(brick_t* ptr) const { ptr->release(); };
	};

	template <class B>
	using brick_uptr = std::unique_ptr<B, bricks_destroyer>;

	typedef std::vector<char> vector_t;

	typedef std::promise<bricks_error_code_e> bricks_promise_t;

	typedef std::future<bricks_error_code_e> bricks_future_t;
	
}




