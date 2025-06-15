idf_component_register(
  SRCS 
    "src/hamqtt_device.c" 
    "src/hamqtt_binary_sensor.c" 
    "src/hamqtt_button.c"
  INCLUDE_DIRS "include"
  REQUIRES mqtt json
)