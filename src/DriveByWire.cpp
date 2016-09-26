// Copyright (C) 2016, Jean-Pierre de la Croix
// see LICENSE included with this software

#include <pololu_maestro/MaestroDevice.hpp>

#include <sabre/DriveByWire.hpp>
#include <sabre/Constants.hpp>

#include <algorithm>
#include <iostream>

namespace sabre {

DriveByWire::DriveByWire()
  : sabre_device_(std::make_shared<pololu::MaestroDevice>()) {
  // no-op
}

void DriveByWire::connect() {
  sabre_device_->connect();
  sabre_device_->setTargetOnChannel(SABRE_THROTTLE_CHANNEL, SABRE_THROTTLE_TARGET_NEUTRAL);
  sabre_device_->setTargetOnChannel(SABRE_STEERING_CHANNEL, SABRE_STEERING_TARGET_NEUTRAL);
}

void DriveByWire::disconnect() {
  reset();
  sabre_device_->disconnect();
}

void DriveByWire::setThrottlePosition(double position) {
  double bounded_position = std::max(std::min(position, 1.0), -1.0);
  if (bounded_position != position) {
    std::cout << "warning: clamped position (" << position << ") to [-1.0,1.0]" << std::endl;
  }
  std::cout << "Setting throttle position to (" << bounded_position << ")" << std::endl;

  if (bounded_position == 0.0) {
    sabre_device_->setTargetOnChannel(SABRE_THROTTLE_CHANNEL, SABRE_THROTTLE_TARGET_NEUTRAL);
  } else if (bounded_position > 0.0) {
    unsigned short target = (unsigned short) (((SABRE_THROTTLE_TARGET_FORWARD-SABRE_THROTTLE_TARGET_NEUTRAL)*bounded_position)+SABRE_STEERING_TARGET_NEUTRAL);
    sabre_device_->setTargetOnChannel(SABRE_THROTTLE_CHANNEL, target);
  } else {
    unsigned short target = (unsigned short) (SABRE_THROTTLE_TARGET_NEUTRAL-((SABRE_THROTTLE_TARGET_REVERSE-SABRE_THROTTLE_TARGET_NEUTRAL)*bounded_position));
    sabre_device_->setTargetOnChannel(SABRE_THROTTLE_CHANNEL, target);
  }
}

void DriveByWire::setSteeringPosition(double position) {
  double bounded_position = std::max(std::min(position, 1.0), -1.0);
  if (bounded_position != position) {
    std::cout << "warning: clamped position (" << position << ") to [-1.0,1.0]" << std::endl;
  }
  std::cout << "Setting steering position to (" << bounded_position << ")" << std::endl;

  if (bounded_position == 0.0) {
    sabre_device_->setTargetOnChannel(SABRE_STEERING_CHANNEL, SABRE_STEERING_TARGET_NEUTRAL);
  } else if (bounded_position > 0.0) {
    unsigned short target = (unsigned short) (((SABRE_STEERING_TARGET_RIGHT-SABRE_STEERING_TARGET_NEUTRAL)*bounded_position)+SABRE_STEERING_TARGET_NEUTRAL);
    sabre_device_->setTargetOnChannel(SABRE_STEERING_CHANNEL, target);
  } else {
    unsigned short target = (unsigned short) (SABRE_STEERING_TARGET_NEUTRAL-((SABRE_STEERING_TARGET_LEFT-SABRE_STEERING_TARGET_NEUTRAL)*bounded_position));
    sabre_device_->setTargetOnChannel(SABRE_STEERING_CHANNEL, target);
  }
}

void DriveByWire::stop() {
  unsigned short position = 0;
  sabre_device_->getPositionOnChannel(SABRE_THROTTLE_CHANNEL, position);
  if (position != SABRE_THROTTLE_TARGET_NEUTRAL) {
    // throw throttle in the opposite direction to stop immediately, not coast
    if (position > SABRE_THROTTLE_TARGET_NEUTRAL) {
      sabre_device_->setTargetOnChannel(SABRE_THROTTLE_CHANNEL, (unsigned short) SABRE_THROTTLE_TARGET_REVERSE_SLOW);
    } else {
      sabre_device_->setTargetOnChannel(SABRE_THROTTLE_CHANNEL, (unsigned short) SABRE_THROTTLE_TARGET_FORWARD_SLOW);
    }
  }
  sabre_device_->setTargetOnChannel(SABRE_THROTTLE_CHANNEL, (unsigned short) SABRE_THROTTLE_TARGET_NEUTRAL);
}

void DriveByWire::reset() {
  stop();
  setSteeringPosition(0.0);
}

}
