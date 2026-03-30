//
// Created by Candy on 2/16/26.
//

#include<thread>
#include "SatelliteSim.hpp"
#include "../Common/TelemetryFrame.hpp"

using boost::asio::ip::tcp;
namespace{
    std::mutex coutMutex;
}

SatelliteSim::SatelliteSim(std::string add, unsigned short int port_i)
    :io_context(),
     acceptor(io_context),
     address(std::move(add)),
     PORT(port_i)
{
    acceptor = create_tcp_acceptor();
}

void SatelliteSim::RunServer()
{
    //While to ensure the server can loop to create place for more clients
    while (true)
    {
        //Create a socket here//
        tcp::socket socket(io_context);

        if (clientCount == 0)
        {
            std::cout<<" Server is waiting to connect "<<std::endl;
        }
        else
        {
            std::cout<<" Server: waiting for a new client...ClientNum :  "<<clientCount<<std::endl;
        }

        //Make the acceptor wait till Client connects//
        acceptor.accept(socket);


        std::thread ClientThread(
            [this](tcp::socket s)
            {
                //call the client server
                HandleClient(s);
            },
            std::move(socket)
    );
ClientThread.detach();
        clientCount++;

        std::cout<<"Client Connected Successfully ! ... "<<std::endl;
    }

}

tcp::acceptor SatelliteSim::create_tcp_acceptor()
{
    tcp::endpoint endpoint(boost::asio::ip::make_address(address),PORT);

    //Create an acceptor to start connection request if sent //
    tcp::acceptor acceptor(io_context,endpoint);

    //Adding an Optional check below //
    tcp::endpoint local = acceptor.local_endpoint();
    std::cout<<"Server is listening on: "<<local.address().to_string()<< " : "<<local.port()<<std::endl;

    return acceptor;

}

void SatelliteSim::HandleClient(tcp::socket &socket)
{

    std::lock_guard<std::mutex> lock(coutMutex);
    std::cout<<"HandleClient in thread id"<<std::this_thread::get_id()<<std::endl;

    //Make an optional check here to see if the which client has connected from their address and port//
    tcp::endpoint remote = socket.remote_endpoint();
    std::cout<<"The cliented is connected from "<<remote.address().to_string()<<" : "<<remote.port()<<std::endl;

    while (true) {

        std::string Decoded = Frame.DecodeFrame(socket);

        if (Decoded == "")
        {
            std::cerr<<"The Decoded string was empty"<<std::endl;
            break;
        }

        auto frame = TelemetryFrame::FromJson(Decoded);

        if (!frame)
        {
            std::cerr<<"Frame was invalid";
            continue;
        }
    else
    {
        std::cout<<frame->sat_id<<"\n";
        std::cout<<frame->sequence<<"\n";
        std::cout<<frame->timestamp_ms<<"\n";
        std::cout<<frame->battery<<"\n";
        std::cout<<frame->temp_c<<"\n";
    }

    }

}
