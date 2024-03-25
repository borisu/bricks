#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <optional>
#include <functional>

using namespace std;

#ifdef BRICKS_EXPORTS
	#define BRICKS_API __declspec(dllexport)
#else
	#define BRICKS_API __declspec(dllimport)
#endif

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
	};

	typedef vector<char> buffer_t;

	class brick_t
	{
	public:
		virtual void release() = 0;
	};

	struct bricks_destroyer
	{
		void operator()(brick_t* ptr) const { ptr->release(); };
	};


	
}




