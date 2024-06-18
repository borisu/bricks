#
# File Groups
#
set(BRICKS_LIBEVENT_API
${CMAKE_CURRENT_SOURCE_DIR}/include/libevent_plugin.h
)

set(BRICKS_LIBEVENT_SRC
libevent_server.h
libevent_server.cpp
libevent_client.h
libevent_client.cpp
libevent_base.h
libevent_base.cpp
pch.cpp
pch.h
)

if(WIN32)
set (BRICKS_LIBEVENT_OS_SPECIFIC_SRC
 bricks_libevent.rc 
 framework.h 
 dllmain.cpp
 resource.h
)
endif()


#
# Target
#
add_library(bricks_libevent SHARED ${BRICKS_LIBEVENT_API} ${BRICKS_LIBEVENT_SRC} ${BRICKS_LIBEVENT_OS_SPECIFIC_SRC})


#
# vcpkg
#
find_package(Libevent CONFIG REQUIRED)
target_link_libraries(bricks_libevent PRIVATE libevent::core libevent::extra bricks_core bricks_logger)

#
# Configuration
#
set_property(TARGET bricks_libevent PROPERTY CXX_STANDARD 17)

target_include_directories(bricks_libevent PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR}/include
${CMAKE_CURRENT_SOURCE_DIR}/../bricks_core/include
${CMAKE_CURRENT_SOURCE_DIR}/../bricks_logger/include
)

add_compile_definitions(BRICKSLIBEVENT_EXPORTS)
  
#
# Source Groups
#
source_group("" FILES ${BRICKS_LIBEVENT_SRC})
source_group("include" FILES ${BRICKS_LIBEVENT_API})
source_group("os_specific" FILES ${BRICKS_LIBEVENT_OS_SPECIFIC_SRC})


