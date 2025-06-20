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
 * @file common.h
 * @brief Common macros and definitions used across the HAMQTT library.
 *
 * This header provides utility macros, constants, and shared definitions
 * used by multiple components of the HAMQTT framework. It ensures consistency
 * and reduces duplication across the codebase.
 *
 * This file is included internally by other headers and source files and
 * should not typically be included directly by user applications.
 *
 * @author Ethan Barnes
 * @date 2025
 * @copyright Apache License 2.0
 */

#pragma once

#include <stdbool.h>

#include "cJSON.h"

#include "esp_system.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "mqtt_client.h"

#include "sdkconfig.h"

#define HAMQTT_DEVICE_MAX_COMPONENTS CONFIG_HAMQTT_DEVICE_MAX_COMPONENTS
#define HAMQTT_MAX_CHAR_BUF_SIZE CONFIG_HAMQTT_MAX_CHAR_BUF_SIZE

#define HAMQTT_MQTT_CONNECT_TIMEOUT_MS CONFIG_HAMQTT_MQTT_CONNECT_TIMEOUT_MS