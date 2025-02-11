cmake_minimum_required(VERSION 3.10)
project(Pathfinder)
add_executable(Pathfinder Pathfinder.cpp Grid.cpp)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# invoked and put all files that match the pattern in the variables
# find source files and header files
file(GLOB_RECURSE SRCS      *.cpp)
file(GLOB_RECURSE HDRS      *.h)

# compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb3 -O0 -Wall -lm -g")

# add executable
add_executable(Pathfinder ${SRCS} ${HDRS})

# find SDL2
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

# Link SDL2 and related libraries
target_include_directories(Pathfinder PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(Pathfinder ${SDL2_LIBRARIES} SDL2_image SDL2_ttf m)


# CMake command syntax: <function-name>(<arg1> [<arg2> ...])