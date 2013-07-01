cmake_minimum_required(VERSION 2.6)
project(cell)

set(CMAKE_AUTOMOC TRUE)

include(CheckCCompilerFlag)
check_c_compiler_flag(-std=c++11 HAS_CXX11 REQUIRED)

if (HAS_CXX11)
    message(STATUS "compiler features full C++11")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif (HAS_CXX11)

add_definitions(-DTHRUST_DEVICE_SYSTEM=THRUST_DEVICE_SYSTEM_OMP)

find_package(Qt4 REQUIRED QtGui)
include(${QT_USE_FILE})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thrust ${QT_INCLUDES})

find_package(OpenMP)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

set(SOURCES main.cpp cell.cpp graphicsview.cpp parallel_radix_sort.h)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    set (SOURCES ${SOURCES} lockless_allocator/ll_alloc.c)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mtune=native -march=native -flto -fwhole-program") #normal
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto -fwhole-program -fprofile-generate)  #generate
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto -fwhole-program -fprofile-use)  #use profile info
else ()
    message(WARNING "do not forget to LD_PRELOAD libllalloc.so.1.3")
endif ()
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ipo -xHost -no-prec-div -no-prec-sqrt")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ipo -xHost -prof-gen")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ipo -xHost -prof-use")
endif()
add_executable(cell ${SOURCES})
target_link_libraries(cell ${QT_LIBRARIES})
