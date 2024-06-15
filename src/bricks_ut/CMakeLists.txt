# https://google.github.io/googletest/quickstart-cmake.html
set (CMAKE_DISABLE_FIND_PACKAGE_Python TRUE)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set (BRICKS_UT_SRC
pch.cpp
pch.h
)

set (BRICKS_UT_CORE_TESTS_SRC
xtree_test.cpp
)

set (BRICKS_UT_GENERIC_TESTS_SRC
generic_tests.h
publish_subsribe_tests.cpp
request_response_tests.cpp
bidi_tests.cpp
)

set (BRICKS_UT_PLUGIN_TESTS_SRC
jester_test.cpp
)

if (WIN32)
set (BRICKS_UT_OS_SPECIFIC_SRC
 bricks_ut.rc 
 resource.h
)
endif()


set (BRICKS_UT_INCLUDE
${CMAKE_CURRENT_SOURCE_DIR}/../bricks_core/include
${CMAKE_CURRENT_SOURCE_DIR}/../bricks_logger/include
${CMAKE_CURRENT_SOURCE_DIR}/../bricks_jester/include
)

set (BRICKS_UT_LIBS 
GTest::gtest_main 
bricks_core 
bricks_logger 
bricks_jester
)

if (NOT NO_KAFKA_PLUGIN)
    set(BRICKS_UT_INCLUDE ${BRICKS_UT_INCLUDE} ${CMAKE_CURRENT_SOURCE_DIR}/../bricks_kafka/include)
    set(BRICKS_UT_PLUGIN_TESTS_SRC ${BRICKS_UT_PLUGIN_TESTS_SRC} kafka_test.cpp)
    set(BRICKS_UT_LIBS ${BRICKS_UT_LIBS} bricks_kafka)
endif()

if (NOT NO_ZEROMQ_PLUGIN)
    set(BRICKS_UT_INCLUDE ${BRICKS_UT_INCLUDE} ${CMAKE_CURRENT_SOURCE_DIR}/../bricks_zeromq/include)
    set(BRICKS_UT_PLUGIN_TESTS_SRC ${BRICKS_UT_PLUGIN_TESTS_SRC} zeromq_test.cpp)
    set(BRICKS_UT_LIBS ${BRICKS_UT_LIBS} bricks_zeromq)
endif()  

if (NOT NO_OATPP_PLUGIN)
    set(BRICKS_UT_INCLUDE ${BRICKS_UT_INCLUDE} ${CMAKE_CURRENT_SOURCE_DIR}/../bricks_oatpp/include)
    set(BRICKS_UT_PLUGIN_TESTS_SRC ${BRICKS_UT_PLUGIN_TESTS_SRC} oatpp_test.cpp)
    set(BRICKS_UT_LIBS ${BRICKS_UT_LIBS} bricks_oatpp)
endif()  

if (NOT NO_REDISPP_PLUGIN)
    set(BRICKS_UT_INCLUDE ${BRICKS_UT_INCLUDE} ${CMAKE_CURRENT_SOURCE_DIR}/../bricks_redispp/include)
    set(BRICKS_UT_PLUGIN_TESTS_SRC ${BRICKS_UT_PLUGIN_TESTS_SRC} redispp_test.cpp)
    set(BRICKS_UT_LIBS ${BRICKS_UT_LIBS} bricks_redispp)
endif()  

if (NOT NO_RABBITQ_PLUGIN)
    set(BRICKS_UT_INCLUDE ${BRICKS_UT_INCLUDE} ${CMAKE_CURRENT_SOURCE_DIR}/../bricks_rabbitmq/include)
    set(BRICKS_UT_PLUGIN_TESTS_SRC ${BRICKS_UT_PLUGIN_TESTS_SRC} rabbitmq_test.cpp)
    set(BRICKS_UT_LIBS ${BRICKS_UT_LIBS} bricks_rabbitmq)
endif()  


#
# Target
#
add_executable(bricks_ut ${BRICKS_UT_SRC} ${BRICKS_UT_CORE_TESTS_SRC} ${BRICKS_UT_GENERIC_TESTS_SRC} ${BRICKS_UT_PLUGIN_TESTS_SRC} ${BRICKS_UT_OS_SPECIFIC_SRC})
target_include_directories(bricks_ut PRIVATE ${BRICKS_UT_INCLUDE})
target_link_libraries(bricks_ut PRIVATE ${BRICKS_UT_LIBS})

add_custom_command(TARGET bricks_ut POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "$<TARGET_FILE_DIR:bricks_core>"   $<TARGET_FILE_DIR:bricks_ut>)
add_custom_command(TARGET bricks_ut POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "$<TARGET_FILE_DIR:bricks_logger>" $<TARGET_FILE_DIR:bricks_ut>)
add_custom_command(TARGET bricks_ut POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "$<TARGET_FILE_DIR:bricks_jester>" $<TARGET_FILE_DIR:bricks_ut>)
if (NOT NO_KAFKA_PLUGIN)
    add_custom_command(TARGET bricks_ut POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "$<TARGET_FILE_DIR:bricks_kafka>" $<TARGET_FILE_DIR:bricks_ut>)
endif()
if (NOT NO_ZEROMQ_PLUGIN)
    add_custom_command(TARGET bricks_ut POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "$<TARGET_FILE_DIR:bricks_zeromq>" $<TARGET_FILE_DIR:bricks_ut>)
endif()  
if (NOT NO_OATPP_PLUGIN)
    add_custom_command(TARGET bricks_ut POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "$<TARGET_FILE_DIR:bricks_oatpp>" $<TARGET_FILE_DIR:bricks_ut>)
endif()  
if (NOT NO_REDISPP_PLUGIN)
    add_custom_command(TARGET bricks_ut POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "$<TARGET_FILE_DIR:bricks_redispp>" $<TARGET_FILE_DIR:bricks_ut>)
endif()  
if (NOT NO_RABBITMQ_PLUGIN)
    add_custom_command(TARGET bricks_ut POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "$<TARGET_FILE_DIR:bricks_rabbitmq>" $<TARGET_FILE_DIR:bricks_ut>)
endif()

#
# Configuration
#
set_property(TARGET bricks_ut PROPERTY CXX_STANDARD 17)

#
# Source Groups
#
source_group("" FILES ${BRICKS_UT_SRC})
source_group("core_tests" FILES ${BRICKS_UT_CORE_TESTS_SRC})
source_group("generic_tests" FILES ${BRICKS_UT_GENERIC_TESTS_SRC})
source_group("plugin_tests" FILES ${BRICKS_UT_PLUGIN_TESTS_SRC})
source_group("os_specific" FILES ${BRICKS_UT_OS_SPECIFIC_SRC})

