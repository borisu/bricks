#
# File Groups
#
set(BRICKS_ZEROMQ_API
${CMAKE_CURRENT_SOURCE_DIR}/include/zeromq_plugin.h
)

set(BRICKS_ZEROMQ_SRC
pch.cpp
pch.h
zeromq_bidi.cpp
zeromq_bidi.h
zeromq_plugin.cpp
zeromq_publisher.cpp
zeromq_publisher.h
zeromq_service.cpp
zeromq_service.h
zeromq_subscriber.cpp
zeromq_subscriber.h
)

if(WIN32)
set (BRICKS_OATPP_OS_SPECIFIC_SRC
 bricks_zeromq.rc 
 framework.h 
 dllmain.cpp
 resource.h
)
endif()



#
# Target
#
add_library(bricks_zeromq SHARED ${BRICKS_ZEROMQ_API} ${BRICKS_ZEROMQ_SRC} ${BRICKS_OATPP_OS_SPECIFIC_SRC})


#
# vcpkg
#
find_package(czmq CONFIG REQUIRED)
target_link_libraries(bricks_zeromq PRIVATE czmq-static bricks_core bricks_logger)

#
# Configuration
#
set_property(TARGET bricks_zeromq PROPERTY CXX_STANDARD 17)

target_include_directories(bricks_zeromq PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR}/include
${CMAKE_CURRENT_SOURCE_DIR}/../bricks_core/include
${CMAKE_CURRENT_SOURCE_DIR}/../bricks_logger/include
)

add_compile_definitions(BRICKSZEROMQ_EXPORTS)
  


#
# Source Groups
#
source_group("" FILES ${BRICKS_ZEROMQ_SRC})
source_group("include" FILES ${BRICKS_ZEROMQ_API})
source_group("os_specific" FILES ${BRICKS_OATPP_OS_SPECIFIC_SRC})


