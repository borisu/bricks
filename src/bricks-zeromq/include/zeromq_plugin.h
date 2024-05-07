#pragma once
#include "bricks.h"

#ifdef BRICKSZEROMQ_EXPORTS
	#define BRICKSZEROMQ_API __declspec(dllexport)
#else
	#define BRICKSZEROMQ_API __declspec(dllimport)
#endif

namespace bricks::plugins { 

BRICKSZEROMQ_API publisher_plugin_t*
	create_zeromq_publisher();

BRICKSZEROMQ_API subscriber_plugin_t*
	create_zeromq_subscriber();

BRICKSZEROMQ_API bidi_plugin_t*
	create_zeromq_bidi();
	
}
 
