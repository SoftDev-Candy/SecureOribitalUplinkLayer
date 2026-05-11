//
// Created by Candy on 2/16/26.
//

#ifndef CHATSERVER_CHATCLIENT_H
#define CHATSERVER_CHATCLIENT_H

#include <boost/asio.hpp>
#include "../Common/TelemetryFrame.hpp"
#include "../Common/FrameCodec.hpp"

class TelemetryHub
{
private:

public:
    // Opens the client socket and kicks off the endless telemetry send loop.
    static int runClient();
    // Sends one frame for each simulated satellite, then waits for the normal ACK after every send.
    static void SendTelemetry (boost::asio::ip::tcp::socket& socket);



};


#endif //CHATSERVER_CHATCLIENT_H
