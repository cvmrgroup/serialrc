cmake_minimum_required(VERSION 2.8.11)

set(TARGET multiradio_test)
project(${TARGET})

find_package(Boost COMPONENTS program_options REQUIRED)

file(GLOB_RECURSE ${TARGET}_HEADERS src/*.h)
file(GLOB_RECURSE ${TARGET}_SOURCES src/*.cpp)

add_executable(${TARGET}
        ${${TARGET}_SOURCES}
        ${${TARGET}_HEADERS})

target_include_directories(${TARGET} PUBLIC
        inputdevices
        src
        ${Boost_INCLUDE_DIRS}
)

target_link_libraries(${TARGET}
        PUBLIC
        multiradio
        inputdevices
        ${Boost_LIBRARIES}
)

# add C++11 as compile option
target_compile_options(${TARGET} PUBLIC -std=c++11)
