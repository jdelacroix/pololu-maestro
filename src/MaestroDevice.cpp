#include <pololu_maestro/MaestroDevice.hpp>

#include <stdexcept>
#include <sstream>
#include <vector>

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#include <cstring>
#include <cerrno>
#include <cstdlib>

namespace pololu {

MaestroDevice::MaestroDevice(DeviceOptions options)
  : options_(options) {
    // no-op
}

void MaestroDevice::connect() {
  // Open the Maestro's virtual COM port.
  int file_descriptor_ = open(options_.getDeviceFile().c_str(), O_RDWR | O_NOCTTY);
  if (file_descriptor_ == -1) {
    throw_exception(options_.getDeviceFile() + ": ");
  }

  // Set virtual COM port options
  struct termios options;
  tcgetattr(file_descriptor_, &options);
  options.c_iflag &= ~(INLCR | IGNCR | ICRNL | IXON | IXOFF);
  options.c_oflag &= ~(ONLCR | OCRNL);
  options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  tcsetattr(file_descriptor_, TCSANOW, &options);
}

void MaestroDevice::disconnect() {
  close(file_descriptor_);
}

void MaestroDevice::send(unsigned char *command, unsigned int command_length) {
  if(write(file_descriptor_, command, command_length) == -1) {
    throw_exception(options_.getDeviceFile() + ": ");
  }
}

void MaestroDevice::receive(unsigned char *response, unsigned int response_length) {
  if(read(file_descriptor_, response, response_length) == -1) {
    throw_exception(options_.getDeviceFile() + ": ");
  }
}


void MaestroDevice::setTargetOnChannel(unsigned char channel, unsigned short target) {
  std::vector<unsigned char> command;

  switch(options_.getProtocol()) {
    case DeviceOptions::Protocol::COMPACT:
      command.push_back(0x84);
      break;
    case DeviceOptions::Protocol::POLOLU:
      command.push_back(0xAA);
      command.push_back(options_.getDeviceNumber());
      command.push_back(0x04);
      break;
    default:
      throw std::runtime_error("setTargetOnChannel(): not supported by our implementation of protocol MINI_SSC");
  }
  command.push_back(channel);
  command.push_back(target & 0x7F);
  command.push_back((target >> 7) & 0x7F);
  send(command.data(), command.size());
}

void MaestroDevice::setMultipleTargets(const std::unordered_map<unsigned char, unsigned short> target_on_channel_map) {
  if (target_on_channel_map.size() > 0) {
    std::vector<unsigned char> command;

    switch(options_.getProtocol()) {
      case DeviceOptions::Protocol::COMPACT:
        command.push_back(0x84);
        break;
      case DeviceOptions::Protocol::POLOLU:
        command.push_back(0xAA);
        command.push_back(options_.getDeviceNumber());
        command.push_back(0x04);
        break;
      default:
        throw std::runtime_error("setMultipleTargets(): not supported by our implementation of protocol MINI_SSC");
    }
    for (auto pair : target_on_channel_map) {
      command.push_back(pair.first);
      command.push_back(pair.second & 0x7F);
      command.push_back((pair.second >> 7) & 0x7F);
    }
    send(command.data(), command.size());
  } else {
    throw std::runtime_error("setMultipleTargets(): no target-channel pairs provided in the map");
  }
}

void MaestroDevice::setSpeedOnChannel(unsigned char channel, unsigned short speed) {
  std::vector<unsigned char> command;

  switch(options_.getProtocol()) {
    case DeviceOptions::Protocol::COMPACT:
      command.push_back(0x87);
      break;
    case DeviceOptions::Protocol::POLOLU:
      command.push_back(0xAA);
      command.push_back(options_.getDeviceNumber());
      command.push_back(0x07);
      break;
    default:
      throw std::runtime_error("setSpeedOnChannel(): not supported by protocol MINI_SSC");
  }
  command.push_back(channel);
  command.push_back(speed & 0x7F);
  command.push_back((speed >> 7) & 0x7F);
  send(command.data(), command.size());
}

void MaestroDevice::setAccelerationOnChannel(unsigned char channel, unsigned short acceleration) {
  std::vector<unsigned char> command;

  switch(options_.getProtocol()) {
    case DeviceOptions::Protocol::COMPACT:
      command.push_back(0x89);
      break;
    case DeviceOptions::Protocol::POLOLU:
      command.push_back(0xAA);
      command.push_back(options_.getDeviceNumber());
      command.push_back(0x09);
      break;
    default:
      throw std::runtime_error("setAccelerationOnChannel(): not supported by protocol MINI_SSC");
  }
  command.push_back(channel);
  command.push_back(acceleration & 0x7F);
  command.push_back((acceleration >> 7) & 0x7F);
  send(command.data(), command.size());
}

void MaestroDevice::setPwm(unsigned short on_time, unsigned short period) {
  if (options_.getType() != DeviceOptions::Type::MICRO_6) {
    std::vector<unsigned char> command;

    switch(options_.getProtocol()) {
      case DeviceOptions::Protocol::COMPACT:
        command.push_back(0x8A);
        break;
      case DeviceOptions::Protocol::POLOLU:
        command.push_back(0xAA);
        command.push_back(options_.getDeviceNumber());
        command.push_back(0x0A);
        break;
      default:
        throw std::runtime_error("setPwm(): not supported by protocol MINI_SSC");
    }
    command.push_back(on_time & 0x7F);
    command.push_back((on_time >> 7) & 0x7F);
    command.push_back(period & 0x7F);
    command.push_back((period >> 7) & 0x7F);
    send(command.data(), command.size());
  } else {
    throw std::runtime_error("getMovingState(): not supported on platform MICRO_6");
  }
}

void MaestroDevice::getPositionOnChannel(unsigned char channel, unsigned short &position) {
  std::vector<unsigned char> command;

  switch(options_.getProtocol()) {
    case DeviceOptions::Protocol::COMPACT:
      command.push_back(0x90);
      break;
    case DeviceOptions::Protocol::POLOLU:
      command.push_back(0xAA);
      command.push_back(options_.getDeviceNumber());
      command.push_back(0x10);
      break;
    default:
      throw std::runtime_error("getPositionOnChannel(): not supported by protocol MINI_SSC");
  }
  command.push_back(channel);
  send(command.data(), command.size());

  unsigned int response_length = 2;
  unsigned char *response = (unsigned char*) std::calloc(response_length, sizeof(unsigned char));

  receive(response, response_length);

  position = (response[1] << 8) | response[0];
}

void MaestroDevice::getMovingState(unsigned char &state) {
  if (options_.getType() != DeviceOptions::Type::MICRO_6) {
    std::vector<unsigned char> command;

    switch(options_.getProtocol()) {
      case DeviceOptions::Protocol::COMPACT:
        command.push_back(0x93);
        break;
      case DeviceOptions::Protocol::POLOLU:
        command.push_back(0xAA);
        command.push_back(options_.getDeviceNumber());
        command.push_back(0x13);
        break;
      default:
        throw std::runtime_error("getMovingState(): not supported by proto col MINI_SSC");
    }
    send(command.data(), command.size());

    unsigned int response_length = 1;
    unsigned char *response = (unsigned char*) std::calloc(response_length, sizeof(unsigned char));

    receive(response, response_length);

    state = response[0];
  } else {
    throw std::runtime_error("getMovingState(): not supported on platform MICRO_6");
  }
}

void MaestroDevice::getErrors(unsigned short &error) {
  std::vector<unsigned char> command;

  switch(options_.getProtocol()) {
    case DeviceOptions::Protocol::COMPACT:
      command.push_back(0xA1);
      break;
    case DeviceOptions::Protocol::POLOLU:
      command.push_back(0xAA);
      command.push_back(options_.getDeviceNumber());
      command.push_back(0x21);
      break;
    default:
      throw std::runtime_error("getErrors(): not supported by protocol MINI_SSC");
  }
  send(command.data(), command.size());

  unsigned int response_length = 2;
  unsigned char *response = (unsigned char*) std::calloc(response_length, sizeof(unsigned char));

  receive(response, response_length);

  error = (response[1] << 8) | response[0];
}

void MaestroDevice::goHome() {
  std::vector<unsigned char> command;

  switch(options_.getProtocol()) {
    case DeviceOptions::Protocol::COMPACT:
      command.push_back(0xA2);
      break;
    case DeviceOptions::Protocol::POLOLU:
      command.push_back(0xAA);
      command.push_back(options_.getDeviceNumber());
      command.push_back(0x22);
      break;
    default:
      throw std::runtime_error("goHome(): not supported by protocol MINI_SSC");
  }
  send(command.data(), command.size());
}

void MaestroDevice::throw_exception(const std::string &preamble) {
  std::stringstream error_msg;
  error_msg << preamble << std::strerror(errno);
  throw std::runtime_error(error_msg.str());
}

}
