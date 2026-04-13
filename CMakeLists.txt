# IDK what the current standard version is for Cmake.
cmake_minimum_required(VERSION 3.10)

project(SOUL)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#For QT
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

#The old behaviour was removed so we use this now to find the BOOST== VROOM VROOM
find_package(Boost CONFIG REQUIRED COMPONENTS system filesystem)
find_package(Boost CONFIG REQUIRED COMPONENTS json system)

#find Packages for QT
find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)


add_library(sqlite3 STATIC
external/sqlite/sqlite3.c)

target_include_directories(sqlite3 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/external/sqlite
)

add_executable(SOUL ReciverServer/main.cpp
        ReciverServer/SatelliteSim.cpp
        ReciverServer/SatelliteSim.hpp
        Common/TelemetryFrame.hpp
        Common/TelemetryFrame.cpp
        Common/FrameCodec.cpp
        Common/FrameCodec.hpp
        Database/Database.cpp
        Database/Database.hpp
)

add_executable(Client SatelliteSender/main.cpp
        SatelliteSender/TelemetryHub.cpp
        SatelliteSender/TelemetryHub.hpp
        Common/TelemetryFrame.cpp
        Common/TelemetryFrame.hpp
        Common/FrameCodec.cpp
        Common/FrameCodec.hpp
        simulation/SimulationState.cpp
        simulation/SimulationState.hpp

)

add_executable(Test scratchTests/TestFrameCodec.cpp
Common/TelemetryFrame.cpp
Common/TelemetryFrame.hpp
Common/FrameCodec.cpp
Common/FrameCodec.hpp
        )
add_executable(UserInterface ui/main.cpp
        ui/mainwindow.cpp
        ui/mainwindow.hpp
        ui/mainwindow.ui
)

target_link_libraries(SOUL PRIVATE sqlite3  Boost::json Boost::system Boost::filesystem)
target_link_libraries(Client PRIVATE Boost::json Boost::system Boost::filesystem)
target_link_libraries(Test PRIVATE Boost::json Boost::system Boost::filesystem)
target_link_libraries(UserInterface PRIVATE Qt::Core Qt::Gui Qt::Widgets)






