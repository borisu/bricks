//
// pch.h
//

#pragma once

#include <memory>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include "gtest/gtest.h"
#include <cstring>
#include "bricks.h"
#include "logger.h"
#include "jester_plugin.h"

#ifdef WIN32
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif