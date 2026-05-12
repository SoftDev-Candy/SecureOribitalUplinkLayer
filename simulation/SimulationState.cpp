//
// Created by Candy on 4/9/26.
//

#include <chrono>
#include <algorithm>
#include "SimulationState.hpp"

//Constructor for Simulation State Class
SimulationState::SimulationState(
    const std::string &id,
    float bat,
    float temp,
    SatelliteMode satMode,
    float batteryDrainPerFrame,
    float temperatureDriftPerFrame)
{
    // This constructor is satellite personality loader each one with some other disorder in some terms you could them special
    sat_id = id;
    battery = bat;
    temp_c = temp;
    sequence = 0;
    mode = satMode;
    battery_drain_per_frame = batteryDrainPerFrame;
    temperature_drift_per_frame = temperatureDriftPerFrame;
}

//Controls how to simulation works --Simple but effective for my use cases
std::optional<TelemetryFrame> SimulationState::MakeNextFrame()
{
    tick_counter++;

    // New loop, new packet, so bump the sequence first.
    sequence++;

    // These are the actual values we will use for this specific tick after the mode tweaks them.
    float batteryDropThisTick = battery_drain_per_frame;
    float tempMoveThisTick = temperature_drift_per_frame;

    // Mode is the "what kind of bad day is this satellite having" switch.
    if (mode == SatelliteMode::PowerDrain)
    {
        batteryDropThisTick += 0.80f;
    }
    else if (mode == SatelliteMode::Overheating)
    {
        tempMoveThisTick += 0.10f;
    }
    else if (mode == SatelliteMode::SignalLoss)
    {
        // SAT_3 still keeps heating while the radio acts flaky.
        tempMoveThisTick += 0.04f;

        // Every so often, pretend the radio goes quiet for a couple of frames.
        if (signal_loss_ticks_left <= 0 && tick_counter % 8 == 0)
        {
            signal_loss_ticks_left = 2;
        }
    }

    // this here is a an engine it goes chooooo chooo: each satellite burns battery and shifts temperature at its own pace.
    battery = std::max(0.0f, battery - batteryDropThisTick);
    temp_c += tempMoveThisTick;

    // Grab "now" in milliseconds so the receiver can still compute age and latency like before.
    //FIXME - Technically need to have something like a public function I called this particular logic 3 times now lol
    auto duration  = std::chrono::system_clock::now().time_since_epoch();
    //Conversion to store it in time stamp
    uint64_t timestamp_val = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

    if (mode == SatelliteMode::SignalLoss && signal_loss_ticks_left > 0)
    {
        signal_loss_ticks_left--;
        return std::nullopt;
    }

    // Stuff all the updated values into the outgoing telemetry frame.---yes tf ... THE FLIPPER or WHAT DAAAA FAAAAA
    TelemetryFrame tf;
    tf.sat_id = sat_id;
    tf.sequence = sequence;
    tf.battery = battery;
    tf.temp_c = temp_c;
    tf.timestamp_ms = timestamp_val;

    return tf;
}
