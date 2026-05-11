//
// Created by Candy on 4/9/26.
//

#include <chrono>
#include <algorithm>
#include "SimulationState.hpp"

//Constructor for Simulation State Class
SimulationState::SimulationState(const std::string &id, float bat, float temp, float batteryDrainPerFrame, float temperatureDriftPerFrame)
{
    sat_id = id;
    battery = bat;
    temp_c = temp;
    sequence = 0;
    battery_drain_per_frame = batteryDrainPerFrame;
    temperature_drift_per_frame = temperatureDriftPerFrame;
}

//Controls how to simulation works --Simple but effective for my use cases
TelemetryFrame SimulationState::MakeNextFrame()
{
    sequence++;
    // Tiny drama engine: each satellite burns battery and shifts temperature at its own pace.
    battery = std::max(0.0f, battery - battery_drain_per_frame);
    temp_c += temperature_drift_per_frame;

    //FIXME - Technically need to have something like a public function I called this particular logic 3 times now lol
    auto duration  = std::chrono::system_clock::now().time_since_epoch();
    //Conversion to store it in time stamp
    uint64_t timestamp_val = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

    TelemetryFrame tf;
    tf.sat_id = sat_id;
    tf.sequence = sequence;
    tf.battery = battery;
    tf.temp_c = temp_c;
    tf.timestamp_ms = timestamp_val;

return tf;
}
