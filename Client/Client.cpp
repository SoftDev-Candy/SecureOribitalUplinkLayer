//
// Created by Candy on 2/7/26.
//

#include<iostream>
#include<array>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

void HandleConnection (boost::asio::ip::tcp::socket& socket)
{
    while (true)
    {
        std::string message;
        std::cout<<"Type you're message : "<<std::endl;

        std::getline(std::cin,message);
        //Handling EOF as well(ctrl+D)
        if (!std::cin)
        {
        std::cout<<"Client : Input Stream closed , exiting " <<std::endl;
            break;
        }

        if (message == "exit")
        {
            std::cout<<"Exiting Client"<<std::endl;
            break;
        }

        //if message empty() skip
        if(message.empty())
        {
            std::cout<<"Message is empty"<<std::endl;
            continue;
        }

        boost::asio::write(socket, boost::asio::buffer(message));
        std::cout<<"The client sent the message"<<std::endl;

        //Creating buffer to read what the server echo's back//
        std::array<char , 1024>buffer{};
        //Creating a block and waiting for the server to send some bytes//
        std::size_t bytes_read = socket.read_some(boost::asio::buffer(buffer));

        if (bytes_read == 0) break;

        std::string ServerMessage(buffer.data(), bytes_read);


        std::cout << "The Server received and echoed the message: "<< ServerMessage << std::endl;
    }

}

int main()
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

    }catch(std::exception& e)
    {
        std::cout<<e.what()<<std::endl;

    }

    return 1;
}
