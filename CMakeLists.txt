cmake_minimum_required(VERSION 3.0)

find_package( Boost 1.62 COMPONENTS context fiber REQUIRED )

set(CMAKE_BUILD_TYPE Debug)

#######################
# csci610
#######################

add_subdirectory(fmt)
#add_subdirectory(range-v3)

add_executable(csci610 csci610.cpp)

target_include_directories(csci610 PUBLIC
    range-v3/include fmt ${Boost_INCLUDE_DIRS})

target_link_libraries(csci610 fmt ${Boost_LIBRARIES})

#######################
# test
#######################

enable_testing()

add_executable(csci610_test 
    csci610_test.cpp)

target_include_directories(csci610_test PRIVATE
    range-v3/include fmt Catch/include ${Boost_INCLUDE_DIRS})

target_link_libraries(csci610_test fmt ${Boost_LIBRARIES})

add_test(csci610_test csci610_test)
