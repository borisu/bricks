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
packages.config
pch.cpp
pch.h
publish_subsribe_tests.cpp
request_response_tests.cpp
bidi_tests.cpp
resource.h
xtree_test.cpp
)

#
# Target
#
add_executable(bricks-ut ${BRICKS_UT_SRC})

#
# vcpkg
#
  
#
# Configuration
#
set_property(TARGET bricks-ut PROPERTY CXX_STANDARD 17)

target_include_directories(bricks-ut PRIVATE 
${CMAKE_CURRENT_SOURCE_DIR}/../bricks-core/include
${CMAKE_CURRENT_SOURCE_DIR}/../bricks-logger/include
${CMAKE_CURRENT_SOURCE_DIR}/../bricks-jester/include
)

target_link_libraries(bricks-ut PRIVATE GTest::gtest_main bricks-core bricks-logger bricks-jester)

add_custom_command(TARGET bricks-ut POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "$<TARGET_FILE_DIR:bricks-core>" $<TARGET_FILE_DIR:bricks-ut>)
  
add_custom_command(TARGET bricks-ut POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "$<TARGET_FILE_DIR:bricks-logger>" $<TARGET_FILE_DIR:bricks-ut>)
  
add_custom_command(TARGET bricks-ut POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "$<TARGET_FILE_DIR:bricks-jester>" $<TARGET_FILE_DIR:bricks-ut>)

if (NOT NO_KAFKA_PLUGIN)
    add_custom_command(TARGET bricks-ut POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "$<TARGET_FILE_DIR:bricks-kafka>" $<TARGET_FILE_DIR:bricks-ut>)
    target_include_directories(bricks-ut PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../bricks-kafka/include)
    target_sources(bricks-ut PRIVATE kafka_test.cpp)
    target_link_libraries(bricks-ut PRIVATE bricks-kafka)
endif()

if (NOT NO_ZEROMQ_PLUGIN)
    add_custom_command(TARGET bricks-ut POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "$<TARGET_FILE_DIR:bricks-zeromq>" $<TARGET_FILE_DIR:bricks-ut>)
    target_include_directories(bricks-ut PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../bricks-zeromq/include)
    target_sources(bricks-ut PRIVATE kafka_test.cpp)
    target_link_libraries(bricks-ut PRIVATE bricks-zeromq)
endif()  

if (NOT NO_OATPP_PLUGIN)
    add_custom_command(TARGET bricks-ut POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "$<TARGET_FILE_DIR:bricks-oatpp>" $<TARGET_FILE_DIR:bricks-ut>)
    target_include_directories(bricks-ut PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../bricks-oatpp/include)
    target_sources(bricks-ut PRIVATE oatpp_test.cpp)
    target_link_libraries(bricks-ut PRIVATE bricks-oatpp)    
endif()  
 



