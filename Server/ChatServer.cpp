//
// Created by Candy on 2/16/26.
//

#include<thread>
#include "ChatServer.h"

using boost::asio::ip::tcp;

ChatServer::ChatServer(std::string add, unsigned short int port_i)
    :io_context(),
     acceptor(io_context),
     address(std::move(add)),
     PORT(port_i)
{
    acceptor = create_tcp_acceptor();
}

void ChatServer::RunServer()
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

        //TODO - WRAP THIS INSIDE A THREAD

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

tcp::acceptor ChatServer::create_tcp_acceptor()
{
    tcp::endpoint endpoint(boost::asio::ip::make_address(address),PORT);

    //Create an acceptor to start connection request if sent //
    tcp::acceptor acceptor(io_context,endpoint);

    //Adding an Optional check below //
    tcp::endpoint local = acceptor.local_endpoint();
    std::cout<<"Server is listening on: "<<local.address().to_string()<< " : "<<local.port()<<std::endl;

    return acceptor;

}

void ChatServer::HandleClient(tcp::socket &socket)
{
    std::cout<<"HandleClient in thread id"<<std::this_thread::get_id()<<std::endl;

    //Make an optional check here to see if the which client has connected from their address and port//
    tcp::endpoint remote = socket.remote_endpoint();
    std::cout<<"The cliented is connected from "<<remote.address().to_string()<<" : "<<remote.port()<<std::endl;

    while (true) {
        //Let's Read some data from the client make a buffer to store data//
        std::array<char , 1024>buffer{};

        //Adding error code here because of EOF so we overload instead of throw.
        boost::system::error_code error;

        //Create a block to wait for some bytes to arrive if they don't connection will close//
        std::size_t bytes_read = socket.read_some(boost::asio::buffer(buffer) , error);

        if (error == boost::asio::error::eof)
        {
        std::cout<<"Server: Client Disconnected cleanly(EOF)"<<std::endl;
        break;//Normal Disconnection Expected
        }

        if (error)
        {
            std::cout<<"Server : Error reading from socket"<<std::endl;
            break;//Some other error Stop handling this client
        }

        if (bytes_read == 0)
        {
            std::cout<<"Buffer returned empty -> Client disconnected"<<std::endl;
            break;
        }
        std::cout<<"Bytes read : "<<bytes_read<<std::endl;

        //Turns raw bytes into a std::string for easy printing //
        std::string message (buffer.data(), bytes_read);

        if (message == "exit")
        {
            std::cout << "Client requested exit. Closing connection.\n";
            break; // don't echo "exit" back, just close
        }


        std::cout<<"And the message is: "<<message<<std::endl;

        //Now lets try and echo from the server back into the client//
        boost::asio::write(socket,boost::asio::buffer(message));
        std::cout<<"The server Echoed the message "<<std::endl<<message<<std::endl;

    }
    //std::cin.get();//Stops the code so I can see how the network works using ss -tln

}
