cmake_minimum_required(VERSION 3.10)

find_program(LSB_RELEASE_EXEC lsb_release)

execute_process(COMMAND ${LSB_RELEASE_EXEC} -is 
    OUTPUT_VARIABLE LSB_RELEASE_ID_SHORT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

message( ${LSB_RELEASE_ID_SHORT} )

set(CMAKE_CXX_STANDARD 14) 
add_executable(Server2 Server2.cpp utils.cpp interp.cpp incCounter.cpp)
target_compile_options(Server2 PUBLIC ${MYSQL_CFLAGS})
target_link_libraries(Server2 ${MYSQL_LIBS} -lrt -lpthread )
include_directories("${CMAKE_SOURCE_DIR}/commonClasses")
