
#include <pololu_maestro/DeviceOptions.hpp>

#include <unordered_map>

namespace pololu {

class MaestroDevice {

public:

  explicit MaestroDevice(DeviceOptions options = DeviceOptions());

  virtual ~MaestroDevice() {}

  // Actions
  void connect();
  void disconnect();

  // Commands
  void setTargetOnChannel(unsigned char channel, unsigned short target);
  void setMultipleTargets(const std::unordered_map<unsigned char, unsigned short> target_on_channel_map);
  void setSpeedOnChannel(unsigned char channel, unsigned short speed);
  void setAccelerationOnChannel(unsigned char channel, unsigned short acceleration);
  void setPwm(unsigned short on_time, unsigned short period);
  void getPositionOnChannel(unsigned char channel, unsigned short &position);
  void getMovingState(unsigned char &state);
  void getErrors(unsigned short &error);
  void goHome();

private:
  DeviceOptions options_;
  int file_descriptor_;

  void send(unsigned char *command, unsigned int command_length);
  void receive(unsigned char *response, unsigned int response_length);

  void throw_exception(const std::string &preamble);

};

}
