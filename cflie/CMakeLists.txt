cmake_minimum_required(VERSION 2.8.11)

# ################### Project ######################

set(PROJECT_NAME "cflie")
set(SOURCE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/src")

project(${PROJECT_NAME})

# ################### Dependencies ######################

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules/")
find_library(USB_LIB usb-1.0)

if(${USB_LIB} STREQUAL "USB_LIB-NOTFOUND")	
	find_package(libusb-1.0 REQUIRED)
	if(NOT LIBUSB_1_FOUND)
  		message(STATUS "The USB libs appear to not be installed.")
  		message(FATAL_ERROR "On Ubuntu, do: sudo apt-get install libusb-1.0-0 libusb-1.0-0-dev")
  	else()
  		set(USB_LIB "${LIBUSB_1_LIBRARIES}")
  	endif()
endif()

# ################### Library ######################

file(GLOB_RECURSE HEADERS "${SOURCE_DIRECTORY}/*.h")
file(GLOB_RECURSE SOURCES "${SOURCE_DIRECTORY}/*.cpp")

add_library(${PROJECT_NAME}
	${SOURCES}
	${HEADERS}
	)

# #################### Linking #####################

target_include_directories(${PROJECT_NAME} PUBLIC
  ${SOURCE_DIRECTORY})

target_link_libraries(${PROJECT_NAME} PUBLIC 
	${USB_LIB})

# ARM needs librt linked in
if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm")
  target_link_libraries(${PROJECT_NAME} rt)
endif()