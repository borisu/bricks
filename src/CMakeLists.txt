cmake_minimum_required(VERSION 3.5)

set (CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(NO_KAFKA_PLUGIN "No Kafka plugin" OFF)

option(NO_ZEROMQ_PLUGIN "No ZeroMQ plugin" OFF)

option(NO_OATPP_PLUGIN "No Oatpp plugin" OFF)

option(NO_REDSIPP_PLUGIN "No Redis-plus-plus plugin" OFF)

option(NO_RABBITMQ_PLUGIN "No RabbitMq plugin" OFF)

option(NO_LIBEVENT_PLUGIN "No Libevent plugin" OFF)

enable_testing()

project(bricks)

add_subdirectory(bricks_logger)

add_subdirectory(bricks_core)

add_subdirectory(bricks_jester)

if (NOT NO_KAFKA_PLUGIN)
    add_subdirectory(bricks_kafka)
else()    
    add_compile_definitions(NO_KAFKA_PLUGIN)
endif()    

if (NOT NO_ZEROMQ_PLUGIN)
    add_subdirectory(bricks_zeromq)
else()
    add_compile_definitions(NO_ZEROMQ_PLUGIN)
endif()

if (NOT NO_OATPP_PLUGIN)
    add_subdirectory(bricks_oatpp)
else()
    add_compile_definitions(NO_OATPP_PLUGIN)
endif()

if (NOT NO_REDSIPP_PLUGIN)
    add_subdirectory(bricks_redispp)
else()
    add_compile_definitions(NO_REDSIPP_PLUGIN)
endif()

if (NOT NO_RABBITMQ_PLUGIN)
    add_subdirectory(bricks_rabbitmq)
else()
    add_compile_definitions(NO_RABBITMQ_PLUGIN)
endif()

if (NOT NO_LIBEVENT_PLUGIN)
    add_subdirectory(bricks_libevent)
else()
    add_compile_definitions(NO_LIBEVENT_PLUGIN)
endif()

add_subdirectory(bricks_ut)
    