#include "pch.h"
#include "buffer_impl.h"

using namespace bricks;
using namespace std;

buffer_t* 
bricks::create_buffer(const char* data, size_t size)
{
    return new buffer_impl_t(data, size);
}

buffer_impl_t::buffer_impl_t(const char* data, size_t length)
{
    vec.reserve(length); // Reserve space for the characters
    std::copy(data, data + length, std::back_inserter(vec));
}

size_t
buffer_impl_t::size()
{
    return vec.size();
}

const char* 
buffer_impl_t::data()
{
    return &vec[0];
}