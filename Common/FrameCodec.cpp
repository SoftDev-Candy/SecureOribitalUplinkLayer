//
// Created by Candy on 3/6/26.
//

#include "FrameCodec.hpp"
#include <iostream>
#include <boost/asio/read.hpp>

std::vector<uint8_t> FrameCodec::EncodeFrame(std::string Json_str)
{
    // Get JSON string size //
    uint32_t json_size = static_cast<uint32_t>(Json_str.size());

    //Create a buffer with 4 bytes for size (like for example - big endian)
    std::vector<uint8_t> buffer;
    // Push it back into the buffer bit by bit //
    buffer.push_back((json_size >> 24) & 0xFF);
    buffer.push_back((json_size >> 16) & 0xFF);
    buffer.push_back((json_size >> 8) & 0xFF);
    buffer.push_back(json_size & 0xFF);

    // Insert it into the buffer and return the encoding bytes //
    buffer.insert(buffer.end(), Json_str.begin(), Json_str.end());

    return buffer;
}

std::string FrameCodec::DecodeFrame(boost::asio::ip::tcp::socket& socket)
{
    // 4-byte Vector buffer that will store the payloads lengths //
    std::array<uint8_t , 4>header ;

    //Max Frame Size Element to check payload length is correct or no strict for now will variate it later //
    constexpr uint32_t Max_Frame_Size = 64 * 1024;

    //System Error code to check for any bad errors
        boost::system::error_code error;

    //Will use bytes read for any Eof errors we might run into //
    std::size_t bytes_read  = boost::asio::read(socket,boost::asio::buffer(header , 4),error);


    if (error == boost::asio::error::eof)
    {
        std::cerr<<" DecoderFrame: Peer Closed Connection  End of file(EOF) Error"<<std::endl;
        return {};
    }

    if (error)
    {
        std::cerr<<"DecodeFrame: failed to read header"<<std::endl;
        return {};//if Some other error Stop handling this client
    }


    if (bytes_read != 4)
    {
        std::cerr<<"DecoderFrame : Incoming bytes insufficient"<<std::endl;
         return {} ; // Just return an Empty String //
    }

    // Decoding and storing the length of the incoming vector //
    uint32_t payload_length = (uint32_t(header[0]) << 24) |
        (uint32_t(header[1]) << 16) |
        (uint32_t(header[2]) << 8)  |
        (uint32_t(header[3]) << 0);

    //If payload length is 0 or too big send error else set payload
    if ( payload_length == 0|| payload_length > Max_Frame_Size)
    {
        std::cerr<<"DecodeFrame: payload length incorrect"<<payload_length<<std::endl;
        return {};
    }


    // Char vector to store the data from the payload length
    std::vector<char> payload(payload_length);
    std::size_t payload_bytes_read = boost::asio::read(socket,boost::asio::buffer(payload , payload_length) , error);

    if (error == boost::asio::error::eof)
    {
        std::cerr << "DecodeFrame: peer closed connection while reading payload (EOF)"<<std::endl;
        return {};
    }

    if (error)
    {
        std::cerr<<"DecodeFrame: failed to read payload "<<error.message()<<std::endl;
        return {};//if Some other error Stop handling this client
    }

    if (payload_bytes_read != payload_length)
    {
        std::cerr << "DecodeFrame: incomplete payload\n";
        return {};
    }


    // Converting the char vector into string for output //
    std::string json_str(payload.begin(), payload.end());
    return json_str;

}
