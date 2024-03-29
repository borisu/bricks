// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"
#include <stdio.h>
#include <atomic>
#include <vector>
#include <any>
#include <map>
#include <string>
#include <list>
#include <optional>
#include <string_view>
#include <iostream>
#include <algorithm>
#include <thread>
#include <mutex>
#include <deque>


#include "tinyxml2.h"
#include "../../modules/base64/include/base64.hpp"

#ifdef WIN32
#include <objbase.h> // For CoCreateGuid
#else

#endif



#endif //PCH_H
