cmake_minimum_required (VERSION 2.8)

project(px_server)

if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Release")
endif ()

set(BASE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

set(LIBRARY_OUTPUT_PATH "${BASE_PATH}/lib")
set(EXECUTABLE_OUTPUT_PATH "${BASE_PATH}/lib")

message(STATUS "${CMAKE_GENERATOR}")

if (${CMAKE_GENERATOR} STREQUAL "Visual Studio 12 Win64" OR ${CMAKE_GENERATOR} STREQUAL "Visual Studio 12 2013 Win64")
    message (STATUS "Windows")
    
    set (CMAKE_CXX_FLAGS_DEBUG "/MT")
    set (CMAKE_CXX_FLAGS_RELEASE "/MT")

    # add_definitions(-DAMD64)

elseif (${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")
    message (STATUS "Linux")

    # add_definitions(-DAMD64 -DLINUX -DLINUX_X64 -DPLATFORM)
    
    set (CMAKE_C_FLAGS_DEBUG "-Wall -g -fshort-wchar -fwide-exec-charset=UTF-16LE -m64")
    set (CMAKE_C_FLAGS_RELEASE "-Wall -O2 -fshort-wchar -fwide-exec-charset=UTF-16LE -m64")
    
    set (CMAKE_CXX_FLAGS_DEBUG "-std=c++11 -Wall -g -fshort-wchar -fwide-exec-charset=UTF-16LE -m64")
    set (CMAKE_CXX_FLAGS_RELEASE "-std=c++11 -Wall -O2 -fshort-wchar -fwide-exec-charset=UTF-16LE -m64")
endif ()

set (SYS_LIB pthread dl rt)

include_directories(${BASE_PATH}/include ${BASE_PATH}/src ${BASE_PATH}/src/http-parser)

# file(GLOB SRC "src/.cpp" "src/px_request_impl.cpp" "src/px_response_impl.cpp" "src/px_server.cpp" "src/px_server_posix.cpp")
file(GLOB SRC "src/*.cpp" "src/http-parser/*.cpp")
list(FILTER SRC EXCLUDE REGEX "src/px_server_win32.cpp")

add_executable(px_server ${SRC})

target_link_libraries(px_server ${SYS_LIB})
