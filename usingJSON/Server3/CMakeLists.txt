cmake_minimum_required(VERSION 3.10)

find_program(LSB_RELEASE_EXEC lsb_release)

execute_process(COMMAND mysql_config --cflags
    OUTPUT_VARIABLE MYSQL_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND mysql_config --libs
    OUTPUT_VARIABLE MYSQL_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)


execute_process(COMMAND ${LSB_RELEASE_EXEC} -is 
    OUTPUT_VARIABLE LSB_RELEASE_ID_SHORT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

message( ${LSB_RELEASE_ID_SHORT} )

set(CMAKE_CXX_STANDARD 14) 
# add_executable(Server2 Server2.cpp utils.cpp interp.cpp incCounter.cpp)
add_executable(Server3 Server3.cpp )
# add_executable(interpTest interpTest.cpp utils.cpp interp.cpp incCounter.cpp)
target_compile_options(Server3 PUBLIC ${MYSQL_CFLAGS})

target_link_libraries(Server3 ${MYSQL_LIBS} -lrt -lpthread )
# target_link_libraries(interpTest ${MYSQL_LIBS} -lrt -lpthread )

# include_directories("${CMAKE_SOURCE_DIR}/commonClasses")
