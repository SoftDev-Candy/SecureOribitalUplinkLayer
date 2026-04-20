//
// Created by Candy on 2/16/26.
//

#include "TelemetryHub.hpp"

#include <chrono>
#include <iostream>
#include <thread>

#include "../ReceiverServer/SatelliteSim.hpp"
#include "../simulation/SimulationState.hpp"
using boost::asio::ip::tcp;


void TelemetryHub::SendTelemetry(boost::asio::ip::tcp::socket &socket)
{
    //FrameCodec Object
    FrameCodec Frame;

    SimulationState Simstate("SAT_1" , 100.0f , 44.6f);

    while (true)
    {
        //TelemetryFrame Object
        TelemetryFrame tf;
        tf = Simstate.MakeNextFrame();

        /*
        //Lets get the current point in time from clock//
        //Lets get the duration Epoch from its start
        auto duration  = std::chrono::system_clock::now().time_since_epoch();

        //Conversion to store it in time stamp
        uint64_t timestamp_val = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
        */

        //Convert To Json
        std::string TelemetryJSON = tf.ToJson();

        //Encoding the Telemetry and writing it down in the socket//
        auto encoded = Frame.EncodeFrame(TelemetryJSON);
        boost::asio::write(socket, boost::asio::buffer(encoded));

        //Decode the incoming string from the server to get ack packet.//
        auto decoded = Frame.DecodeFrame(socket);
        if (decoded== "")
        {
            std::cerr<<"Decoded Ack from the server is invalid";
        }else
        {
            std::cout<<"Ack Type "<<decoded<<std::endl;
        }

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


