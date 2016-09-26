// Copyright (C) 2016, Jean-Pierre de la Croix
// see LICENSE included with this software

#ifndef _POLOLU_MAESTRO_DEVICE_OPTIONS_HPP_
#define _POLOLU_MAESTRO_DEVICE_OPTIONS_HPP_

#include <string>

namespace pololu {

class DeviceOptions {

public:

  enum Type : int { MICRO_6, MINI_12, MINI_18, MINI_24 };
  enum Protocol : int { COMPACT, POLOLU, MINI_SSC };

  DeviceOptions(Type type, Protocol protocol, std::string device_file, unsigned char device_number)
    : type_(type), protocol_(protocol), device_file_(device_file), device_number_(device_number) {};

  DeviceOptions()
    : type_(Type::MICRO_6), protocol_(Protocol::COMPACT), device_file_("/dev/ttyACM0"), device_number_(0x0C) {};

  virtual ~DeviceOptions() {};

  Type getType() { return type_; }
  Protocol getProtocol() { return protocol_; }
  std::string getDeviceFile() { return device_file_; }
  unsigned char getDeviceNumber() { return device_number_; }

private:
  Type type_;
  Protocol protocol_;
  std::string device_file_;
  unsigned char device_number_;
};

}

#endif
