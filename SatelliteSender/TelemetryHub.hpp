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
    static int runClient();
    static void SendTelemetry (boost::asio::ip::tcp::socket& socket);



};


#endif //CHATSERVER_CHATCLIENT_H