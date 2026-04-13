//
// Created by Candy on 2/27/26.
//

#ifndef SOUL_TELEMETRYFRAME_H
#define SOUL_TELEMETRYFRAME_H

#include <cstdint>
#include <optional>
#include <string>
#include <boost/json.hpp>

//Atomic Unit of state update basically a message system
struct TelemetryFrame
{
    std::string sat_id{};//Satellite id to show distinction between Satellite.

    //sequence helps detect application-level message loss that is lost frames or reordered frames or duplicates etc.
    uint64_t sequence{};

    //Message Generation time at the satellite you can use it to calculate delay , staleness , frame age etc.
    uint64_t timestamp_ms{};

    //System health basically to see if the system is dying or overheating or if something abnormal
    float battery{};

    //Same thing as the battery can be used to measure a lot of things
    float temp_c{};

    //Convert or serialize string To Json//
     std::string ToJson()const ;

    //From deserialize the returned string from ToJson() Function//
    //Its optional because some data may or may not have entry, and we want that to be safe
    static std::optional<TelemetryFrame> FromJson(const std::string& json);
};

#endif //SOUL_TELEMETRYFRAME_H
