cmake_minimum_required(VERSION 3.10)

find_program(LSB_RELEASE_EXEC lsb_release)

execute_process(COMMAND ${LSB_RELEASE_EXEC} -is
    OUTPUT_VARIABLE LSB_RELEASE_ID_SHORT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

message( ${LSB_RELEASE_ID_SHORT} )

set(CMAKE_CXX_STANDARD 14) 
find_library(MQTT mosquitto)

message( ${CMAKE_SOURCE_DIR}/commonClasses )

file(GLOB SOURCES "*.cpp")

include_directories("${CMAKE_SOURCE_DIR}/commonClasses")
SET (BASE_LIB_OUT_PATH "${CMAKE_SOURCE_DIR}/commonClasses")
link_directories( "${CMAKE_SOURCE_DIR}/commonClasses" )

add_executable(Server Server.cpp)
target_compile_options(Server PUBLIC ${MYSQL_CFLAGS})
target_link_libraries(Server ${MYSQL_LIBS} -lrt )

add_executable(dispatch dispatch.cpp ${MQTT})
target_link_libraries(dispatch ${MQTT} -lrt)

add_executable(plc plc.cpp )
target_link_libraries(plc aseClasses )

add_executable(lights lights.cpp )
target_link_libraries(lights aseClasses )

add_executable(pubSunRiset pubSunRiset.cpp)
target_link_libraries(pubSunRiset m ${MQTT} -lrt)

add_executable(syncMQTT syncMQTT.cpp)
target_compile_options(syncMQTT PUBLIC ${MYSQL_CFLAGS})
target_link_libraries(syncMQTT ${MQTT} ${MYSQL_LIBS} -lrt )

add_executable(tstSQL tstSQL.cpp)
target_compile_options(tstSQL PUBLIC ${MYSQL_CFLAGS})
target_link_libraries(tstSQL ${MYSQL_LIBS} -lrt )
