cmake_minimum_required(VERSION 2.8)

project(tcptoster)

#option(BUILD_SERVER "Build Server" ON)
#option(BUILD_CLIENT "Build Client" ON)

set(Boost_USE_MULTITHREADED ON)
find_package(Boost COMPONENTS thread system REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

include_directories(
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/src/server"
    "${CMAKE_SOURCE_DIR}/src/client")

#if (BUILD_SERVER)
    message(STATUS "BUILDING SERVER")
    add_executable(tosterserver
        src/server/server_core.h
        src/server/tcp_session.h
        src/server/tcp_session.cpp
        src/server/server_core.cpp
        src/common_tools.cpp
        src/server/server_main.cpp
        src/common_tools.h)

    target_link_libraries(tosterserver ${Boost_LIBRARIES})
#endif(BUILD_SERVER)


#if(BUILD_CLIENT)
    message(STATUS "BUILDING CLIENT")
    add_executable(tosterclient
        src/client/client_core.h
        src/client/client_manager.h
        src/client/client_manager.cpp
        src/client/client_session.h
        src/client/client_session.cpp
        src/client/client_core.cpp
        src/common_tools.cpp
        src/common_tools.h
        src/client/client_main.cpp)

    target_link_libraries(tosterclient ${Boost_LIBRARIES})
#endif(BUILD_CLIENT)
