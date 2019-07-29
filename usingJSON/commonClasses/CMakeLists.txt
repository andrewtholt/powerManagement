cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 14) 
find_library(MQTT mosquitto)

# find_package(Boost)

file(GLOB SOURCES "*.cpp")

message( ${CMAKE_SOURCE_DIR}/commonClasses )

add_library(aseClasses SHARED alarmBase.cpp plcBase.cpp plcSocket.cpp timerBase.cpp timerBase.h toggle.cpp edge.cpp utils.cpp tQueue.cpp)

include_directories("${CMAKE_SOURCE_DIR}/commonClasses")
SET (BASE_LIB_OUT_PATH "${CMAKE_SOURCE_DIR}/commonClasses")
link_directories( "${CMAKE_SOURCE_DIR}/commonClasses" )

add_executable(tstQ tstQ.cpp )
target_link_libraries(tstQ aseClasses )

# add_executable(tstTimer tstTimer.cpp timerBase.cpp)
add_executable(tstTimer tstTimer.cpp )
target_link_libraries(tstTimer aseClasses )

# add_executable(tstPlc tstPlc.cpp plcBase.cpp plcSocket.cpp alarmBase.cpp)
add_executable(tstPlc tstPlc.cpp )
target_link_libraries(tstPlc aseClasses )

add_executable(tstAlarm tstAlarm.cpp )
target_link_libraries(tstAlarm aseClasses )

add_executable(tstToggle tstToggle.cpp )
target_link_libraries(tstToggle aseClasses )

add_executable(tstEdge tstEdge.cpp )
target_link_libraries(tstEdge aseClasses )

# add_library(myutils SHARED
#     utils.cpp
# )
# set_target_properties(myutils PROPERTIES SOVERSION 0.1 )
# 
# install(TARGETS myutils
#     LIBRARY DESTINATION "${CMAKE_SOURCE_DIR}/lib"
# )
# 
# 
