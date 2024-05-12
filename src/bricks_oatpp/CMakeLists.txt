#
# File Groups
#
set(BRICKS_OATPP_API
${CMAKE_CURRENT_SOURCE_DIR}/include/oatpp_plugin.h
)

set(BRICKS_OATPP_SRC
dllmain.cpp
framework.h
oatpp_client.cpp
oatpp_client.h
oatpp_impl.cpp
oatpp_server.cpp
oatpp_server.h
oatpp_server_controller.cpp
oatpp_server_controller.h
pch.cpp
pch.h
)


#
# Target
#
add_library(bricks-oatpp SHARED ${BRICKS_OATPP_API} ${BRICKS_OATPP_SRC})

#
# vcpkg
#
  
#
# Configuration
#
set_property(TARGET bricks-oatpp PROPERTY CXX_STANDARD 17)

target_include_directories(bricks-oatpp PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR}/../../3rdparty/modules/oatpp/src
${CMAKE_CURRENT_SOURCE_DIR}/include
${CMAKE_CURRENT_SOURCE_DIR}/../bricks-core/include
${CMAKE_CURRENT_SOURCE_DIR}/../bricks-logger/include
)

target_link_libraries(bricks-oatpp PRIVATE bricks-core bricks-logger)

target_link_libraries(bricks-oatpp PRIVATE debug ${CMAKE_CURRENT_SOURCE_DIR}/../../3rdparty/modules/oatpp/build/src/Debug/oatpp.lib)
target_link_libraries(bricks-oatpp PRIVATE optimized ${CMAKE_CURRENT_SOURCE_DIR}/../../3rdparty/modules/oatpp/build/src/Release/oatpp.lib)

add_compile_definitions(BRICKSOATPP_EXPORTS)

#
# Source Groups
#
source_group("" FILES ${BRICKS_OATPP_SRC})
source_group("include" FILES ${BRICKS_OATPP_API})


