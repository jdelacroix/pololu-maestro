// Copyright (C) 2016, Jean-Pierre de la Croix
// see LICENSE included with this software

#include <sabre/DriveByWire.hpp>

#include <SDL2/SDL.h>

#include <stdexcept>
#include <iostream>

int main(int args, char **argv) {

  sabre::DriveByWire drive_by_wire;
  drive_by_wire.connect();

  if (SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER)) {
    throw std::runtime_error("Unable to initialize SDL and its subsystems.");
  }

  SDL_GameControllerEventState(SDL_ENABLE);

  SDL_GameController* game_controller = nullptr;

  int joystick_count = SDL_NumJoysticks();

  if (joystick_count > 0) {
    for (int i=0 ; i<joystick_count; i++) {
      if (SDL_IsGameController(i)) {
        std::cout << SDL_GameControllerNameForIndex(i) << " is a compatible game controller." << std::endl;
        game_controller = SDL_GameControllerOpen(i);
        if (game_controller) {
          std::cout << "Selected." << std::endl;
        } else {
          throw std::runtime_error("Unable to open this game controller!");
        }
      }
    }
  } else {
    throw std::runtime_error("No joysticks are connected to this device.");
  }

  SDL_Event e;

  bool ok = true;

  while (ok) {
    if (SDL_WaitEvent(&e)) {
      switch(e.type) {
        case SDL_CONTROLLERBUTTONDOWN:
          if (e.cbutton.button == SDL_CONTROLLER_BUTTON_GUIDE) {
            std::cout << "Termination signal received." << std::endl;
            ok = false;
          } else if (e.cbutton.button == SDL_CONTROLLER_BUTTON_B) {
            // stop
            drive_by_wire.stop();
          } else if (e.cbutton.button == SDL_CONTROLLER_BUTTON_Y) {
            // reset
            drive_by_wire.reset();
          }
          break;
        case SDL_CONTROLLERAXISMOTION:
          if (e.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT) {
            // throttle forward
            double position = e.caxis.value / 32767.0;
            drive_by_wire.setThrottlePosition(position);
          } else if (e.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT) {
            // throttle reverse
            double position = e.caxis.value / 32767.0;
            drive_by_wire.setThrottlePosition(-position);
          } else if (e.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX) {
            // steering
            if (e.caxis.value == 0) {
              drive_by_wire.setSteeringPosition(0.0);
            } if (e.caxis.value > 0) {
              double position = e.caxis.value / 32767.0;
              drive_by_wire.setSteeringPosition(position);
            } else {
              double position = e.caxis.value / 32768.0;
              drive_by_wire.setSteeringPosition(position);
            }

          }
      }
    } else {
      throw std::runtime_error("SDL_WaitEvent: " + std::string(SDL_GetError()));
    }
  }



  SDL_GameControllerClose(game_controller);
  SDL_Quit();

  drive_by_wire.disconnect();

}
