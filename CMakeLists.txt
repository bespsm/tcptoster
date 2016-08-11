cmake_minimum_required(VERSION 2.8)

project(tcptoster)

set(Boost_USE_MULTITHREADED ON)
find_package(Boost COMPONENTS thread system REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

include_directories(
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/src/server"
    "${CMAKE_SOURCE_DIR}/src/client")


SET(BUILD_SHARED_LIBS TRUE)

add_library(comm_tools
    src/common_tools.cpp)

message(STATUS "BUILDING SERVER")

add_executable(tosterserver
    src/server/tcp_session.cpp
    src/server/server_core.cpp
    src/server/server_main.cpp
)

target_link_libraries(tosterserver ${Boost_LIBRARIES} comm_tools)

message(STATUS "BUILDING CLIENT")

add_executable(tosterclient
    src/client/client_manager.cpp
    src/client/client_session.cpp
    src/client/client_core.cpp
    src/client/client_main.cpp
)

target_link_libraries(tosterclient ${Boost_LIBRARIES} comm_tools)
