#
# File Groups
#
set(BRICKS_LOGGER_API
${CMAKE_CURRENT_SOURCE_DIR}/include/logger.h
)

set(BRICKS_LOGGER_SRC
pch.h
logger.cpp 
pch.cpp 
)

if(WIN32)
set (BRICKS_LOGGER_OS_SPECIFIC_SRC
 bricks_logger.rc 
 framework.h 
 dllmain.cpp
 resource.h
)
endif()


#
# Target
#
add_library(bricks_logger SHARED ${BRICKS_LOGGER_API} ${BRICKS_LOGGER_SRC} ${BRICKS_LOGGER_OS_SPECIFIC_SRC})



#
# Configuration
#
set_property(TARGET bricks_logger PROPERTY CXX_STANDARD 17)
set_property(TARGET bricks_logger PROPERTY C_STANDARD 11)

target_include_directories(bricks_logger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_compile_definitions(BRICKSLOGGER_EXPORTS)

#
# Source Groups
#
source_group("" FILES ${BRICKS_LOGGER_SRC})
source_group("include" FILES ${BRICKS_LOGGER_API})
source_group("os_specific" FILES ${BRICKS_LOGGER_OS_SPECIFIC_SRC})


