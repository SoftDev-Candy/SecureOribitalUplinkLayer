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
    // These mirror the telemetry frame fields we care about keeping alive between sends.
    // sat_id tells us who we are pretending to be.
    // sequence keeps climbing so the backend sees a normal stream.
    // battery and temp_c are the actual changing health values.
    std::string sat_id{};
    uint64_t sequence{};
    float battery{};
    float temp_c{};
    // These two are the behavior sliders for the simple simulation.
    // drain_per_frame says how much battery disappears every tick.
    // drift_per_frame says how much the temperature moves every tick.
    float battery_drain_per_frame{};
    float temperature_drift_per_frame{};

public:
    // Builds one small satellite state machine with its own starting health values and drift behavior.
    SimulationState(const std::string& id, float bat, float temp, float batteryDrainPerFrame = 1.0f, float temperatureDriftPerFrame = -0.02f);

    // Generates the next telemetry frame after applying the simple battery/temperature behavior for this satellite.
    TelemetryFrame MakeNextFrame();

};

#endif //SOUL_SIMULATIONSTATE_HPP
