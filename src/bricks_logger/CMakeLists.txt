#
# File Groups
#
set(BRICKS_LOGGER_API
${CMAKE_CURRENT_SOURCE_DIR}/include/logger.h
)

set(BRICKS_LOGGER_SRC
framework.h 
pch.h
dllmain.cpp  
logger.cpp 
pch.cpp 
)

#
# Target
#
add_library(bricks_logger SHARED ${BRICKS_LOGGER_API} ${BRICKS_LOGGER_SRC})

target_sources(bricks_logger PRIVATE bricks_logger.rc)


#
# Configuration
#
set_property(TARGET bricks_logger PROPERTY CXX_STANDARD 17)

target_include_directories(bricks_logger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_compile_definitions(BRICKSLOGGER_EXPORTS)

#
# Source Groups
#
source_group("" FILES ${BRICKS_LOGGER_SRC})
source_group("include" FILES ${BRICKS_LOGGER_API})


