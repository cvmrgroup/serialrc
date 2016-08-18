cmake_minimum_required(VERSION 2.8.11)

project(multiradio)

message("\n\n   " ${PROJECT_NAME} "\n##########################################")

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

find_package(OpenCV REQUIRED)
find_package(Qt5SerialPort REQUIRED)
find_package(Boost REQUIRED COMPONENTS system log thread regex date_time)

# ----------------------------------------------------

file(GLOB_RECURSE ${PROJECT_NAME}_HEADERS src/*.h)
file(GLOB_RECURSE ${PROJECT_NAME}_SOURCES src/*.cpp)

# ----------------------------------------------------

add_library(${PROJECT_NAME} STATIC
        ${${PROJECT_NAME}_SOURCES}
        ${${PROJECT_NAME}_HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC
        src
        ${OpenCV_INCLUDE_DIRS} # required by xcode
        ${Boost_INCLUDE_DIRS}
        ${Qt5SerialPort_INCLUDE_DIRS}        
)

target_link_libraries(${PROJECT_NAME} PUBLIC
        cflie
        utils
        Qt5::SerialPort
        ${OpenCV_LIBS}
        ${Boost_LIBRARIES}
)

# set the compile definition for the BOOST Log
target_compile_definitions(${PROJECT_NAME} PUBLIC "-DBOOST_LOG_DYN_LINK")

# add C++11 as compile option
target_compile_options(${PROJECT_NAME} PUBLIC -std=c++11)
