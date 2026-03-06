//
// Created by Candy on 3/2/26.
//

#include "TelemetryFrame.hpp"

#include <iostream>
namespace json = boost::json;

std::string TelemetryFrame::ToJson() const
{

  //Create a JSON Object here//
  json::object obj;

  //Setting the JSON obj value to be serialized
  obj["sat_id"]     =  this->sat_id;
  obj["sequence"]     =  this->sequence;
  obj["timestamp_ms"] =  this->timestamp_ms;
  obj["battery"]      =  this->battery;
  obj["temp_c"]       =  this->temp_c;

  std::string serialized_str = json::serialize(obj);

  std::cout<<serialized_str<<std::endl;

  return serialized_str;
}

TelemetryFrame TelemetryFrame::FromJson(const std::string &json_string)
{
  //Telemetry Frame struct object //
  TelemetryFrame tf;

  json::error_code ec;

  //Parsing the string and storing its value
  json::value val_from_string = json::parse(json_string, ec);

  if (ec)
  {
    std::cerr<<"Failed to parse Json "<<ec.message()<<std::endl;
    //I want to use throw, but I don't believe its necessary
  }

  if (!val_from_string.is_object())
  {
    throw std::runtime_error("Failed to parse JSON data");
  }

  //  Created an obj from Value
  json::object& val_obj = val_from_string.get_object();

 //To check and validate if obj contain their respective values
  if (!val_obj.contains("sat_id"))
  {
    throw std::runtime_error("Failed to find sat_id in JSON data");
  }

  else if (!val_obj.contains("sequence"))
  {
    throw std::runtime_error("Failed to find sequence in JSON data");
  }

  else if (!val_obj.contains("timestamp_ms"))
  {
    throw std::runtime_error("Failed to find timestamp_ms in JSON data");
  }

  else if (!val_obj.contains("battery"))
  {
    throw std::runtime_error("Failed to find battery in JSON data");
  }

  else if (!val_obj.contains("temp_c"))
  {
    throw std::runtime_error("Failed to find temp_c in JSON data");
  }


  try
  {

    //Setting values in telemetry frame's object using value_to function for safe deserialization
  tf.sat_id =  json::value_to<std::string>(val_obj.at("sat_id"));;
  tf.sequence =  json::value_to<uint64_t>(val_obj.at("sequence"));;
  tf.timestamp_ms =  json::value_to<uint64_t>(val_obj.at("timestamp_ms"));;
  tf.battery =  json::value_to<float>(val_obj.at("battery"));;
  tf.temp_c =  json::value_to<float>(val_obj.at("temp_c"));;

  }

  catch (std::exception& e)
  {
    std::cerr<<"Error Accessing JSON field's"<<e.what()<<std::endl;
  }

return tf;
}
