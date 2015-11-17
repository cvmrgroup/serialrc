cmake_minimum_required(VERSION 2.8.11)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules/")

#add_subdirectory(test)

set(TARGET multiradio)
project(${TARGET})

# USB Library
#find_library(USB_LIB usb-1.0)
find_package(libusb-1.0 REQUIRED)

find_package(OpenCV REQUIRED)
find_package(CFlie REQUIRED)
find_package(Boost REQUIRED COMPONENTS system log thread regex date_time)
find_package(Qt5SerialPort REQUIRED)

file(GLOB_RECURSE ${TARGET}_HEADERS src/*.h)
file(GLOB_RECURSE ${TARGET}_SOURCES src/*.cpp)

add_library(${TARGET} STATIC
        ${${TARGET}_SOURCES}
        ${${TARGET}_HEADERS})

target_include_directories(${TARGET} PUBLIC
        ${OpenCV_INCLUDE_DIRS} # required by xcode
        ${CFlie_INCLUDE_DIR}
        ${LIBUSB_1_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
        ${Qt5SerialPort_INCLUDE_DIRS}
        src
)

target_link_libraries(${TARGET} PUBLIC
        ${CFlie_LIBRARY}
        Qt5::SerialPort
        ${OpenCV_LIBS}
        ${Boost_LIBRARIES}
        utils
#${USB_LIB}
        ${LIBUSB_1_LIBRARIES} # needs to be included after CFlie
)

# set the compile definition for the BOOST Log
target_compile_definitions(${TARGET} PUBLIC "-DBOOST_LOG_DYN_LINK")

# add C++11 as compile option
target_compile_options(${TARGET} PUBLIC -std=c++11)
