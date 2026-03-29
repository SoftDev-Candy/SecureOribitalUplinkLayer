# IDK what the current standard version is for Cmake.
cmake_minimum_required(VERSION 3.10)

project(SOUL)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#The old behaviour was removed so we use this now to find the BOOST== VROOM VROOM
find_package(Boost CONFIG REQUIRED COMPONENTS system filesystem)
find_package(Boost CONFIG REQUIRED COMPONENTS json system)


add_executable(SOUL Server/main.cpp
        Server/SatelliteSim.cpp
        Server/SatelliteSim.hpp
        Common/TelemetryFrame.hpp
        Common/TelemetryFrame.cpp
        Common/FrameCodec.cpp
        Common/FrameCodec.hpp
)

add_executable(Client Hub/main.cpp
        Hub/TelemetryHub.cpp
        Hub/TelemetryHub.hpp
        Common/TelemetryFrame.cpp
        Common/TelemetryFrame.hpp
        Common/FrameCodec.cpp
        Common/FrameCodec.hpp
        )

add_executable(Test scratchTests/TestFrameCodec.cpp
Common/TelemetryFrame.cpp
Common/TelemetryFrame.hpp
Common/FrameCodec.cpp
Common/FrameCodec.hpp
        )


target_link_libraries(SOUL PRIVATE Boost::json Boost::boost)
target_link_libraries(Client PRIVATE Boost::json Boost::boost)
target_link_libraries(Test PRIVATE Boost::json Boost::boost)
