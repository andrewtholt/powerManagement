cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 14) 
find_library(MQTT mosquitto)

message( ${CMAKE_SOURCE_DIR}/commonClasses )

file(GLOB SOURCES "*.cpp")

include_directories("${CMAKE_SOURCE_DIR}/commonClasses")
SET (BASE_LIB_OUT_PATH "${CMAKE_SOURCE_DIR}/commonClasses")
link_directories( "${CMAKE_SOURCE_DIR}/commonClasses" )

add_executable(Server Server.cpp)
target_compile_options(Server PUBLIC ${MYSQL_CFLAGS})
target_link_libraries(Server ${MYSQL_LIBS})

add_executable(dispatch dispatch.cpp ${MQTT})
target_link_libraries(dispatch ${MQTT} -lrt)

add_executable(plc plc.cpp )
target_link_libraries(plc aseClasses )

