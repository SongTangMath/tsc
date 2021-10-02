cmake_minimum_required( VERSION 3.15 )
project ( tsc )

set(tsc_test_sources ast/main.cc ast/parser.cc ast/semantics_analysis.cc ast/lex.yy.cc ast/tsc.tab.cc)
set(CMAKE_CXX_STANDARD 11)

FIND_PACKAGE(FLEX REQUIRED)
FIND_PACKAGE(BISON REQUIRED)
list(APPEND CXX_FLAGS "-g")

include_directories( ${CMAKE_SOURCE_DIR}/ast )
add_custom_command(OUTPUT ${CMAKE_SOURCE_DIR}/ast/lex.yy.cc ${CMAKE_SOURCE_DIR}/ast/tsc.tab.cc
        COMMAND 	flex --header-file=${CMAKE_SOURCE_DIR}/ast/lex.yy.h -o ${CMAKE_SOURCE_DIR}/ast/lex.yy.cc ${CMAKE_SOURCE_DIR}/ast/tsc.l
        COMMAND	    bison -d -o ${CMAKE_SOURCE_DIR}/ast/tsc.tab.cc ${CMAKE_SOURCE_DIR}/ast/tsc.y
        )


add_executable( test.out ${tsc_test_sources} )