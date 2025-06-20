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
 * @file HAMQTT.h
 * @brief Main public API header for the HAMQTT library.
 *
 * This is the primary include file for users of the HAMQTT framework.
 * It provides access to all components and functionality required to integrate
 * custom devices with Home Assistant via MQTT using ESP-IDF.
 *
 * Including this file will automatically include all core component headers,
 * allowing for simplified integration in user applications.
 *
 * @author Ethan Barnes
 * @date 2025
 * @copyright Apache License 2.0
 */

#pragma once

#include "HAMQTT/hamqtt_device.h"

// Components
#include "HAMQTT/hamqtt_binary_sensor.h"
#include "HAMQTT/hamqtt_button.h"
