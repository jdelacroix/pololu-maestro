// Copyright (C) 2016, Jean-Pierre de la Croix
// see LICENSE included with this software

#include <pololu_maestro/MaestroDevice.hpp>

#include <cstdlib>
#include <iostream>

int main(int args, char **argv) {

  pololu::MaestroDevice device;

  device.connect();

  int target = std::atoi(argv[1]);

  unsigned short rc = 128;

  device.setTargetOnChannel(3, target*4);
  device.getErrors(rc);
  std::cout << "Error code: " << rc << std::endl;

  unsigned short position = 128;
  device.getPositionOnChannel(3, position);
  std::cout << "Position of CH1: " << position/4 << std::endl;

  device.disconnect();

  return 0;
}
