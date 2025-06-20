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
 * @file hamqtt_device.c
 * @brief Implementation of the HAMQTT_Device MQTT device abstraction.
 *
 * This file defines the internal structure and logic of the `HAMQTT_Device`,
 * including device metadata handling, MQTT discovery publishing, message routing,
 * and lifecycle management of components.
 *
 * This is the core implementation used by device applications integrating with
 * Home Assistant's MQTT discovery protocol.
 *
 * @author Ethan Barnes
 * @date 2025
 * @copyright Apache License 2.0
 */

#include "HAMQTT/hamqtt_device.h"

#define MQTT_CONNECTED_BIT BIT0

static const char *TAG = "HAMQTT_Device";

HAMQTT_Device_Config hamqtt_device_config_default(void) {
    HAMQTT_Device_Config config = {
        .mqtt_config_topic_prefix = "homeassistant",
        .mqtt_uri = NULL,
        .mqtt_username = NULL,
        .mqtt_password = NULL,
        .manufacturer = NULL,
        .model = NULL,
        .serial_number = NULL,
        .unique_id = NULL,
        .sw_version = NULL,
        .hw_version = NULL,
        .origin_url = NULL,
        .name = "ESP32 Device"
    };
    return config;
}

struct HAMQTT_Device {
    HAMQTT_Device_Config *device_config;

    HAMQTT_Component *components[HAMQTT_DEVICE_MAX_COMPONENTS];
    int component_count;

    char availability_topic[HAMQTT_CHAR_BUF_SIZE];

    esp_mqtt_client_handle_t mqtt_client;
    EventGroupHandle_t mqtt_event_group;
};

/* ----- Private HAMQTT Device function declarations ----- */

/**
 * @brief Validates that the minimum required fields are populated in the configuration.
 *
 * Required fields: `mqtt_config_topic_prefix`, `mqtt_uri`, `unique_id`, and `name`.
 *
 * @param device The device to validate.
 * @return true if valid, false otherwise.
 */
static bool hamqtt_device_is_config_valid(const HAMQTT_Device *device);

/**
 * @brief Builds the full Home Assistant discovery config JSON for the device and its components.
 *
 * @param[in] device The device whose configuration to serialize.
 * @param[out] root The output cJSON object to populate.
 * @return ESP_OK on success, or appropriate error on failure.
 */
static esp_err_t hamqtt_device_build_config(const HAMQTT_Device *device, cJSON* root);

/**
 * @brief Subscribes to all topics requested by each registered component.
 *
 * @param[in] device The device whose components should be subscribed.
 */
static void hamqtt_device_subscribe(const HAMQTT_Device *device);

/**
 * @brief Callback handler for all MQTT client events.
 *
 * @param handler_args Pointer to the HAMQTT_Device.
 * @param base Event base.
 * @param event_id Event ID.
 * @param event_data Pointer to the event data.
 */
static void hamqtt_device_mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

/**
 * @brief Determines if any component is subscribed to a given topic and dispatches the message to it.
 *
 * @param[in] device The device instance.
 * @param[in] topic Topic string.
 * @param[in] topic_len Length of the topic.
 * @param[in] data Payload data.
 * @param[in] data_len Length of the payload.
 */
void hamqtt_device_handle_mqtt_message(const HAMQTT_Device *device, const char *topic, int topic_len, const char *data, int data_len);

/* ----- HAMQTT Device function definitions ----- */

HAMQTT_Device *hamqtt_device_create(HAMQTT_Device_Config *config){
    HAMQTT_Device *device = malloc(sizeof(HAMQTT_Device));
    if (!device) {
        ESP_LOGE(TAG, "Unable to allocate space for HAMQTT Device");
        return NULL;
    }

    device->device_config = config;
    device->component_count = 0;
    device->mqtt_client = NULL;

    if (!hamqtt_device_is_config_valid(device)) {
        ESP_LOGW(TAG, "Device config is missing required fields");
    }

    return device;
}

void hamqtt_device_destroy(HAMQTT_Device *device) {
    if (!device) return;

    free(device);
}

esp_err_t hamqtt_device_add_component(HAMQTT_Device *device, HAMQTT_Component *component) {
    ESP_RETURN_ON_FALSE(component,
                        ESP_ERR_INVALID_ARG,
                        TAG,
                        "Attempted to add component before it was initialized");

    ESP_RETURN_ON_FALSE(device->component_count <= HAMQTT_DEVICE_MAX_COMPONENTS,
                        ESP_ERR_NO_MEM,
                        TAG,
                        "Component buffer is full! No more than %d components can be added",
                        HAMQTT_DEVICE_MAX_COMPONENTS);

    device->components[device->component_count] = component;
    device->component_count++;

    return ESP_OK;
}

esp_err_t hamqtt_device_connect(HAMQTT_Device *device) {
    // Get HomeAssistant configuration
    cJSON* ha_dev_config_json = cJSON_CreateObject();
    ESP_RETURN_ON_ERROR(hamqtt_device_build_config(device, ha_dev_config_json), TAG, "Failed to build HomeAssistant configuration");

    char *ha_dev_config_str;
    ha_dev_config_str = cJSON_Print(ha_dev_config_json);

    // Create MQTT config
    esp_mqtt_client_config_t mqtt_config = {};
    mqtt_config.broker.address.uri = device->device_config->mqtt_uri;

    if (device->device_config->mqtt_username) mqtt_config.credentials.username = device->device_config->mqtt_username;
    if (device->device_config->mqtt_password) mqtt_config.credentials.authentication.password = device->device_config->mqtt_password;

    mqtt_config.session.last_will.topic = device->availability_topic;
    mqtt_config.session.last_will.msg = "offline";
    mqtt_config.session.last_will.qos = 1;
    mqtt_config.session.last_will.retain = 1;

    // Connect to the mqtt broker
    device->mqtt_event_group = xEventGroupCreate();

    device->mqtt_client = esp_mqtt_client_init(&mqtt_config);
    esp_mqtt_client_register_event(device->mqtt_client, ESP_EVENT_ANY_ID, hamqtt_device_mqtt_event_handler, device);

    ESP_RETURN_ON_ERROR(esp_mqtt_client_start(device->mqtt_client), TAG, "Failed ot start MQTT Client");

    // Wait for connection
    EventBits_t bits = xEventGroupWaitBits(
        device->mqtt_event_group,
        MQTT_CONNECTED_BIT,
        pdFALSE,
        pdFALSE,
        pdMS_TO_TICKS(HAMQTT_MQTT_CONNECT_TIMEOUT_MS)
    );

    ESP_RETURN_ON_FALSE(bits & MQTT_CONNECTED_BIT, ESP_FAIL, TAG, "MQTT Failed to connect within timeout");

    // Publish the config to MQTT Broker
    ESP_LOGI(TAG, "Publishing Configuration");
    
    char config_topic[HAMQTT_CHAR_BUF_SIZE];
    snprintf(config_topic,
             sizeof(config_topic),
             "%s/device/%s/config",
             device->device_config->mqtt_config_topic_prefix,
             device->device_config->unique_id);
    
    esp_mqtt_client_publish(device->mqtt_client, config_topic, ha_dev_config_str, strlen(ha_dev_config_str), 1, 1);

    cJSON_Delete(ha_dev_config_json);

    return ESP_OK;
}

esp_err_t hamqtt_device_publish_availability(const HAMQTT_Device *device, bool availability) {
    ESP_RETURN_ON_FALSE(device->mqtt_client, ESP_ERR_INVALID_STATE, TAG, "Tried to publish availability before MQTT connection was created");

    if (availability) {
        esp_mqtt_client_publish(device->mqtt_client, device->availability_topic, "online", 0, 1, 1);
    } else {
        esp_mqtt_client_publish(device->mqtt_client, device->availability_topic, "offline", 0, 1, 1);
    }

    return ESP_OK;
}

void hamqtt_device_loop(const HAMQTT_Device *device) {
    for (int i = 0; i < device->component_count; ++i) {
        HAMQTT_Component *component = device->components[i];
        hamqtt_component_update(component, device->mqtt_client);
    }
}

const HAMQTT_Device_Config *hamqtt_device_get_config(const HAMQTT_Device *device) {
    return device->device_config;
}

bool hamqtt_device_is_config_valid(const HAMQTT_Device *device) {
    if (!device->device_config->mqtt_config_topic_prefix) return false;
    if (!device->device_config->mqtt_uri) return false;
    if (!device->device_config->unique_id) return false;
    if (!device->device_config->name) return false;

    return true;
}

esp_err_t hamqtt_device_build_config(const HAMQTT_Device *device, cJSON* root) {
    ESP_RETURN_ON_FALSE(hamqtt_device_is_config_valid(device), ESP_ERR_INVALID_STATE, TAG, "Some required fields are missing");

    // Set availability topic
    snprintf(device->availability_topic, sizeof(device->availability_topic), "%s/availability", device->device_config->unique_id);

    // Build HomeAssistant config
    ESP_LOGI(TAG, "Building Configuration");

    // Device Config
    cJSON *device_json = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "device", device_json);

    cJSON_AddStringToObject(device_json, "ids", device->device_config->unique_id);
    cJSON_AddStringToObject(device_json, "name", device->device_config->name);
    if (device->device_config->manufacturer) cJSON_AddStringToObject(device_json, "mf", device->device_config->manufacturer);
    if (device->device_config->model) cJSON_AddStringToObject(device_json, "mdl", device->device_config->model);
    if (device->device_config->sw_version) cJSON_AddStringToObject(device_json, "sw", device->device_config->sw_version);
    if (device->device_config->hw_version) cJSON_AddStringToObject(device_json, "hw", device->device_config->hw_version);
    if (device->device_config->serial_number) cJSON_AddStringToObject(device_json, "sn", device->device_config->serial_number);

    // Origin Config
    cJSON *origin_json = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "origin", origin_json);

    cJSON_AddStringToObject(origin_json, "name", device->device_config->name);
    if (device->device_config->sw_version) cJSON_AddStringToObject(origin_json, "sw", device->device_config->sw_version);
    if (device->device_config->origin_url) cJSON_AddStringToObject(origin_json, "url", device->device_config->origin_url);

    // Component Config
    cJSON *components_json = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "cmps", components_json);

    for (size_t i = 0; i < device->component_count; ++i) {
        HAMQTT_Component *component = device->components[i];
        cJSON *component_json = cJSON_CreateObject();

        ESP_RETURN_ON_ERROR(hamqtt_component_get_discovery_config(component, component_json, device->device_config->unique_id),
                            TAG,
                            "Failed to get configuration of a component");

        const char *unique_id = hamqtt_component_get_unique_id(component);

        cJSON_AddItemToObject(components_json, unique_id, component_json);
    }

    // Availability Config
    cJSON_AddStringToObject(root, "availability_topic", device->availability_topic);

    cJSON_AddStringToObject(root, "qos", "1");

    return ESP_OK;
}

void hamqtt_device_subscribe(const HAMQTT_Device *device) {
    for (size_t i = 0; i < device->component_count; ++i) {
        HAMQTT_Component *component = device->components[i];

        size_t topic_count = 0;
        const char *const *topics = hamqtt_component_get_subscribed_topics(component, &topic_count);

        for (size_t j = 0; j < topic_count; ++j) {
            ESP_LOGI(TAG, "Subscribing to Topic %s", topics[j]);
            esp_mqtt_client_subscribe_single(device->mqtt_client, topics[j], 1);
        }
    }
}

void hamqtt_device_mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    HAMQTT_Device *device = (HAMQTT_Device *)handler_args;

    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;

    switch (event_id)
    {
    case MQTT_EVENT_BEFORE_CONNECT:
        ESP_LOGI(TAG, "MQTT Connecting");
        break;
    
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT Connected");
        xEventGroupSetBits(device->mqtt_event_group, MQTT_CONNECTED_BIT);

        ESP_LOGI(TAG, "Publishing As Available");
        hamqtt_device_publish_availability(device, true);

        ESP_LOGI(TAG, "Subscribing to Component Topics");
        hamqtt_device_subscribe(device);

        break; 

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGW(TAG, "MQTT Lost Connection");
        xEventGroupClearBits(device->mqtt_event_group, MQTT_CONNECTED_BIT);
        break;

    case MQTT_EVENT_DATA:
        hamqtt_device_handle_mqtt_message(device, event->topic, event->topic_len, event->data, event->data_len);
        break;

    default:
        break;
    }
}

void hamqtt_device_handle_mqtt_message(const HAMQTT_Device *device, const char *topic, int topic_len, const char *data, int data_len) {
    // The data must be converted into two new Cstrings, since `topic` and `data` are not guaranteed to end in a null char
    char topic_str[HAMQTT_CHAR_BUF_SIZE];
    char data_str[HAMQTT_CHAR_BUF_SIZE];

    memset(topic_str, 0, sizeof(topic_str));
    memset(data_str, 0, sizeof(data_str));

    memcpy(topic_str, topic, topic_len < HAMQTT_CHAR_BUF_SIZE - 1 ? topic_len : HAMQTT_CHAR_BUF_SIZE - 1);
    memcpy(data_str, data, data_len < HAMQTT_CHAR_BUF_SIZE - 1 ? data_len : HAMQTT_CHAR_BUF_SIZE - 1);
    
    ESP_LOGI(TAG, "MQTT Event Data Received");
    ESP_LOGI(TAG, "Topic: %s", topic_str);
    ESP_LOGI(TAG, "Data: %s", data_str);

    for (size_t i = 0; i < device->component_count; ++i) {
        HAMQTT_Component *component = device->components[i];

        // const char *unique_id = hamqtt_component_get_unique_id(component); TODO : Determine if I can remove this. I forget why its here

        size_t topic_count = 0;
        const char *const *topics = hamqtt_component_get_subscribed_topics(component, &topic_count);

        for (size_t j = 0; j < topic_count; ++j) {
            if (strcmp(topics[j], topic_str) == 0) {
                hamqtt_component_handle_mqtt_message(component, topic_str, data_str);
            }
        }
    }
}