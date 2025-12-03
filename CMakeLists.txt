cmake_minimum_required(VERSION 4.0)

project(Client CXX)

set(CMAKE_CXX_STANDARD 23)

#Find the required SDL2 libraries
find_package(SDL2 REQUIRED)

#Include the SDL2 header files
include_directories(
        ${SDL2_INCLUDE_DIRS}
)

file(GLOB_RECURSE SOURCE_FILES "src/*.h" "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME}
        ${SDL2_LIBRARIES}
)

target_include_directories(${PROJECT_NAME}
        PRIVATE ${SDL2_INCLUDE_DIRS}
)