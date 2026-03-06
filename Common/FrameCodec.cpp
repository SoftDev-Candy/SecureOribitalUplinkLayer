//
// Created by Candy on 3/6/26.
//

#include "FrameCodec.hpp"

std::vector<uint8_t> FrameCodec::EncodeFrame(std::string Json_str)
{
    //Get Json string size
    uint32_t json_size = static_cast<uint32_t>(Json_str.size());

    //Create a buffer with 4 bytes for size (like for example - big endian)

    std::vector<uint8_t> buffer;

    buffer.push_back((json_size >> 24) & 0xFF);
    buffer.push_back((json_size >> 16) & 0xFF);
    buffer.push_back((json_size >> 8) & 0xFF);
    buffer.push_back(json_size & 0xFF);

    //Insert it into the buffer and return the encoding bytes//
    buffer.insert(buffer.end(), Json_str.begin(), Json_str.end());

    return buffer;

}

std::string FrameCodec::DecodeFrame(boost::asio::ip::tcp::socket socket)
{


}
