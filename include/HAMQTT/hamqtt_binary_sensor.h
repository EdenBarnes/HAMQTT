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
 * @file hamqtt_binary_sensor.h
 * @brief HAMQTT Binary Sensor component interface for Home Assistant MQTT discovery.
 *
 * Declares the interface and configuration for binary sensor components, such as door
 * sensors or motion detectors, within the HAMQTT MQTT integration framework for Home Assistant.
 *
 * @author Ethan Barnes
 * @date 2025
 * @copyright Apache License 2.0
 */

#pragma once

#include "common.h"

/**
 * @brief Configuration parameters for a HAMQTT binary sensor.
 */
typedef struct {
    char *device_class;         ///< The class of the sensor, which alters how its state and icon is rendered. @note Optional.
    bool enabled_by_default;    ///< Set to `false` if the device should not be enabled when the device is first added.
    char *entity_picture;       ///< URL to a picture for the entity. @note Optional.
    int expire_after;           ///< Number of seconds a sensor's state expires, if it's not updated. Set to `-1` to disable expiration.
    bool force_update;          ///< Set to `true` if Home Assistant should always send update events, even when the sensor's state doesn't change.
    char *icon;                 ///< Icon for the sensor. @note Optional.
    char *name;                 ///< The name of the sensor shown in Home Assistant.
    int off_delay;              ///< Number of seconds after the sensor turns on that Home Assistant turns it back off. Set to `-1` to disable this feature.
    char *unique_id;            ///< Unique identifier used for discovery in Home Assistant.
} HAMQTT_Binary_Sensor_Config;

/**
 * @brief Returns a default-initialized binary sensor configuration.
 *
 * Caller is still responsible for assigning unique_id
 * 
 * @return An default-initialized HAMQTT_Binary_Sensor_Config struct.
 * 
 * @memberof HAMQTT_Binary_Sensor_Config
 */
HAMQTT_Binary_Sensor_Config hamqtt_binary_sensor_config_default(void);

/**
 * @struct HAMQTT_Binary_Sensor
 * @brief Internal representation of a Home Assistant MQTT binary sensor.
 */
typedef struct HAMQTT_Binary_Sensor HAMQTT_Binary_Sensor;

/**
 * @typedef HAMQTT_Binary_Sensor_Get_State_Func
 * @brief Function pointer type for retrieving the state of a binary sensor.
 *
 * This function pointer is used to obtain the current state of a binary sensor.
 * It is expected to return `true` if the sensor is active/on, or `false` otherwise.
 *
 * @param args A pointer to user-defined arguments or context data required to get the state.
 *             This is typically a pointer to the sensor-specific data structure.
 * @return `true` if the sensor is active; `false` otherwise.
 */
typedef bool (*HAMQTT_Binary_Sensor_Get_State_Func)(void *args);

/**
 * @brief Create a new HAMQTT binary sensor.
 *
 * @param config Pointer to a binary sensor configuration. Must remain valid for the lifetime of the binary sensor.
 * @param get_state_func Function pointer for retrieving the state of a binary sensor. (See @ref HAMQTT_Binary_Sensor_Get_State_Func).
 * @param get_state_func_args A pointer to the arguments to be passed to the `get_state_func`.
 * @return Pointer to the created HAMQTT_Binary_Sensor, or NULL on failure.
 * 
 * @memberof HAMQTT_Binary_Sensor
 */
HAMQTT_Binary_Sensor *hamqtt_binary_sensor_create(HAMQTT_Binary_Sensor_Config *config, HAMQTT_Binary_Sensor_Get_State_Func get_state_func, void *get_state_func_args);

/**
 * @brief Destroy a HAMQTT binary sensor and free all resources.
 *
 * @param sensor Pointer to the binary sensor to destroy. Must not be NULL.
 * 
 * @memberof HAMQTT_Binary_Sensor
 */
void hamqtt_binary_sensor_destroy(HAMQTT_Binary_Sensor *sensor);

/**
 * @brief Get the configuration used to initialize the binary sensor.
 *
 * @param sensor Pointer to the binary sensor.
 * @return Pointer to the original configuration struct.
 * 
 * @memberof HAMQTT_Binary_Sensor
 */
const HAMQTT_Binary_Sensor_Config *hamqtt_binary_sensor_get_config(const HAMQTT_Binary_Sensor *sensor);

