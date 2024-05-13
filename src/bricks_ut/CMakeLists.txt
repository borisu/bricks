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

set(BRICKS_UT_SRC
generic_tests.h
jester_test.cpp
pch.cpp
pch.h
publish_subsribe_tests.cpp
request_response_tests.cpp
bidi_tests.cpp
xtree_test.cpp
)

#
# Target
#
add_executable(bricks_ut ${BRICKS_UT_SRC})

if (WIN32)
 target_sources(bricks_ut PRIVATE bricks_ut.rc resource.h)
endif()

#
# vcpkg
#
  
#
# Configuration
#
set_property(TARGET bricks_ut PROPERTY CXX_STANDARD 17)

target_include_directories(bricks_ut PRIVATE 
${CMAKE_CURRENT_SOURCE_DIR}/../bricks_core/include
${CMAKE_CURRENT_SOURCE_DIR}/../bricks_logger/include
${CMAKE_CURRENT_SOURCE_DIR}/../bricks_jester/include
)

target_link_libraries(bricks_ut PRIVATE GTest::gtest_main bricks_core bricks_logger bricks_jester)

add_custom_command(TARGET bricks_ut POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "$<TARGET_FILE_DIR:bricks_core>" $<TARGET_FILE_DIR:bricks_ut>)
  
add_custom_command(TARGET bricks_ut POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "$<TARGET_FILE_DIR:bricks_logger>" $<TARGET_FILE_DIR:bricks_ut>)
  
add_custom_command(TARGET bricks_ut POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "$<TARGET_FILE_DIR:bricks_jester>" $<TARGET_FILE_DIR:bricks_ut>)

if (NOT NO_KAFKA_PLUGIN)
    add_custom_command(TARGET bricks_ut POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "$<TARGET_FILE_DIR:bricks_kafka>" $<TARGET_FILE_DIR:bricks_ut>)
    target_include_directories(bricks_ut PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../bricks_kafka/include)
    target_sources(bricks_ut PRIVATE kafka_test.cpp)
    target_link_libraries(bricks_ut PRIVATE bricks_kafka)
endif()

if (NOT NO_ZEROMQ_PLUGIN)
    add_custom_command(TARGET bricks_ut POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "$<TARGET_FILE_DIR:bricks_zeromq>" $<TARGET_FILE_DIR:bricks_ut>)
    target_include_directories(bricks_ut PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../bricks_zeromq/include)
    target_sources(bricks_ut PRIVATE zeromq_test.cpp)
    target_link_libraries(bricks_ut PRIVATE bricks_zeromq)
endif()  

if (NOT NO_OATPP_PLUGIN)
    add_custom_command(TARGET bricks_ut POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "$<TARGET_FILE_DIR:bricks_oatpp>" $<TARGET_FILE_DIR:bricks_ut>)
    target_include_directories(bricks_ut PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../bricks_oatpp/include)
    target_sources(bricks_ut PRIVATE oatpp_test.cpp)
    target_link_libraries(bricks_ut PRIVATE bricks_oatpp)    
endif()  
 



