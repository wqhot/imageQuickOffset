cmake_minimum_required(VERSION 3.5)
project(zntk_offset_fb)

## Compile as C++11, supported in ROS Kinetic and newer
# add_compile_options(-std=c++11)

# set(CMAKE_BUILD_TYPE "RELEASE")
set(CMAKE_BUILD_TYPE "DEBUG")
if (CMAKE_BUILD_TYPE STREQUAL "RELEASE")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}-std=c++14 -O3 -pthread -fopenmp -s")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -pthread -fopenmp -s")
else(CMAKE_BUILD_TYPE STREQUAL "RELEASE")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}-std=c++14 -O0 -g -Wall -pthread")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g -Wall -pthread")
endif(CMAKE_BUILD_TYPE STREQUAL "RELEASE")

# set(CMAKE_BUILD_TYPE "DEBUG")
# set(CMAKE_CXX_FLAGS "-std=c++14 -O0 -Wall -g -ggdb")

find_package(OpenCV 3 REQUIRED)
# find_package(realsense2 REQUIRED)
find_package(Threads)



include_directories(
        ${PROJECT_SOURCE_DIR}/includes
        ${OpenCV_INCLUDE_DIRS}
)

add_library(thread_manage
        src/thread_manage.cpp)

add_library(imageprocess
        src/image_process.cpp)

add_library(write2fb
        src/cp_framebuffer.cpp)

add_executable(${PROJECT_NAME} src/zntk_offset_fb.cpp)

set(LINK_DIR /usr/local/lib/)

link_directories(${LINK_DIR})


target_link_libraries(${PROJECT_NAME}
  PRIVATE
  imageprocess
  thread_manage
  write2fb
  ${OpenCV_LIBS} 
  ${CMAKE_THREAD_LIBS_INIT}
  libjibianv2.so
)
 

