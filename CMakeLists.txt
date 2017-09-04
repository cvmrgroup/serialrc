cmake_minimum_required(VERSION 3.0.2)

project(serialrc)

message("\n### " ${PROJECT_NAME})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules/")

# Build Example ################################################################

if (${BUILD_EXAMPLE})
    add_subdirectory(example)
endif ()

message(STATUS "WITH_TESTS: ${WITH_TESTS}")

set(${PROJECT_NAME}_INCLUDE_DIRS "")
set(${PROJECT_NAME}_LINK_LIBS "")
set(${PROJECT_NAME}_FLAGS "")

# Default Packages #############################################################

find_package(Boost REQUIRED COMPONENTS system log thread)

# Default Source Directory #####################################################

file(GLOB ${PROJECT_NAME}_HEADERS src/*.h)
file(GLOB ${PROJECT_NAME}_SOURCES src/*.cpp)

# Select Radio(s) ##############################################################

# arduino headers/sources
file(GLOB ARDUINO_HEADERS src/arduino/*.h)
file(GLOB ARDUINO_SOURCES src/arduino/*.cpp)
list(APPEND ${PROJECT_NAME}_HEADERS ${ARDUINO_HEADERS})
list(APPEND ${PROJECT_NAME}_SOURCES ${ARDUINO_SOURCES})
list(APPEND ${PROJECT_NAME}_FLAGS WITH_ARDUINO)

# icarus headers/sources
file(GLOB ICARUS_HEADERS src/icarus/*.h)
file(GLOB ICARUS_SOURCES src/icarus/*.cpp)
list(APPEND ${PROJECT_NAME}_HEADERS ${ICARUS_HEADERS})
list(APPEND ${PROJECT_NAME}_SOURCES ${ICARUS_SOURCES})

# Raspberry Pi GPIO bare DSMX transmitter module
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

message(STATUS "WITH_RASPBERRYPI: ${WITH_RASPBERRYPI}")

# ##############################################################################

add_library(${PROJECT_NAME} STATIC
        ${${PROJECT_NAME}_SOURCES}
        ${${PROJECT_NAME}_HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC
        src
        ${${PROJECT_NAME}_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PUBLIC
        ${${PROJECT_NAME}_LINK_LIBS}
        ${Boost_LIBRARIES})

# set the compile definition for the BOOST Log
target_compile_definitions(${PROJECT_NAME} PUBLIC "-DBOOST_LOG_DYN_LINK" ${${PROJECT_NAME}_FLAGS})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

# install ######################################################################

install(DIRECTORY src/
        DESTINATION "include/${PROJECT_NAME}"
        FILES_MATCHING PATTERN "*.h"
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
        ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        COMPONENT library)
