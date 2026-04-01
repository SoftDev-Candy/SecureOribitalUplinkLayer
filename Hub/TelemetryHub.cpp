//
// Created by Candy on 2/16/26.
//

#include "TelemetryHub.hpp"

#include <iostream>
using boost::asio::ip::tcp;

void TelemetryHub::SendTelemetry(boost::asio::ip::tcp::socket &socket)
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
        //TODO - Need to add a delay So something like Chrono thread delay would be nice but .... not the best way to do it//
        std::this_thread::sleep_for(std::chrono::seconds(2));

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


