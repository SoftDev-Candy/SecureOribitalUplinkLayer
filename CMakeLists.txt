cmake_minimum_required(VERSION 3.10)
project(ChatServer)

set(CMAKE_CXX_STANDARD 20)
find_package(Boost REQUIRED)

add_executable(ChatServer Server/Server.cpp
        Server/ChatServer.cpp
        Server/ChatServer.h)

add_executable(Client Client/Client.cpp
        Client/ChatClient.cpp
        Client/ChatClient.h)

target_include_directories(ChatServer PRIVATE ${Boost_INCLUDE_DIRS})

