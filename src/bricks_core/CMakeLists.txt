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
)

set (BRICKS_CORE_SRC 
pch.cpp
pch.h
buffer_impl.cpp
buffer_impl.h
common.cpp
dllmain.cpp
framework.h
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
)

set (BRICKS_CORE_3RD_PARTY
${CMAKE_CURRENT_SOURCE_DIR}/../../3rdparty/modules/tinyxml2/tinyxml2.h
${CMAKE_CURRENT_SOURCE_DIR}/../../3rdparty/modules/tinyxml2/tinyxml2.cpp
)

#
# Target
#
add_library(bricks_core SHARED 
${BRICKS_CORE_API}
${BRICKS_CORE_SRC}
${BRICKS_CORE_3RD_PARTY}
)

target_sources(bricks_core PRIVATE bricks_core.rc)

#
# Configuration
#
add_compile_definitions(BRICKS_EXPORTS)

set_property(TARGET bricks_core PROPERTY CXX_STANDARD 17)

target_include_directories(bricks_core PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR}/include
${CMAKE_CURRENT_SOURCE_DIR}/../../3rdparty/modules/tinyxml2
${CMAKE_CURRENT_SOURCE_DIR}/../../3rdparty/modules/base64/include
)

#
# Source Groups
#
source_group("" FILES ${BRICKS_CORE_SRC})
source_group("include" FILES ${BRICKS_CORE_API})
source_group("3rdparty" FILES ${BRICKS_CORE_3RD_PARTY})



