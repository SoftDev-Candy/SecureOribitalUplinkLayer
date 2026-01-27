#include<iostream>
#include <boost/asio.hpp>

//Step-2 Defining the TCP type shortcut so we dont have to keep calling it.
using boost::asio::ip::tcp;

int main()
{
    //TODO : Figure out, how to work with error codes in Boost::asio
    try {
        //Step -1 Create I/O context(Think of this as "I/O Engine")
        boost::asio::io_service io_service; //basically hides all the boost specific things

        //Step-3 Create an endpoint by basically giving a string to get converted into something it understands//
        unsigned short PORT = 5000;
        tcp::endpoint endpoint(boost::asio::ip::make_address("127.0.0.1"), PORT);

        //  //Create a socket here //
        // tcp::socket socket(io_service);
        //
        //  //Step-5 ask the socket to try and connect to someting
        //  socket.connect()
        //

        //Step-5 Create a acceptor I dont know what this  but I need to bound to that enddpoint
        tcp::acceptor acceptor(io_service , endpoint);

        std::cout<<"Server Bound to 127.0.0.1: "<<PORT<<std::endl;

    }catch(std::exception& e) {
        std::cerr<<e.what()<<std::endl;
        return 1;

    }


    return 0;
}