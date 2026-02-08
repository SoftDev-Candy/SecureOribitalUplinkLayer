cmake_minimum_required(VERSION 3.10)
project(ChatServer)

set(CMAKE_CXX_STANDARD 20)
find_package(Boost REQUIRED)

add_executable(ChatServer src/main.cpp)
add_executable(Client src/Client.cpp)
target_include_directories(ChatServer PRIVATE ${Boost_INCLUDE_DIRS})

