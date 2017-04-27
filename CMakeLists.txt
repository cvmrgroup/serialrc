cmake_minimum_required(VERSION 2.8.11)

project(radio)

message("\n\n   " ${PROJECT_NAME} "\n##########################################")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules/")

# ----------------------------------------------------

# check if WITH_TESTS is defined
if(DEFINED WITH_TESTS)
    # add the test subdirectory for multiradio
    add_subdirectory(test)
endif()
# print the WITH_TESTS
message(STATUS "WITH_TESTS: ${WITH_TESTS}")

set(${PROJECT_NAME}_INCLUDE_DIRS "")
set(${PROJECT_NAME}_LINK_LIBS "")
set(${PROJECT_NAME}_FLAGS "")

# optional radios ####################################

if (${WITH_ARDUINO})
    file(GLOB ${PROJECT_NAME}_HEADERS src/radios/arduino/*.h)
    file(GLOB ${PROJECT_NAME}_SOURCES src/radios/arduino/*.cpp)
    list(APPEND ${PROJECT_NAME}_INCLUDE_DIRS ${Qt5SerialPort_INCLUDE_DIRS})
    list(APPEND ${PROJECT_NAME}_LINK_LIBS Qt5::SerialPort)
    list(APPEND ${PROJECT_NAME}_FLAGS WITH_ARDUINO)
endif ()

if (${WITH_RASPBERRYPI})
    find_package(WiringPi REQUIRED)
    file(GLOB ${PROJECT_NAME}_HEADERS src/radios/raspberrypi/*.h)
    file(GLOB ${PROJECT_NAME}_SOURCES src/radios/raspberrypi/*.cpp)
    list(APPEND ${PROJECT_NAME}_INCLUDE_DIRS ${WIRINGPI_INCLUDE_DIRS})
    list(APPEND ${PROJECT_NAME}_LINK_LIBS ${WIRINGPI_LIBRARIES})
    list(APPEND ${PROJECT_NAME}_FLAGS WITH_RASPBERRYPI)
endif ()

if (${WITH_CRAZYRADIO})
    add_subdirectory(ext/libcflie)
    find_package(Qt5SerialPort REQUIRED)
    file(GLOB ${PROJECT_NAME}_HEADERS src/*.h)
    file(GLOB ${PROJECT_NAME}_SOURCES src/*.cpp)
    list(APPEND ${PROJECT_NAME}_INCLUDE_DIRS cflie)
    list(APPEND ${PROJECT_NAME}_LINK_LIBS cflie)
    list(APPEND ${PROJECT_NAME}_FLAGS WITH_CRAZYRADIO)
endif ()

message(STATUS "Arduino: ${WITH_ARDUINO}")
message(STATUS "RaspberryPi: ${WITH_RASPBERRYPI}")
message(STATUS "Crazyradio: ${WITH_CRAZYRADIO}")

# ----------------------------------------------------

find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED COMPONENTS system log thread regex date_time)

# ----------------------------------------------------

file(GLOB ${PROJECT_NAME}_HEADERS src/*.h)
file(GLOB ${PROJECT_NAME}_SOURCES src/*.cpp)

# ----------------------------------------------------

add_library(${PROJECT_NAME} STATIC
    ${${PROJECT_NAME}_SOURCES}
    ${${PROJECT_NAME}_HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC
    src
    ${${PROJECT_NAME}_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS} # required by xcode
    ${Boost_INCLUDE_DIRS}
    )

target_link_libraries(${PROJECT_NAME} PUBLIC
    utils
    ${${PROJECT_NAME}_LINK_LIBS}
    ${OpenCV_LIBS}
    ${Boost_LIBRARIES}
    )

# set the compile definition for the BOOST Log
target_compile_definitions(${PROJECT_NAME} PUBLIC "-DBOOST_LOG_DYN_LINK" ${PROJECT_NAME}_FLAGS)

# add C++11 as compile option
target_compile_options(${PROJECT_NAME} PUBLIC -std=c++11)
