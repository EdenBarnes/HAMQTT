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
#include "HAMQTT/hamqtt_component_internal.h"

static const char *TAG = "HAMQTT_Binary_Sensor";

HAMQTT_Binary_Sensor_Config hamqtt_binary_sensor_config_default(void) {
    HAMQTT_Binary_Sensor_Config config = {
        .device_class = NULL,
        .enabled_by_default = true,
        .entity_picture = NULL,
        .expire_after = -1,
        .force_update = false,
        .icon = NULL,
        .name = "ESP32 Binary Sensor",
        .off_delay = -1,
        .unique_id = NULL
    };
    return config;
}

struct HAMQTT_Binary_Sensor {
    HAMQTT_Component base;

    HAMQTT_Binary_Sensor_Config *component_config;
    HAMQTT_Binary_Sensor_Get_State_Func get_state_func;
    void *get_state_func_args;

    bool has_sent_state;
    bool previous_state;
    char state_topic[HAMQTT_CHAR_BUF_SIZE];
};

/* ----- Virtual Methods ----- */

static esp_err_t hamqtt_binary_sensor_get_discovery_config(HAMQTT_Component *component, 
                                                           cJSON *root,
                                                           const char *device_unique_id) {
    HAMQTT_Binary_Sensor *sensor = (HAMQTT_Binary_Sensor *)component;

    // TODO : Finish
}

static void hamqtt_binary_sensor_handle_mqtt_message(HAMQTT_Component *component,
                                                     const char *topic,
                                                     int topic_len,
                                                     const char *data,
                                                     int data_len) {
    HAMQTT_Binary_Sensor *sensor = (HAMQTT_Binary_Sensor *)component;

    // TODO : Finish
}

static void hamqtt_binary_sensor_update(HAMQTT_Component *component,
                                        esp_mqtt_client_handle_t mqtt_client) {
    HAMQTT_Binary_Sensor *sensor = (HAMQTT_Binary_Sensor *)component;

    // TODO : Finish
}

static const char hamqtt_binary_sensor_get_unique_id(HAMQTT_Component *component) {
    HAMQTT_Binary_Sensor *sensor = (HAMQTT_Binary_Sensor *)component;

    // TODO : Finish
}

/* ----- V-Table ----- */
static const HAMQTT_Component_VTable binary_sensor_vtable = {
    .get_discovery_config = hamqtt_binary_sensor_get_discovery_config,
    .handle_mqtt_message = hamqtt_binary_sensor_handle_mqtt_message,
    .update = hamqtt_binary_sensor_update,
    .get_unique_id = hamqtt_binary_sensor_get_unique_id,
    .get_subscribed_topics = NULL
};

/* ----- Private HAMQTT Binary Sensor function declarations ----- */

/**
 * @brief Validates that the minimum required fields are populated in the configuration.
 *
 * Required fields: `unique_id`, and `name`.
 *
 * @param sensor The binary sensor to validate.
 * @return true if valid, false otherwise.
 */
static bool hamqtt_binary_sensor_is_config_valid(const HAMQTT_Binary_Sensor *sensor);

/* ----- HAMQTT Binary Sensor function definitions ----- */

HAMQTT_Binary_Sensor *hamqtt_binary_sensor_create(HAMQTT_Binary_Sensor_Config *config,
                                                  HAMQTT_Binary_Sensor_Get_State_Func get_state_func,
                                                  void *get_state_func_args) {
    HAMQTT_Binary_Sensor *sensor = malloc(sizeof(HAMQTT_Binary_Sensor));
    if (!sensor) {
        ESP_LOGE(TAG, "Unable to allocate space for HAMQTT Binary Sensor");
        return NULL;
    }

    sensor->base.v = &binary_sensor_vtable;
    sensor->component_config = config;
    sensor->get_state_func = get_state_func;
    sensor->get_state_func_args = get_state_func_args;
    sensor->has_sent_state = false;
    sensor->previous_state = false;
    
    if (!hamqtt_binary_sensor_is_config_valid(sensor)) {
        ESP_LOGW(TAG, "Binary Sensor config is missing required fields");
    }

    return sensor;
}

void hamqtt_binary_sensor_destroy(HAMQTT_Binary_Sensor *sensor) {
    if (!sensor) return;

    free(sensor);
}

const HAMQTT_Binary_Sensor_Config *hamqtt_binary_sensor_get_config(const HAMQTT_Binary_Sensor *sensor) {
    return sensor->component_config;
}
