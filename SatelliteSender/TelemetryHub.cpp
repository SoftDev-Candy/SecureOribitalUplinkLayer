//
// Created by Candy on 2/16/26.
//

#include "TelemetryHub.hpp"

#include <array>
#include <chrono>
#include <iostream>
#include <thread>

#include "../ReceiverServer/SatelliteSim.hpp"
#include "../simulation/SimulationState.hpp"
using boost::asio::ip::tcp;


void TelemetryHub::SendTelemetry(boost::asio::ip::tcp::socket &socket)
{
    // This little codec dude keeps the framed TCP protocol exactly the same as before.
    FrameCodec Frame;

    // Three satellites, same radio pipe, slightly different personalities.
    // SAT_1 is the chill one.
    // SAT_2 burns battery faster.
    // SAT_3 warms up slowly and sometimes goes radio-silent for a bit.
    std::array<SimulationState, 3> satellites = {
        SimulationState("SAT_1", 100.0f, 44.6f, SatelliteMode::Nominal, 0.10f, 0.00f),
        SimulationState("SAT_2", 100.0f, 42.5f, SatelliteMode::PowerDrain, 0.55f, 0.00f),
        SimulationState("SAT_3", 100.0f, 39.0f, SatelliteMode::SignalLoss, 0.18f, 0.06f)
    };

    while (true)
    {
        // One loop now means "every satellite gets a turn" instead of just SAT_1 hogging the spotlight.
        for (SimulationState& satellite : satellites)
        {
            // Ask the simulation for the next fake-but-useful telemetry frame.
            std::optional<TelemetryFrame> nextFrame = satellite.MakeNextFrame();

            // No frame here means "pretend the signal dropped", not "the whole client died".
            if (!nextFrame.has_value())
            {
                continue;
            }

            TelemetryFrame tf = *nextFrame;

            /*
            //Lets get the current point in time from clock//
            //Lets get the duration Epoch from its start
            auto duration  = std::chrono::system_clock::now().time_since_epoch();

            //Conversion to store it in time stamp
            uint64_t timestamp_val = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
            */

            // Turn the frame into JSON first because that is what the framing layer already expects.
            std::string TelemetryJSON = tf.ToJson();

            // Wrap the payload in the framed protocol and ship it out.
            auto encoded = Frame.EncodeFrame(TelemetryJSON);
            boost::asio::write(socket, boost::asio::buffer(encoded));

            // Same old handshake, just repeated for whichever frames actually got sent this round.
            auto decoded = Frame.DecodeFrame(socket);
            if (decoded == "")
            {
                std::cerr<<"Decoded Ack from the server is invalid";
            }
            else
            {
                std::cout<<"Ack Type "<<decoded<<std::endl;
            }
        }

        // Tiny pause so the sender does not become an absolute menace.
        //Simple Threading Underneath ...I should get into knitting ..//
        //I really don't think this is a good practice do want to see how pro's do this tho ...lets do steady clock
        //and sleep until to slow the entire thread down so we dont get spammed constantly//
        //First define the clock type to use
        using Clock = std::chrono::steady_clock;

        //Second set the current time
        Clock::time_point rightNow  = Clock::now();

        //Third Calculate the time to wake_up the thread  //
        Clock::time_point wakeupTime = rightNow + std::chrono::seconds(2);

        //Call std::this_thread::sleep_until() with the future time point
        std::this_thread::sleep_until(wakeupTime);


    }

}

 int TelemetryHub::runClient()
{
    try
    {
        // The client side is still intentionally boring: make socket, connect once, start sending forever.
        boost::asio::io_context io_context;
        unsigned short int PORT = 5000;

        tcp::endpoint endpoint(boost::asio::ip::make_address("127.0.0.1"),PORT);

        //Create a socket
        tcp::socket socket(io_context);

        socket.connect(endpoint);

        SendTelemetry(socket);

        return 0; // success

    }catch(std::exception& e)
    {
        std::cout<<e.what()<<std::endl;
        return 1; //Error
    }

}


