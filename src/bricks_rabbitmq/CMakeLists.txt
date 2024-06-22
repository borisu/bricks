#
# File Groups
#
set(BRICKS_RABBITMQ_API
${CMAKE_CURRENT_SOURCE_DIR}/include/rabbitmq_plugin.h
)

set(BRICKS_RABBITMQ_SRC
rabbitmq_subscriber.h
rabbitmq_subscriber.cpp
rabbitmq_publisher.h
rabbitmq_publisher.cpp
rabbitmq_utils.cpp
rabbitmq_utils.h
rabbitmq_base.h
rabbitmq_base.cpp
pch.cpp
pch.h
)

if(WIN32)
set (BRICKS_RABBITMQ_OS_SPECIFIC_SRC
 bricks_rabbitmq.rc 
 framework.h 
 dllmain.cpp
 resource.h
)
endif()


#
# Target
#
add_library(bricks_rabbitmq SHARED ${BRICKS_RABBITMQ_API} ${BRICKS_RABBITMQ_SRC} ${BRICKS_RABBITMQ_OS_SPECIFIC_SRC})


#
# vcpkg
#
find_package(rabbitmq-c CONFIG REQUIRED)
if(WIN32)
target_link_libraries(bricks_rabbitmq PRIVATE rabbitmq::rabbitmq bricks_core bricks_logger)
endif()
if(LINUX)
target_link_libraries(bricks_rabbitmq PRIVATE rabbitmq::rabbitmq-static bricks_core bricks_logger)
endif()

#
# Configuration
#
set_property(TARGET bricks_rabbitmq PROPERTY CXX_STANDARD 17)

target_include_directories(bricks_rabbitmq PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR}/include
${CMAKE_CURRENT_SOURCE_DIR}/../bricks_core/include
${CMAKE_CURRENT_SOURCE_DIR}/../bricks_logger/include
)

add_compile_definitions(BRICKSRABBITMQ_EXPORTS)
  
#
# Source Groups
#
source_group("" FILES ${BRICKS_RABBITMQ_SRC})
source_group("include" FILES ${BRICKS_RABBITMQ_API})
source_group("os_specific" FILES ${BRICKS_RABBITMQ_OS_SPECIFIC_SRC})


