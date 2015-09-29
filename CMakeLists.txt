cmake_minimum_required(VERSION 2.8.11)

set(TARGET multiradio)
project(${TARGET})

set(CMAKE_AUTOMOC ON)

find_package(libusb-1.0 REQUIRED)
find_package(OpenCV REQUIRED)
find_package(CFlie REQUIRED)
find_package(Boost REQUIRED COMPONENTS system log thread regex date_time)

add_definitions(-DBOOST_LOG_DYN_LINK)

file(GLOB_RECURSE ${TARGET}_HEADERS src/*.h)
file(GLOB_RECURSE ${TARGET}_SOURCES src/*.cpp)

add_library(${TARGET}
	${${TARGET}_SOURCES}
	${${TARGET}_HEADERS})

target_include_directories(${TARGET} PUBLIC
	${Qt5Widgets_INCLUDE_DIRS} # required by ubuntu
	${Qt5SerialPort_INCLUDE_DIRS} # required by ubuntu
	${OpenCV_INCLUDE_DIRS} # required by xcode
	${LIBUSB_1_INCLUDE_DIRS}
	${CFlie_INCLUDE_DIR}
	src
	)

target_link_libraries(${TARGET}
	${Boost_LIBRARIES}
	${OpenCV_LIBS}
	${CFlie_LIBRARY}
	${LIBUSB_1_LIBRARIES}
	)
