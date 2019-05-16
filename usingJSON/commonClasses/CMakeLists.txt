cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 14) 
find_library(MQTT mosquitto)

file(GLOB SOURCES "*.cpp")

add_library(baseClasses OBJECT plcBase.cpp plcSocket.cpp timerBase.cpp)
