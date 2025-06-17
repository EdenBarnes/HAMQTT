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
 * @file hamqtt_binary_sensor.c
 * @brief Implementation of the HAMQTT_Binary_Sensor component.
 *
 * Contains logic for setting state and generating discovery/configuration
 * messages for binary sensors within the HAMQTT framework for Home Assistant MQTT integration.
 *
 * Implements the interface defined in @ref hamqtt_binary_sensor.h.
 *
 * @author Ethan Barnes
 * @date 2025
 * @copyright Apache License 2.0
 */

#include "HAMQTT/hamqtt_binary_sensor.h"