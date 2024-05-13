#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <optional>
#include <functional>
#include <memory>
#include <list>

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
		BRICKS_REMOTE_ERROR = 3,
	};

	class brick_t
	{
	public:
		virtual void release() = 0;
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

	
}




