cmake_minimum_required(VERSION 2.8.11)

# ----------------------------------------------------

# check if WITH_TESTS is defined
if(DEFINED WITH_TESTS)
    # add the test subdirectory for multiradio
    add_subdirectory(test)
endif()
# print the WITH_TESTS
message(STATUS "WITH_TESTS: ${WITH_TESTS}")

# ----------------------------------------------------

# add the cflie library
add_subdirectory(cflie)

# ----------------------------------------------------

set(TARGET multiradio)
project(${TARGET})

# ----------------------------------------------------

find_package(OpenCV REQUIRED)
find_package(Qt5SerialPort REQUIRED)
find_package(Boost REQUIRED COMPONENTS system log thread regex date_time)

# ----------------------------------------------------

file(GLOB_RECURSE ${TARGET}_HEADERS src/*.h)
file(GLOB_RECURSE ${TARGET}_SOURCES src/*.cpp)

# ----------------------------------------------------

add_library(${TARGET} STATIC
        ${${TARGET}_SOURCES}
        ${${TARGET}_HEADERS})

target_include_directories(${TARGET} PUBLIC
        src
        ${OpenCV_INCLUDE_DIRS} # required by xcode
        ${Boost_INCLUDE_DIRS}
        ${Qt5SerialPort_INCLUDE_DIRS}        
)

target_link_libraries(${TARGET} PUBLIC
        cflie
        utils
        Qt5::SerialPort
        ${OpenCV_LIBS}
        ${Boost_LIBRARIES}
)

# set the compile definition for the BOOST Log
target_compile_definitions(${TARGET} PUBLIC "-DBOOST_LOG_DYN_LINK")

# add C++11 as compile option
target_compile_options(${TARGET} PUBLIC -std=c++11)
