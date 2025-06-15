#pragma once

#include "common.h"

#include "hamqtt_component.h"

typedef struct {
    char *mqtt_config_topic_prefix;
    char *mqtt_uri;
    char *mqtt_username;
    char *mqtt_password;
    char *manufacturer;
    char *model;
    char *serial_number;
    char *unique_id;
    char *sw_version;
    char *hw_version;
    char *origin_url;
    char *name;
} HAMQTT_Device_Config;

typedef struct HAMQTT_Device HAMQTT_Device;

/* ----- Public HAMQTT Device function declarations ----- */

// Create a new HAMQTT Device and return a pointer to it
HAMQTT_Device *hamqtt_device_create(HAMQTT_Device_Config *config);

// Take a pointer to an HAMQTT Device and deallocate its memory
void hamqtt_device_destroy(HAMQTT_Device *device);

// Add a component to be handled by the device
esp_err_t hamqtt_device_add_component(HAMQTT_Device *device, HAMQTT_Component *component);

// Connect to the MQTT broker and publish HomeAssistant config
esp_err_t hamqtt_device_connect(HAMQTT_Device *device);

// Tell HomeAssistant whether the device is available
esp_err_t hamqtt_device_publish_availability(const HAMQTT_Device *device, bool availability);

// Update HomeAssistant sensors and publish their states
void hamqtt_device_loop(const HAMQTT_Device *device);

// Get the config of a device
HAMQTT_Device_Config *get_config(const HAMQTT_Device *device);

// Set the config of a device
// TODO: Decide if we should have this. Likely would cause way too many issues
// void set_config(HAMQTT_Device_Config config);


