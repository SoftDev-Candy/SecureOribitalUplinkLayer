//
// Created by Candy on 3/18/26.
//

//FIXME -- File here only for me to test the code nothing else nothing more //

#include <iostream>
#include "../Common/FrameCodec.hpp"
#include "../Common/TelemetryFrame.hpp"

int main()
{
    TelemetryFrame tf;
    tf.sat_id = "SAT_1";
    tf.sequence = 482094824;
    tf.timestamp_ms = 749284738947;
    tf.battery =89.3;
    tf.temp_c = 44.6;

    std::string tojsontest1= tf.ToJson();
    std::printf("%s\n", tojsontest1.c_str());

    tf.FromJson(tojsontest1);
    std::printf("%s\n", tojsontest1.c_str());

    FrameCodec frame;
    frame.EncodeFrame(tojsontest1);
    std::printf("%s\n", tojsontest1.c_str());

    return 0;
}
