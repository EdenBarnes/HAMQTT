/*
 * Copyright 2025 Ethan Barnes
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file hamqtt_device.h
 * @brief Public API for representing a Home Assistant MQTT device.
 *
 * This header provides the `HAMQTT_Device` type and functions for managing
 * a device with one or more MQTT components in Home Assistant. It allows
 * registration of components, setting device metadata, and updating MQTT state.
 *
 * Components are dispatched using the HAMQTT_Component interface.
 *
 * @author Ethan Barnes
 * @date 2025
 * @copyright Apache License 2.0
 */

#pragma once

#include "common.h"
#include "hamqtt_component.h"

/**
 * @struct HAMQTT_Device_Config
 * @brief Configuration parameters for a HAMQTT device.
 */
typedef struct {
    char *mqtt_config_topic_prefix; ///< The prefix for Home Assistant MQTT discovery topics.
    char *mqtt_uri;                 ///< The URI of the MQTT broker (e.g., "mqtt://broker.hivemq.com:1883").
    char *mqtt_username;            ///< The MQTT username. @note Optional.
    char *mqtt_password;            ///< The MQTT password. @note Optional.
    char *manufacturer;             ///< The manufacturer of the device. @note Optional.
    char *model;                    ///< The model of the device. @note Optional.
    char *serial_number;            ///< The serial number of the device. @note Optional.
    char *unique_id;                ///< Unique identifier used for discovery in Home Assistant.
    char *sw_version;               ///< The software version of the device. @note Optional.
    char *hw_version;               ///< The hardware version of the device. @note Optional.
    char *origin_url;               ///< URL to documentation or device homepage. @note Optional.
    char *name;                     ///< The name of the device shown in Home Assistant.
} HAMQTT_Device_Config;

/**
 * @brief Returns a default-initialized device configuration.
 *
 * Caller is still responsible for assigning mqtt_uri and unique_id
 * 
 * @return An default-initialized HAMQTT_Device_Config struct.
 * 
 * @memberof HAMQTT_Device_Config
 */
HAMQTT_Device_Config hamqtt_device_config_default(void);

/**
 * @struct HAMQTT_Device
 * @brief Internal representation of a Home Assistant MQTT device.
 */
typedef struct HAMQTT_Device HAMQTT_Device;

/**
 * @brief Create a new HAMQTT device.
 *
 * @param config Pointer to a device configuration. Must remain valid for the lifetime of the device.
 * @return Pointer to the created HAMQTT_Device, or NULL on failure.
 * 
 * @memberof HAMQTT_Device
 */
HAMQTT_Device *hamqtt_device_create(HAMQTT_Device_Config *config);

/**
 * @brief Destroy a HAMQTT device and free all resources.
 *
 * @param device Pointer to the device to destroy. Must not be NULL.
 * 
 * @memberof HAMQTT_Device
 */
void hamqtt_device_destroy(HAMQTT_Device *device);

/**
 * @brief Add a component (e.g. sensor, switch) to the device.
 *
 * @param device Pointer to the device.
 * @param component Pointer to a component implementing the HAMQTT_Component interface.
 * @return
 * - ESP_OK on success
 * - ESP_ERR_NO_MEM if component buffer is full
 * - ESP_ERR_INVALID_ARG if inputs are invalid
 * 
 * @memberof HAMQTT_Device
 */
esp_err_t hamqtt_device_add_component(HAMQTT_Device *device, HAMQTT_Component *component);

/**
 * @brief Connect the device to the MQTT broker and publish its Home Assistant discovery config.
 *
 * This must be called before calling `hamqtt_device_loop`.
 *
 * @param device Pointer to the device.
 * @return
 * - ESP_OK on success
 * - ESP_FAIL or MQTT-related error code on failure
 * 
 * @memberof HAMQTT_Device
 */
esp_err_t hamqtt_device_connect(HAMQTT_Device *device);

/**
 * @brief Publish an availability message to Home Assistant.
 *
 * @param device Pointer to the device.
 * @param availability true if available, false if offline.
 * @return
 * - ESP_OK on success
 * - ESP_ERR_INVALID_STATE if the device has not connected to MQTT
 * 
 * @memberof HAMQTT_Device
 */
esp_err_t hamqtt_device_publish_availability(const HAMQTT_Device *device, bool availability);

/**
 * @brief Update the state of all components and publish to MQTT.
 *
 * This should be called periodically to keep sensor states up to date.
 *
 * @param device Pointer to the device.
 * 
 * @memberof HAMQTT_Device
 */
void hamqtt_device_loop(const HAMQTT_Device *device);

/**
 * @brief Get the configuration used to initialize the device.
 *
 * @param device Pointer to the device.
 * @return Pointer to the original configuration struct.
 * 
 * @memberof HAMQTT_Device
 */
const HAMQTT_Device_Config *hamqtt_device_get_config(const HAMQTT_Device *device);