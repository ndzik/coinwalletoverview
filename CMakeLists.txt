project (CursesWalletOverview)
cmake_minimum_required (VERSION 3.10)
set (CMAKE_CXX_STANDARD 17)
enable_testing()

# custom find sqlite
set (CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set (CMAKE_CXX_FLAGS "-g -O0 -Wall")

# for youcompleteme
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

### GLOBBING ###
file (GLOB_RECURSE sources src/*.cpp)
file (GLOB_RECURSE includes include/*.h)
file (GLOB_RECURSE tests test/*.cpp)

### TARGET DEFINITIONS ###
add_executable (cwo ${sources})

# compiler options
target_compile_options (cwo PUBLIC -Wall -ggdb)
target_include_directories (cwo PUBLIC include)

### DEPENDENCIES ###

# Add pthread
find_package(Threads)
target_link_libraries(cwo pthread)

# Add curses
set (CURSES_NEED_NCURSES TRUE)
find_package (Curses)
target_link_libraries (cwo ncurses)
#
## Add curl
find_package (CURL)
target_include_directories (cwo PUBLIC ${CURL_INCLUDE_DIRS})
target_link_libraries (cwo curl)
#
## Add sqlite3
find_package (SQLite3)
target_link_libraries (cwo sqlite3)
#
## Add boost
find_package (Boost 1.71 COMPONENTS system unit_test_framework REQUIRED)
target_include_directories (cwo PUBLIC ${Boost_INCLUDE_DIRS})
target_link_directories (cwo PUBLIC ${Boost_LIBRARY_DIRS})

target_link_libraries(cwo stdc++fs)

foreach(testsrc ${tests})
  file (GLOB_RECURSE lib_srcs src/*.cpp)
  list(FILTER lib_srcs EXCLUDE REGEX ".*main.cpp$")
  get_filename_component(testName ${testsrc} NAME_WE)
  add_executable(${testName} ${testsrc} ${lib_srcs})
  target_link_libraries(${testName}
    ${Boost_LIBRARIES}
    pthread
    ncurses
    curl
    sqlite3)
  target_include_directories(${testName} PUBLIC include)
  set_target_properties(${testName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_CURRENT_BINARY_DIR}/bintest)
  add_test(NAME ${testName}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/testBin
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/testBin/${testName}
    )
endforeach(testsrc)

###############################################################################
################## DEBUG-OUTPUT FOR CMAKE CONFIGURATION #######################
###############################################################################

macro(print_all_variables)
    message(STATUS
      "print_all_variables------------------------------------------{")
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
    message(STATUS
      "print_all_variables------------------------------------------}")
endmacro()

#print_all_variables ()
#-----------------------------------------------------------------------------#
