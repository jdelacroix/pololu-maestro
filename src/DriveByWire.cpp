// Copyright (C) 2016, Jean-Pierre de la Croix
// see LICENSE included with this software

#include <pololu_maestro/MaestroDevice.hpp>

#include <traxxas/DriveByWire.hpp>
#include <traxxas/Constants.hpp>

#include <algorithm>
#include <iostream>

namespace traxxas {

DriveByWire::DriveByWire()
  : traxxas_device_(std::make_shared<pololu::MaestroDevice>()) {
  // no-op
}

void DriveByWire::connect() {
  traxxas_device_->connect();
  traxxas_device_->setTargetOnChannel(TRAXXAS_THROTTLE_CHANNEL, TRAXXAS_THROTTLE_TARGET_NEUTRAL);
  traxxas_device_->setTargetOnChannel(TRAXXAS_STEERING_CHANNEL, TRAXXAS_STEERING_TARGET_NEUTRAL);
}

void DriveByWire::disconnect() {
  reset();
  traxxas_device_->disconnect();
}

void DriveByWire::setThrottlePosition(double position) {
  double bounded_position = std::max(std::min(position, 1.0), -1.0);
  if (bounded_position != position) {
    std::cout << "warning: clamped position (" << position << ") to [-1.0,1.0]" << std::endl;
  }
  std::cout << "Setting throttle position to (" << bounded_position << ")" << std::endl;

  if (bounded_position == 0.0) {
    traxxas_device_->setTargetOnChannel(TRAXXAS_THROTTLE_CHANNEL, TRAXXAS_THROTTLE_TARGET_NEUTRAL);
  } else if (bounded_position > 0.0) {
    unsigned short target = (unsigned short) (((TRAXXAS_THROTTLE_TARGET_FORWARD-TRAXXAS_THROTTLE_TARGET_NEUTRAL)*bounded_position)+TRAXXAS_STEERING_TARGET_NEUTRAL);
    traxxas_device_->setTargetOnChannel(TRAXXAS_THROTTLE_CHANNEL, target);
  } else {
    unsigned short target = (unsigned short) (TRAXXAS_THROTTLE_TARGET_NEUTRAL-((TRAXXAS_THROTTLE_TARGET_REVERSE-TRAXXAS_THROTTLE_TARGET_NEUTRAL)*bounded_position));
    traxxas_device_->setTargetOnChannel(TRAXXAS_THROTTLE_CHANNEL, target);
  }
}

void DriveByWire::setSteeringPosition(double position) {
  double bounded_position = std::max(std::min(position, 1.0), -1.0);
  if (bounded_position != position) {
    std::cout << "warning: clamped position (" << position << ") to [-1.0,1.0]" << std::endl;
  }
  std::cout << "Setting steering position to (" << bounded_position << ")" << std::endl;

  if (bounded_position == 0.0) {
    traxxas_device_->setTargetOnChannel(TRAXXAS_STEERING_CHANNEL, TRAXXAS_STEERING_TARGET_NEUTRAL);
  } else if (bounded_position > 0.0) {
    unsigned short target = (unsigned short) (((TRAXXAS_STEERING_TARGET_RIGHT-TRAXXAS_STEERING_TARGET_NEUTRAL)*bounded_position)+TRAXXAS_STEERING_TARGET_NEUTRAL);
    traxxas_device_->setTargetOnChannel(TRAXXAS_STEERING_CHANNEL, target);
  } else {
    unsigned short target = (unsigned short) (TRAXXAS_STEERING_TARGET_NEUTRAL-((TRAXXAS_STEERING_TARGET_LEFT-TRAXXAS_STEERING_TARGET_NEUTRAL)*bounded_position));
    traxxas_device_->setTargetOnChannel(TRAXXAS_STEERING_CHANNEL, target);
  }
}

void DriveByWire::stop() {
  unsigned short position = 0;
  traxxas_device_->getPositionOnChannel(TRAXXAS_THROTTLE_CHANNEL, position);
  if (position != TRAXXAS_THROTTLE_TARGET_NEUTRAL) {
    // throw throttle in the opposite direction to stop immediately, not coast
    if (position > TRAXXAS_THROTTLE_TARGET_NEUTRAL) {
      traxxas_device_->setTargetOnChannel(TRAXXAS_THROTTLE_CHANNEL, (unsigned short) TRAXXAS_THROTTLE_TARGET_REVERSE_SLOW);
    } else {
      traxxas_device_->setTargetOnChannel(TRAXXAS_THROTTLE_CHANNEL, (unsigned short) TRAXXAS_THROTTLE_TARGET_FORWARD_SLOW);
    }
  }
  traxxas_device_->setTargetOnChannel(TRAXXAS_THROTTLE_CHANNEL, (unsigned short) TRAXXAS_THROTTLE_TARGET_NEUTRAL);
}

void DriveByWire::reset() {
  stop();
  setSteeringPosition(0.0);
}

}
