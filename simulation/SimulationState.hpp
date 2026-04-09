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

public:
    SimulationState(const std::string& id,float bat,float temp);

 TelemetryFrame MakeNextFrame();

};

#endif //SOUL_SIMULATIONSTATE_HPP
