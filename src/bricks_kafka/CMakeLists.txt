#
# File Groups
#
set(BRICKS_KAFKA_API
${CMAKE_CURRENT_SOURCE_DIR}/include/rdkafka_plugin.h
)

set(BRICKS_KAFKA_SRC
dllmain.cpp
framework.h
kafka_publisher.cpp
kafka_publisher.h
kafka_service.cpp
kafka_service.h
kafka_subscriber.cpp
kafka_subscriber.h
pch.cpp
pch.h
rdkafka_plugin.cpp
)


#
# Target
#
add_library(bricks_kafka SHARED ${BRICKS_KAFKA_API} ${BRICKS_KAFKA_SRC})

#
# vcpkg
#
find_package(RdKafka CONFIG REQUIRED)
target_link_libraries(bricks_kafka PRIVATE RdKafka::rdkafka bricks_core bricks_logger)



#
# Configuration
#
set_property(TARGET bricks_kafka PROPERTY CXX_STANDARD 17)

target_include_directories(bricks_kafka PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR}/include
${CMAKE_CURRENT_SOURCE_DIR}/../bricks_core/include
${CMAKE_CURRENT_SOURCE_DIR}/../bricks_logger/include
)

add_compile_definitions(BRICKSKAFKA_EXPORTS)
  


#
# Source Groups
#
source_group("" FILES ${BRICKS_KAFKA_SRC})
source_group("include" FILES ${BRICKS_KAFKA_API})


