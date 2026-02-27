# IDK what the current standard version is for Cmake.
cmake_minimum_required(VERSION 3.10)

project(SOUL)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#The old behaviour was removed so we use this now to find the BOOST== VROOM VROOM
find_package(Boost CONFIG REQUIRED COMPONENTS system filesystem)

add_executable(SOUL Server/Server.cpp
        Server/ChatServer.cpp
        Server/ChatServer.h)

add_executable(Client Client/Client.cpp
        Client/ChatClient.cpp
        Client/ChatClient.h)


target_link_libraries(SOUL PRIVATE Boost::boost)
target_link_libraries(Client PRIVATE Boost::boost)