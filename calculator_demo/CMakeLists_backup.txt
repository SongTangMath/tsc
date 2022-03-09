cmake_minimum_required( VERSION 3.15 )
project ( calculator_demo )

set(calculator_demo_sources a.cpp lex.yy.cpp tsc.tab.cpp)
set(CMAKE_CXX_STANDARD 11)

FIND_PACKAGE(FLEX REQUIRED)
FIND_PACKAGE(BISON REQUIRED)
list(APPEND CXX_FLAGS "-g")

include_directories( ${CMAKE_SOURCE_DIR} )
add_custom_command(OUTPUT ${CMAKE_SOURCE_DIR}/lex.yy.cpp ${CMAKE_SOURCE_DIR}/tsc.tab.cpp
                    COMMAND 	flex --header-file=${CMAKE_SOURCE_DIR}/lex.yy.h -o ${CMAKE_SOURCE_DIR}/lex.yy.cpp ${CMAKE_SOURCE_DIR}/tsc.l
                    COMMAND	    bison -d -o ${CMAKE_SOURCE_DIR}/tsc.tab.cpp ${CMAKE_SOURCE_DIR}/tsc.y
                   )


add_executable( a.out ${calculator_demo_sources} )