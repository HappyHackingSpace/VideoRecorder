cmake_minimum_required(VERSION 3.5)

project(VideoRecorder)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)

#add_executable(VideoRecorder main.cpp)
add_executable(VideoRecorder video_recorder.cpp)
target_include_directories(VideoRecorder PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(VideoRecorder Qt6::Widgets ${OpenCV_LIBS})
