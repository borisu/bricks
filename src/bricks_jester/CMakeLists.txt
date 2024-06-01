#
# File Groups
#
set(BRICKS_JESTER_API
${CMAKE_CURRENT_SOURCE_DIR}/include/jester_plugin.h
)

set(BRICKS_JESTER_SRC
jester_bidi.cpp
jester_bidi.h
jester_pubsub.cpp
jester_pubsub.h
jester_reqrep.cpp
jester_reqrep.h
pch.cpp
pch.h
)


if (WIN32)
set (BRICKS_JESTER_OS_SPECIFIC_SRC
 bricks_jester.rc 
 resource.h 
 dllmain.cpp 
 framework.h
)
endif()

#
# Target
#
add_library(bricks_jester SHARED 
 ${BRICKS_JESTER_API} 
 ${BRICKS_JESTER_SRC}
 ${BRICKS_JESTER_OS_SPECIFIC_SRC}
 )



#
# vcpkg
#
target_link_libraries(bricks_jester PRIVATE bricks_core bricks_logger)

#
# Configuration
#
set_property(TARGET bricks_jester PROPERTY CXX_STANDARD 17)

target_include_directories(bricks_jester PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR}/include
${CMAKE_CURRENT_SOURCE_DIR}/../bricks_core/include
${CMAKE_CURRENT_SOURCE_DIR}/../bricks_logger/include
)

add_compile_definitions(BRICKSJESTER_EXPORTS)
 


#
# Source Groups
#
source_group("" FILES ${BRICKS_JESTER_SRC})
source_group("include" FILES ${BRICKS_JESTER_API})
source_group("os_specific" FILES ${BRICKS_JESTER_OS_SPECIFIC_SRC})


