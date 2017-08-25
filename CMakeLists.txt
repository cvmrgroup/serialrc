cmake_minimum_required(VERSION 2.8.11)

project(multiradio)

message("\n\n   " ${PROJECT_NAME} "\n##########################################")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules/")

# Build Tests ##################################################################

if(DEFINED WITH_TESTS)
    add_subdirectory(test)
endif()

message(STATUS "WITH_TESTS: ${WITH_TESTS}")

set(${PROJECT_NAME}_INCLUDE_DIRS "")
set(${PROJECT_NAME}_LINK_LIBS "")
set(${PROJECT_NAME}_FLAGS "")

# Default Packages #############################################################

find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED COMPONENTS system log thread regex date_time)

# Default Source Directory #####################################################

file(GLOB ${PROJECT_NAME}_HEADERS src/*.h)
file(GLOB ${PROJECT_NAME}_SOURCES src/*.cpp)

# Select Radio(s) ##############################################################

if (${WITH_ARDUINO})
    file(GLOB ARDUINO_HEADERS src/arduino/*.h)
    file(GLOB ARDUINO_SOURCES src/arduino/*.cpp)
    list(APPEND ${PROJECT_NAME}_HEADERS ${ARDUINO_HEADERS})
    list(APPEND ${PROJECT_NAME}_SOURCES ${ARDUINO_SOURCES})
    list(APPEND ${PROJECT_NAME}_FLAGS WITH_ARDUINO)
endif ()

if (${WITH_RASPBERRYPI})
    find_package(WiringPi REQUIRED)
    file(GLOB RASPBERRYPI_HEADERS src/raspberrypi/*.h)
    file(GLOB RASPBERRYPI_SOURCES src/raspberrypi/*.cpp)
    list(APPEND ${PROJECT_NAME}_HEADERS ${RASPBERRYPI_HEADERS})
    list(APPEND ${PROJECT_NAME}_SOURCES ${RASPBERRYPI_SOURCES})
    list(APPEND ${PROJECT_NAME}_INCLUDE_DIRS ${WIRINGPI_INCLUDE_DIRS})
    list(APPEND ${PROJECT_NAME}_LINK_LIBS ${WIRINGPI_LIBRARIES})
    list(APPEND ${PROJECT_NAME}_FLAGS WITH_RASPBERRYPI)
endif ()

if (${WITH_CRAZYRADIO})
    add_subdirectory(ext/crazyflie_ros/crazyflie_cpp)
    file(GLOB CRAZYRADIO_HEADERS src/crazyradio/*.h)
    file(GLOB CRAZYRADIO_SOURCES src/crazyradio/*.cpp)
    list(APPEND ${PROJECT_NAME}_HEADERS ${CRAZYRADIO_HEADERS})
    list(APPEND ${PROJECT_NAME}_SOURCES ${CRAZYRADIO_SOURCES})
    list(APPEND ${PROJECT_NAME}_INCLUDE_DIRS ext/crazyflie_ros/crazyflie_cpp/include)
    list(APPEND ${PROJECT_NAME}_LINK_LIBS crazyflie_cpp)
    list(APPEND ${PROJECT_NAME}_FLAGS WITH_CRAZYRADIO)
endif ()

message(STATUS "WITH_ARDUINO: ${WITH_ARDUINO}")
message(STATUS "WITH_RASPBERRYPI: ${WITH_RASPBERRYPI}")
message(STATUS "WITH_CRAZYRADIO: ${WITH_CRAZYRADIO}")

# ##############################################################################

add_library(${PROJECT_NAME} STATIC
    ${${PROJECT_NAME}_SOURCES}
    ${${PROJECT_NAME}_HEADERS})

message(STATUS "multiradio include dirs: " ${${PROJECT_NAME}_INCLUDE_DIRS})

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
target_compile_definitions(${PROJECT_NAME} PUBLIC "-DBOOST_LOG_DYN_LINK" ${${PROJECT_NAME}_FLAGS})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
