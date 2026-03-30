//
// Created by Candy on 2/16/26.
//

#include "TelemetryHub.hpp"

#include <iostream>
using boost::asio::ip::tcp;

void TelemetryHub::HandleConnection(boost::asio::ip::tcp::socket &socket)
{

    while (true)
    {
        //TelemetryFrame Object
        TelemetryFrame tf;

        //FrameCodec Object
        FrameCodec Frame;

        //Telemetry Data
        tf.sat_id = "SAT_1";
        tf.sequence = 482;
        tf.timestamp_ms = 75;
        tf.battery =89.3;
        tf.temp_c = 44.6;

        //Convert To Json
        std::string TelemetryJSON = tf.ToJson();

        auto encoded = Frame.EncodeFrame(TelemetryJSON);
        boost::asio::write(socket, boost::asio::buffer(encoded));

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

        HandleConnection(socket);

        return 0; // success

    }catch(std::exception& e)
    {
        std::cout<<e.what()<<std::endl;
        return 1; //Error
    }

}


