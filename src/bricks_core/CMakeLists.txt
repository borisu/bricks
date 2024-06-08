#
# File Groups
#
set(BRICKS_CORE_API
${CMAKE_CURRENT_SOURCE_DIR}/include/bricks.h
${CMAKE_CURRENT_SOURCE_DIR}/include/buffer.h
${CMAKE_CURRENT_SOURCE_DIR}/include/common.h
${CMAKE_CURRENT_SOURCE_DIR}/include/plugin.h
${CMAKE_CURRENT_SOURCE_DIR}/include/queue.h
${CMAKE_CURRENT_SOURCE_DIR}/include/selector.h
${CMAKE_CURRENT_SOURCE_DIR}/include/utils.h
${CMAKE_CURRENT_SOURCE_DIR}/include/xtree.h
${CMAKE_CURRENT_SOURCE_DIR}/include/timer.h
${CMAKE_CURRENT_SOURCE_DIR}/include/meta.h
)

set (BRICKS_CORE_SRC 
pch.cpp
pch.h
buffer_impl.cpp
buffer_impl.h
common.cpp
locking_queue.h
queue_impl.cpp
queue_impl.h
selector_impl.cpp
selector_impl.h
xtree_cloner.cpp
xtree_cloner.h
xtree_impl.cpp
xtree_impl.h
xtree_xml.cpp
xtree_xml.h
pubsub_server.h
pubsub_server.cpp
pubsub_client.h
pubsub_client.cpp
poller_impl.h
poller_impl.cpp
timer_impl.h
timer_impl.cpp

)

set (BRICKS_CORE_3RD_PARTY
${CMAKE_CURRENT_SOURCE_DIR}/../../3rdparty/modules/tinyxml2/tinyxml2.h
${CMAKE_CURRENT_SOURCE_DIR}/../../3rdparty/modules/tinyxml2/tinyxml2.cpp
)

if (WIN32)
set (BRICKS_CORE_OS_SPECIFIC_SRC
 bricks_core.rc 
 dllmain.cpp
 framework.h
 resource.h
)
endif()

#
# Target
#
add_library(bricks_core SHARED 
${BRICKS_CORE_API}
${BRICKS_CORE_SRC}
${BRICKS_CORE_3RD_PARTY}
${BRICKS_CORE_OS_SPECIFIC_SRC}
)

#
# Configuration Optionss
#
add_compile_definitions(BRICKS_EXPORTS)

set_property(TARGET bricks_core PROPERTY CXX_STANDARD 17)

#
# Include Directories
#
target_include_directories(bricks_core PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR}/include
${CMAKE_CURRENT_SOURCE_DIR}/../../3rdparty/modules/tinyxml2
${CMAKE_CURRENT_SOURCE_DIR}/../../3rdparty/modules/base64/include
)

#
# Source Groups
#
source_group("" FILES ${BRICKS_CORE_SRC})
source_group("os_specific" FILES ${BRICKS_CORE_OS_SPECIFIC_SRC})
source_group("include" FILES ${BRICKS_CORE_API})
source_group("3rdparty" FILES ${BRICKS_CORE_3RD_PARTY})



