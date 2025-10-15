cmake_minimum_required(VERSION 4.0)

# the project name is Client
project(Client CXX)

set(CMAKE_CXX_STANDARD 20)

# let FindSDL2.cmake figure out where it is on Mac/Linux (though we're overriding with locals)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Find the standard SDL2 libraries installed on the system
find_package(SDL2 REQUIRED)


# include SDL header files
include_directories(${SDL2_INCLUDE_DIRS}
)

# load user source and header files
file(GLOB_RECURSE SOURCE_FILES "src/*.h" "src/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCE_FILES}
        main.cpp
)

target_link_libraries(${PROJECT_NAME}
        ${SDL2_LIBRARIES}
)

target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})




