cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 14) 
find_library(MQTT mosquitto)

file(GLOB SOURCES "*.cpp")

message( ${CMAKE_SOURCE_DIR}/commonClasses )

add_library(baseClasses OBJECT plcBase.cpp plcSocket.cpp timerBase.cpp timerBase.h )

include_directories("${CMAKE_SOURCE_DIR}/commonClasses")
add_executable(tstTimer tstTimer.cpp timerBase.cpp)
add_executable(tstPLC tstPLC.cpp plcBase.cpp plcSocket.cpp)

