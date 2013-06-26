cmake_minimum_required(VERSION 2.6)
project(cell)

set(CMAKE_AUTOMOC TRUE)

add_definitions(-DTHRUST_DEVICE_SYSTEM=THRUST_DEVICE_SYSTEM_OMP)


find_package(Qt4 REQUIRED QtGui)
include(${QT_USE_FILE})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thrust ${QT_INCLUDES})

find_package(OpenMP)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto -fwhole-program -std=c++11 ${OpenMP_CXX_FLAGS}") #normal
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto -fwhole-program -fprofile-generate -std=c++11 ${OpenMP_CXX_FLAGS}")  #generate
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto -fwhole-program -fprofile-use -std=c++11 ${OpenMP_CXX_FLAGS}")  #use profile info

add_executable(cell main.cpp cell.cpp graphicsview.cpp)
target_link_libraries(cell ${QT_LIBRARIES} /usr/lib/libllalloc.a)
