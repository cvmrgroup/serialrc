cmake_minimum_required(VERSION 3.0.2)

project(serialrc-example)

find_package(SDL REQUIRED)
find_package(Boost COMPONENTS program_options filesystem REQUIRED)

file(GLOB_RECURSE ${PROJECT_NAME}_HEADERS src/*.h)
file(GLOB_RECURSE ${PROJECT_NAME}_SOURCES src/*.cpp)

add_executable(${PROJECT_NAME}
        ${${PROJECT_NAME}_SOURCES}
        ${${PROJECT_NAME}_HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC
        src
        ${SDL_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME}
        serialrc
        ${SDL_LIBRARY}
        ${Boost_LIBRARIES})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)