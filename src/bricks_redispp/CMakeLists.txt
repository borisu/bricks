#
# File Groups
#
set(BRICKS_REDISPP_API
${CMAKE_CURRENT_SOURCE_DIR}/include/redispp_plugin.h
)

set(BRICKS_REDISPP_SRC
redispp_subscriber.h
redispp_subscriber.cpp
redispp_psubscriber.h
redispp_psubscriber.cpp
redispp_publisher.h
redispp_publisher.cpp
pch.cpp
pch.h
)

if(WIN32)
set (BRICKS_REDISPP_OS_SPECIFIC_SRC
 bricks_redispp.rc 
 framework.h 
 dllmain.cpp
 resource.h
)
endif()


#
# Target
#
add_library(bricks_redispp SHARED ${BRICKS_REDISPP_API} ${BRICKS_REDISPP_SRC} ${BRICKS_REDISPP_OS_SPECIFIC_SRC})


#
# vcpkg
#
find_package(redis++ CONFIG REQUIRED)
if(WIN32)
target_link_libraries(bricks_redispp PRIVATE  redis++::redis++ bricks_core bricks_logger)
endif()
if(UNIX)
target_link_libraries(bricks_redispp PRIVATE  redis++::redis++_static bricks_core bricks_logger)
endif()
#
# Configuration
#
set_property(TARGET bricks_redispp PROPERTY CXX_STANDARD 17)

target_include_directories(bricks_redispp PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR}/include
${CMAKE_CURRENT_SOURCE_DIR}/../bricks_core/include
${CMAKE_CURRENT_SOURCE_DIR}/../bricks_logger/include
)

add_compile_definitions(BRICKSREDISPP_EXPORTS)
  
#
# Source Groups
#
source_group("" FILES ${BRICKS_REDISPP_SRC})
source_group("include" FILES ${BRICKS_REDISPP_API})
source_group("os_specific" FILES ${BRICKS_REDISPP_OS_SPECIFIC_SRC})


