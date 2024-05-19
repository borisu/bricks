// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#ifdef WIN32
  #include "framework.h"
#endif

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
#include <memory>
#include <cctype>
#include <locale>
#include <random>
#include <inttypes.h>
#include <functional>
#include <queue>

#include "tinyxml2.h"
#include "base64.hpp"

#endif //PCH_H
