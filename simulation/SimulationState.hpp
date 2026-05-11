//
// Created by Candy on 4/9/26.
//

#ifndef SOUL_SIMULATIONSTATE_HPP
#define SOUL_SIMULATIONSTATE_HPP
#include"../Common/TelemetryFrame.hpp"

//FIXME - Everything is public for now if time we hide more
class SimulationState
{
private:
    //Copied var from TelemetryFrame class
    std::string sat_id{};
    uint64_t sequence{};
    float battery{};
    float temp_c{};
    float battery_drain_per_frame{};
    float temperature_drift_per_frame{};

public:
    // Builds one small satellite state machine with its own starting health values and drift behavior.
    SimulationState(const std::string& id, float bat, float temp, float batteryDrainPerFrame = 1.0f, float temperatureDriftPerFrame = -0.02f);

    // Generates the next telemetry frame after applying the simple battery/temperature behavior for this satellite.
    TelemetryFrame MakeNextFrame();

};

#endif //SOUL_SIMULATIONSTATE_HPP
