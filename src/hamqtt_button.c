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
 * @file hamqtt_button.c
 * @brief Implementation of the HAMQTT_Button component.
 *
 * Provides functions for initializing, configuring, and publishing button events
 * via MQTT for integration with Home Assistant. This file implements the interface
 * declared in @ref hamqtt_button.h.
 *
 * @author Ethan Barnes
 * @date 2025
 * @copyright Apache License 2.0
 */

#include "HAMQTT/hamqtt_button.h"
#include "HAMQTT/hamqtt_component_internal.h"

static const char* TAG = "HAMQTT_Button";

HAMQTT_Button_Config hamqtt_button_config_default(void) {
    HAMQTT_Button_Config config = {
        .device_class = NULL,
        .enabled_by_default = true,
        .entity_picture = NULL,
        .icon = NULL,
        .name = "ESP32 Button",
        .unique_id = NULL
    };
    return config;
}

struct HAMQTT_Button {
    HAMQTT_Component base;

    HAMQTT_Button_Config *component_config;
    HAMQTT_Button_On_Press_Func on_press_func;
    void *on_press_func_args;

    char *command_topic;
    const char *subscribed_topics[1];
};

/* ----- Private HAMQTT Button function declarations ----- */

/**
 * @brief Validates that the minimum required fields are populated in the configuration.
 *
 * Required fields: `name` and `unique_id`.
 *
 * @param button The button to validate.
 * @return true if valid, false otherwise.
 */
static bool hamqtt_button_is_config_valid(const HAMQTT_Button *button);

/* ----- Virtual Methods ----- */

static esp_err_t hamqtt_button_get_discovery_config(HAMQTT_Component *component, 
                                                    cJSON *root,
                                                    const char *device_unique_id) {
    HAMQTT_Button *button = (HAMQTT_Button *)component;

    ESP_RETURN_ON_FALSE(hamqtt_button_is_config_valid(button),
                        ESP_ERR_INVALID_STATE,
                        TAG,
                        "Button was used despite config missing required fields");
                                                    
    size_t command_topic_size = strlen(device_unique_id)
                          + strlen(button->component_config->unique_id)
                          + 1 /* slash */ + 6 /* "/press" */
                          + 1; /* NUL */
    
    if (button->command_topic) free(button->command_topic);
    button->command_topic = malloc(command_topic_size);
    ESP_RETURN_ON_FALSE(button->command_topic,
                        ESP_ERR_NO_MEM,
                        TAG, 
                        "Unable to allocate space for HAMQTT Button command topic");

    snprintf(button->command_topic, command_topic_size,
             "%s/%s/press", device_unique_id,
             button->component_config->unique_id);

    button->subscribed_topics[0] = button->command_topic;

    // Build configuration on root
    cJSON_AddStringToObject(root, "p", "button");
    cJSON_AddStringToObject(root, "name", button->component_config->name);
    cJSON_AddStringToObject(root, "command_topic", button->command_topic);
    cJSON_AddStringToObject(root, "unique_id", button->component_config->unique_id);
    cJSON_AddBoolToObject(root, "enabled_by_default", button->component_config->enabled_by_default);
    if (button->component_config->device_class) cJSON_AddStringToObject(root, "device_class", button->component_config->device_class);
    if (button->component_config->entity_picture) cJSON_AddStringToObject(root, "entity_picture", button->component_config->entity_picture);
    if (button->component_config->icon) cJSON_AddStringToObject(root, "icon", button->component_config->icon);

    return ESP_OK;
}

static void hamqtt_button_handle_mqtt_message(HAMQTT_Component *component,
                                              const char *topic,
                                              const char *data) {
    HAMQTT_Button *button = (HAMQTT_Button *)component;

    if (!button->command_topic) return;

    if (strcmp(topic, button->command_topic) != 0) return; // Sanity check
    if (strcmp(data, "PRESS") != 0) return;

    if (!button->on_press_func) {
        ESP_LOGE(TAG, "Button is missing on_press_func");
        return;
    }

    button->on_press_func(button->on_press_func_args);
}

static void hamqtt_button_update(HAMQTT_Component *component,
                                 esp_mqtt_client_handle_t mqtt_client) {
    // DO NOTHING
}

static const char *hamqtt_button_get_unique_id(HAMQTT_Component *component) {
    HAMQTT_Button *button = (HAMQTT_Button *)component;

    return button->component_config->unique_id;
}

static const char *const *hamqtt_button_get_subscribed_topics(HAMQTT_Component *component, size_t *count) {
    HAMQTT_Button *button = (HAMQTT_Button *)component;

    *count = 1;

    return button->subscribed_topics;
}

/* ----- V-Table ----- */
static const HAMQTT_Component_VTable button_vtable = {
    .get_discovery_config = hamqtt_button_get_discovery_config,
    .handle_mqtt_message = hamqtt_button_handle_mqtt_message,
    .update = hamqtt_button_update,
    .get_unique_id = hamqtt_button_get_unique_id,
    .get_subscribed_topics = hamqtt_button_get_subscribed_topics
};

/* ----- HAMQTT Button function definitions ----- */

static bool hamqtt_button_is_config_valid(const HAMQTT_Button *button) {
    if (!button->component_config->name) return false;
    if (!button->component_config->unique_id) return false;

    return true;
}

HAMQTT_Button *hamqtt_button_create(HAMQTT_Button_Config *config,
                                    HAMQTT_Button_On_Press_Func on_press_func,
                                    void *on_press_func_args) {
    HAMQTT_Button *button = calloc(1, sizeof(HAMQTT_Button));
    if (!button) {
        ESP_LOGE(TAG, "Unable to allocate space for HAMQTT Button");
        return NULL;
    }

    button->base.v = &button_vtable;
    button->component_config = config;
    button->on_press_func = on_press_func;
    button->on_press_func_args = on_press_func_args;

    if (!hamqtt_button_is_config_valid(button)) {
        ESP_LOGE(TAG, "Button config is missing required fields");
        free(button);
        return NULL;
    }

    return button;
}

void hamqtt_button_destroy(HAMQTT_Button *button) {
    if (!button) return;
    if (button->command_topic) free(button->command_topic);

    free(button);
}

const HAMQTT_Button_Config *hamqtt_button_get_config(const HAMQTT_Button *button) {
    return button->component_config;
}