cmake_minimum_required(VERSION 3.1)

project(tffm)

# set cmake variables
set(CMAKE_VERBOSE_MAKEFILE off)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter")

# set cmake variables for Qt5
set(CMAKE_AUTOMOC ON)

# find Qt5 libraries
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)

# specify the project files
add_executable(tffm src/tffm.cpp src/mainwindow.cpp src/filemanager.cpp)

# specify the libraries to be linked
target_link_libraries(tffm Qt5Core Qt5::Gui Qt5::Widgets)
