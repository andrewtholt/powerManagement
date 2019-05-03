cmake_minimum_required(VERSION 3.10)
project(PLC)

set(CMAKE_CXX_STANDARD 14) 

execute_process(COMMAND mysql_config --cflags
    OUTPUT_VARIABLE MYSQL_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND mysql_config --libs
    OUTPUT_VARIABLE MYSQL_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)

# target_compile_options(cgm PUBLIC ${MYSQL_CFLAGS})
# target_link_libraries(cgm ${MYSQL_LIBS})

add_subdirectory(src)
