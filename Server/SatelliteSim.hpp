//
// Created by Candy on 2/16/26.
//

#ifndef CHATSERVER_CHATSERVER_H
#define CHATSERVER_CHATSERVER_H

#include<iostream>
#include<boost/asio.hpp>
#include<array>
#include<mutex>

#include "../Common/FrameCodec.hpp"

namespace{
    using boost::asio::ip::tcp;
    using std::mutex;
}
class SatelliteSim
{
public:
    SatelliteSim( std::string add , unsigned short int port_i );
    void RunServer();
    void HandleClient(tcp::socket &socket);
    FrameCodec Frame;

private:
    tcp::acceptor create_tcp_acceptor();

    //Basically the I/O Engine for boost.
    boost::asio::io_context io_context;
    tcp::acceptor acceptor;
     uint_fast64_t clientCount = 0 ;
    std::string address;
    unsigned short int PORT;

};


#endif //CHATSERVER_CHATSERVER_H