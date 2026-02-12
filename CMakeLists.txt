cmake_minimum_required(VERSION 3.10)
project(ChatServer)

set(CMAKE_CXX_STANDARD 20)
find_package(Boost REQUIRED)

add_executable(ChatServer Server/Server.cpp)
add_executable(Client Client/Client.cpp)
target_include_directories(ChatServer PRIVATE ${Boost_INCLUDE_DIRS})

