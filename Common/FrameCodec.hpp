//
// Created by Candy on 3/6/26.
//

#ifndef SOUL_FRAMECODEC_HPP
#define SOUL_FRAMECODEC_HPP

#include <string>
#include <boost/asio/ip/tcp.hpp>

class FrameCodec
{
    private:
    //Nothing to add here yet//

    public:
    FrameCodec();

    static std::vector<uint8_t> EncodeFrame(std::string Json_str);
    static std::string DecodeFrame(boost::asio::ip::tcp::socket socket);

    ~FrameCodec();

};


#endif //SOUL_FRAMECODEC_HPP