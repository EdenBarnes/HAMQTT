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

typedef struct {
    char *device_class;
    bool enabled_by_default;
    char *entity_picture;
    int expire_after;
    bool force_update;
    char *icon;
    char *name;
    int off_delay;
    char *unique_id;
} HAMQTT_Binary_Sensor_Config;

typedef struct HAMQTT_Binary_Sensor HAMQTT_Binary_Sensor;
typedef bool (*HAMQTT_Binary_Sensor_Get_State_Func)(void *args);

HAMQTT_Binary_Sensor *hamqtt_binary_sensor_create(HAMQTT_Binary_Sensor_Config *config, HAMQTT_Binary_Sensor_Get_State_Func get_state_func, void *get_state_func_args);