#include "pch.h"
#include "bricks.h"
#include "locking_queue.h"

using namespace bricks;

void bricks::brick_destroy(brick_t* ptr) { ptr->release(); };