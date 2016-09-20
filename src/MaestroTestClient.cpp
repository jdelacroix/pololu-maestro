// Copyright (C) 2016, Jean-Pierre de la Croix
// see LICENSE included with this software

#include <pololu_maestro/MaestroDevice.hpp>

int main(int args, char **argv) {

  pololu::MaestroDevice device;

  device.connect();

  device.disconnect();

  return 0;
}
