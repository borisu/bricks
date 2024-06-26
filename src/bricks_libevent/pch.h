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

#include <ctype.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#include <functional>
#include <string>
#include <map>
#include <atomic>
#include <thread>
#include <mutex>

#include <event2/event-config.h>
#include <event2/thread.h>
#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>

#include "logger.h"
#include "bricks.h"
#include "utils.h"

#endif //PCH_H
