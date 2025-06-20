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
    char *state_topic;
};

/* ----- Private HAMQTT Binary Sensor function declarations ----- */

/**
 * @brief Validates that the minimum required fields are populated in the configuration.
 *
 * Required fields: `name` and `unique_id`.
 *
 * @param sensor The binary sensor to validate.
 * @return true if valid, false otherwise.
 */
static bool hamqtt_binary_sensor_is_config_valid(const HAMQTT_Binary_Sensor *sensor);

/* ----- Virtual Methods ----- */

static esp_err_t hamqtt_binary_sensor_get_discovery_config(HAMQTT_Component *component, 
                                                           cJSON *root,
                                                           const char *device_unique_id) {
    HAMQTT_Binary_Sensor *sensor = (HAMQTT_Binary_Sensor *)component;

    ESP_RETURN_ON_FALSE(hamqtt_binary_sensor_is_config_valid(sensor),
                        ESP_ERR_INVALID_STATE,
                        TAG,
                        "Binary sensor was used despite config missing required fields");

    size_t state_topic_size = strlen(device_unique_id)
                          + strlen(sensor->component_config->unique_id)
                          + 1 /* slash */ + 6 /* "/state" */
                          + 1; /* NUL */

    if (sensor->state_topic) free(sensor->state_topic);
    sensor->state_topic = malloc(state_topic_size);
    ESP_RETURN_ON_FALSE(sensor->state_topic,
                        ESP_ERR_NO_MEM,
                        TAG,
                        "Unable to allocate space for HAMQTT Binary Sensor state topic");

    snprintf(sensor->state_topic,
             state_topic_size,
             "%s/%s/state", device_unique_id,
             sensor->component_config->unique_id);

    // Build configuration on root
    cJSON_AddStringToObject(root, "p", "binary_sensor");
    cJSON_AddStringToObject(root, "name", sensor->component_config->name);
    cJSON_AddStringToObject(root, "state_topic", sensor->state_topic);
    cJSON_AddStringToObject(root, "unique_id", sensor->component_config->unique_id);
    cJSON_AddBoolToObject(root, "enabled_by_default", sensor->component_config->enabled_by_default);
    cJSON_AddBoolToObject(root, "force_update", sensor->component_config->force_update);
    if (sensor->component_config->device_class) cJSON_AddStringToObject(root, "device_class", sensor->component_config->device_class);
    if (sensor->component_config->entity_picture) cJSON_AddStringToObject(root, "entity_picture", sensor->component_config->entity_picture);
    if (sensor->component_config->icon) cJSON_AddStringToObject(root, "icon", sensor->component_config->icon);
    if (sensor->component_config->expire_after != -1) cJSON_AddNumberToObject(root, "expire_after", sensor->component_config->expire_after);
    if (sensor->component_config->off_delay != -1) cJSON_AddNumberToObject(root, "off_delay", sensor->component_config->off_delay);

    return ESP_OK;
}

static void hamqtt_binary_sensor_handle_mqtt_message(HAMQTT_Component *component,
                                                     const char *topic,
                                                     const char *data) {
    // DO NOTHING
}

static void hamqtt_binary_sensor_update(HAMQTT_Component *component,
                                        esp_mqtt_client_handle_t mqtt_client) {
    HAMQTT_Binary_Sensor *sensor = (HAMQTT_Binary_Sensor *)component;

    if (!sensor->get_state_func) {
        ESP_LOGE(TAG, "Binary sensor is missing get_state_func");
        return;
    }

    bool current_state = sensor->get_state_func(sensor->get_state_func_args);

    if (current_state == sensor->previous_state && sensor->has_sent_state) return;
    sensor->has_sent_state = true;
    sensor->previous_state = current_state;

    esp_mqtt_client_publish(mqtt_client, sensor->state_topic, current_state ? "ON" : "OFF", 0, 1, 1);
}

static const char *hamqtt_binary_sensor_get_unique_id(HAMQTT_Component *component) {
    HAMQTT_Binary_Sensor *sensor = (HAMQTT_Binary_Sensor *)component;

    return sensor->component_config->unique_id;
}

static const char *const *hamqtt_binary_sensor_get_subscribed_topics(HAMQTT_Component *component, size_t *count) {
    *count = 0;
    return NULL;
}

/* ----- V-Table ----- */
static const HAMQTT_Component_VTable binary_sensor_vtable = {
    .get_discovery_config = hamqtt_binary_sensor_get_discovery_config,
    .handle_mqtt_message = hamqtt_binary_sensor_handle_mqtt_message,
    .update = hamqtt_binary_sensor_update,
    .get_unique_id = hamqtt_binary_sensor_get_unique_id,
    .get_subscribed_topics = hamqtt_binary_sensor_get_subscribed_topics
};

/* ----- HAMQTT Binary Sensor function definitions ----- */

static bool hamqtt_binary_sensor_is_config_valid(const HAMQTT_Binary_Sensor *sensor) {
    if (!sensor->component_config->name) return false;
    if (!sensor->component_config->unique_id) return false;

    return true;
}

HAMQTT_Binary_Sensor *hamqtt_binary_sensor_create(HAMQTT_Binary_Sensor_Config *config,
                                                  HAMQTT_Binary_Sensor_Get_State_Func get_state_func,
                                                  void *get_state_func_args) {
    HAMQTT_Binary_Sensor *sensor = calloc(1, sizeof(HAMQTT_Binary_Sensor));
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
        ESP_LOGE(TAG, "Binary Sensor config is missing required fields");
        free(sensor);
        return NULL;
    }

    return sensor;
}

void hamqtt_binary_sensor_destroy(HAMQTT_Binary_Sensor *sensor) {
    if (!sensor) return;
    if (sensor->state_topic) free(sensor->state_topic);

    free(sensor);
}

const HAMQTT_Binary_Sensor_Config *hamqtt_binary_sensor_get_config(const HAMQTT_Binary_Sensor *sensor) {
    return sensor->component_config;
}
