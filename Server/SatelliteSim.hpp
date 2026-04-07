//
// Created by Candy on 2/16/26.
//

#ifndef CHATSERVER_CHATSERVER_H
#define CHATSERVER_CHATSERVER_H

#include<iostream>
#include<boost/asio.hpp>
#include<boost/thread.hpp>
#include<array>
#include<mutex>

#include "../Common/FrameCodec.hpp"
#include "../Common/TelemetryFrame.hpp"

namespace{
    using boost::asio::ip::tcp;
    using std::mutex;
}
class SatelliteSim
{
public:
    //Constructor
    SatelliteSim( std::string add , unsigned short int port_i );

    //Names says it all but responsible to run the server
    void RunServer();

    //Will receive telemetry data and parse thru it and print it out for now
    void ReceiveTelemetry(tcp::socket &socket) const;
    FrameCodec Frame;

private:
    tcp::acceptor create_tcp_acceptor();

    //Basically the I/O Engine for boost.
    boost::asio::io_context io_context;
    //boost acceptor
    tcp::acceptor acceptor;
    //Counts the amount of clients connected not required now but can be used to keep track of the satellite
     uint_fast64_t clientCount = 0 ;
    std::string address;
    unsigned short int PORT;

    //To Store the current state of the pushed data//
   static std::unordered_map<std::string ,std::optional<TelemetryFrame>>TelemetryStateMap ;



};


#endif //CHATSERVER_CHATSERVER_H