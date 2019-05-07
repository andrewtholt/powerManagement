cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 14) 
find_library(MQTT mosquitto)

file(GLOB SOURCES "*.cpp")

add_executable(Server Server.cpp)
target_compile_options(Server PUBLIC ${MYSQL_CFLAGS})
target_link_libraries(Server ${MYSQL_LIBS})

add_executable(dispatch dispatch.cpp ${MQTT})
target_link_libraries(dispatch ${MQTT} -lrt)
