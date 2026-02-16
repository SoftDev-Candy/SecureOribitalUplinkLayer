//
// Created by Candy on 2/7/26.
//

#include"ChatServer.h"
int main()
{
    try {
        //Create a card to show this is where I am and this is what I would like to use .
        unsigned short int PORT = 5000;
        ChatServer Server("127.0.0.1" , PORT);
        Server.RunServer();

        return 0;


    }catch(std::exception& e) {
        std::cout<<e.what()<<std::endl;

    return 1;
    }

}