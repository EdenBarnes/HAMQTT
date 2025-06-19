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
 * @file hamqtt_button.h
 * @brief HAMQTT Button component interface for Home Assistant MQTT discovery.
 *
 * Declares the interface and configuration for button components within the 
 * HAMQTT MQTT integration framework for Home Assistant.
 *
 * @author Ethan Barnes
 * @date 2025
 * @copyright Apache License 2.0
 */

#pragma once

#include "common.h"

/**
 * @brief Configuration parameters for a HAMQTT button.
 */
typedef struct {
    char *device_class;         ///< The class of the button, which alters how its state and icon is rendered. @note Optional.
    bool enabled_by_default;    ///< Set to `false` if the button should not be enabled when the button is first added.
    char *entity_picture;       ///< URL to a picture for the button. @note Optional.
    char *icon;                 ///< Icon for the button. @note Optional.
    char *name;                 ///< The name of the button shown in Home Assistant.
    char *unique_id;            ///< Unique identifier used for discovery in Home Assistant.
} HAMQTT_Button_Config;

/**
 * @brief Returns a default-initialized button configuration.
 *
 * Caller is still responsible for assigning unique_id
 * 
 * @return A default-initialized HAMQTT_Button_Config struct.
 * 
 * @memberof HAMQTT_Button_Config
 */
HAMQTT_Button_Config hamqtt_button_config_default(void);

/**
 * @struct HAMQTT_Button
 * @brief Internal representation of a Home Assistant MQTT button.
 */ 
typedef struct HAMQTT_Button HAMQTT_Button;

/**
 * @typedef HAMQTT_Button_On_Press_Func
 * @brief Function pointer type for receiving button press events.
 *
 * This function pointer is called every the button is pressed.
 *
 * @param args A pointer to user-defined arguments or context data required to handle the button press.
 *             This is typically a pointer to the button-specific data structure.
 */
typedef void (*HAMQTT_Button_On_Press_Func)(void *args);

/**
 * @brief Create a new HAMQTT button.
 *
 * @param config Pointer to a button configuration. Must remain valid for the lifetime of the button.
 * @param on_press_func Function pointer for handling pressing the ubtton. (See @ref HAMQTT_Button_On_Press_Func).
 * @param on_press_func_args A pointer to the arguments to be passed to the `on_press_func`.
 * @return Pointer to the created HAMQTT_Button, or NULL on failure.
 * 
 * @memberof HAMQTT_Button
 */
HAMQTT_Button *hamqtt_button_create(HAMQTT_Button_Config *config, HAMQTT_Button_On_Press_Func on_press_func, void *on_press_func_args);

/**
 * @brief Destroy a HAMQTT button and free all resources.
 *
 * @param button Pointer to the button to destroy. Must not be NULL.
 * 
 * @memberof HAMQTT_Button
 */
void hamqtt_button_destroy(HAMQTT_Button *button);

/**
 * @brief Get the configuration used to initialize the button.
 *
 * @param button Pointer to the button.
 * @return Pointer to the original configuration struct.
 * 
 * @memberof HAMQTT_Button
 */
const HAMQTT_Button_Config *hamqtt_button_get_config(const HAMQTT_Button *button);

