// Copyright (C) 2016, Jean-Pierre de la Croix
// see LICENSE included with this software

#ifndef _SABRE_DRIVE_BY_WIRE_HPP_
#define _SABRE_DRIVE_BY_WIRE_HPP_

#include <pololu_maestro/MaestroDevice.hpp>

#include <memory>

namespace sabre {

class DriveByWire {

public:
  explicit DriveByWire();
  virtual ~DriveByWire() {}

  void connect();
  void disconnect();

  //
  void setThrottlePosition(double position);
  void setSteeringPosition(double position);

  // actions
  void stop();
  void reset();

private:
  int throttle_target_;
  int steering_target_;

  std::shared_ptr<pololu::MaestroDevice> sabre_device_ = nullptr;

};

}

#endif
