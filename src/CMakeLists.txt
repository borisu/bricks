cmake_minimum_required(VERSION 3.5)

set (VCPKG_ROOT "${CMAKE_CURRENT_LIST_DIR}/../3rdparty/modules/vcpkg")
set (CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

option(NO_KAFKA_PLUGIN "No Kafka plugin" OFF)

option(NO_ZEROMQ_PLUGIN "No ZeroMQ plugin" OFF)

option(NO_OATPP_PLUGIN "No Oatpp plugin" OFF)

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

add_subdirectory(bricks_ut)
    