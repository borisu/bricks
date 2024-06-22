#include "pch.h"
#include "bricks.h"
#include "locking_queue.h"
#include "utils.h"

using namespace bricks;

thread_local std::random_device rd; // Obtain a random number from hardware
thread_local std::mt19937_64 gen(rd()); // Seed the generator (Mersenne Twister for 64-bit numbers)
thread_local std::uniform_int_distribution<uint64_t> distrib(1, UINT64_MAX); // Define the rang
std::mutex mtx;

void 
bricks::brick_destroy(brick_t* ptr) 
{
     ptr->release(); 
};

void 
bricks::generate_random_64hex_str(hex64_str_t hex_buffer)
{
    SYNCHRONIZED(mtx);

    int64_t req_handle = distrib(gen);

	snprintf(hex_buffer, 64, "0x%" PRIx64, req_handle);
}