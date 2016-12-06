cmake_minimum_required(VERSION 3.0)
project (CSCI610)

find_package( Boost 1.62 COMPONENTS context REQUIRED )

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(fmt)
#add_subdirectory(range-v3)

set (CMAKE_CXX_STANDARD 14)

#######################
# csci610_base
#######################

add_library(csci610_base
    allocator.cpp
    environment.cpp
    instrument.cpp
    scheduler.cpp
    stack.o)

target_include_directories(csci610_base PUBLIC
    range-v3/include fmt ${Boost_INCLUDE_DIRS})

target_link_libraries(csci610_base LINK_PUBLIC fmt ${Boost_LIBRARIES})

add_custom_command(
    OUTPUT stack.o
    COMMAND ${CMAKE_SOURCE_DIR}/emit_llvm.sh
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    DEPENDS stack.cpp stack.hpp var.hpp allocator.hpp
    MAIN_DEPENDENCY 
    )

#######################
# csci610
#######################

add_executable(csci610
    csci610.cpp)

add_dependencies(csci610 csci610_base)

target_include_directories(csci610 PUBLIC
    range-v3/include fmt ${Boost_INCLUDE_DIRS})

target_link_libraries(csci610 csci610_base fmt ${Boost_LIBRARIES})

#######################
# test
#######################

enable_testing()

add_executable(csci610_test 
    csci610_test.cpp)

add_dependencies(csci610_test csci610_base)

target_include_directories(csci610_test PRIVATE
    range-v3/include fmt Catch/include ${Boost_INCLUDE_DIRS})

target_link_libraries(csci610_test csci610_base fmt ${Boost_LIBRARIES})

add_test(csci610_test csci610_test)
