//
// Created by Candy on 4/9/26.
//

#include <chrono>
#include "SimulationState.hpp"

//Constructor for Simulation State Class
SimulationState::SimulationState(const std::string &id, float bat, float temp)
{
    sat_id = id;
    battery = bat;
    temp_c = temp;
    sequence = 0;
}

//Controls how to simulation works --Simple but effective for my use cases
TelemetryFrame SimulationState::MakeNextFrame()
{
    sequence++;
    battery -= 1.0f;
    temp_c -= 0.02f;

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
