//
// Created by Candy on 2/27/26.
//

#ifndef SOUL_TELEMETRYFRAME_H
#define SOUL_TELEMETRYFRAME_H
#include <cstdint>


class TelemetryFrame
{
    public:
    //Constructor
    TelemetryFrame();

    //Destructor
    ~TelemetryFrame();

    private:
    std::string sat_id;       //Satellite id
    uint64_t sequence;       //Byte Sequence or Message Sequence
    uint64_t timestamp_ms;  //Message Delivery Time basically latency value
    float battery;          //Battery Percentage?
    float temp_c;           //Current temprature?





};




#endif //SOUL_TELEMETRYFRAME_H