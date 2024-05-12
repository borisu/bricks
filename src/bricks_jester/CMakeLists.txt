#
# File Groups
#
set(BRICKS_JESTER_API
${CMAKE_CURRENT_SOURCE_DIR}/include/jester_plugin.h
)

set(BRICKS_JESTER_SRC
dllmain.cpp
framework.h
jester_bidi.cpp
jester_bidi.h
jester_pubsub.cpp
jester_pubsub.h
jester_reqrep.cpp
jester_reqrep.h
pch.cpp
pch.h
)


#
# Target
#
add_library(bricks-jester SHARED ${BRICKS_JESTER_API} ${BRICKS_JESTER_SRC})

#
# vcpkg
#
target_link_libraries(bricks-jester PRIVATE bricks-core bricks-logger)

#
# Configuration
#
set_property(TARGET bricks-jester PROPERTY CXX_STANDARD 17)

target_include_directories(bricks-jester PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR}/include
${CMAKE_CURRENT_SOURCE_DIR}/../bricks-core/include
${CMAKE_CURRENT_SOURCE_DIR}/../bricks-logger/include
)

add_compile_definitions(BRICKSJESTER_EXPORTS)
 


#
# Source Groups
#
source_group("" FILES ${BBRICKS_JESTER_SRC})
source_group("include" FILES ${BBRICKS_JESTER_API})


