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
 * @file hamqtt_component.h
 * @brief Interface definition for Home Assistant MQTT components.
 *
 * This header defines the abstract `HAMQTT_Component` interface and
 * the virtual dispatch function declarations for MQTT-based Home Assistant
 * components. Components implementing this interface must provide their own
 * VTable implementation for MQTT discovery, messaging, and updates.
 *
 * @author Ethan Barnes
 * @date 2025
 * @copyright Apache License 2.0
 */

#pragma once

#include "common.h"

typedef struct HAMQTT_Component HAMQTT_Component;

/* ----- Dispatch helpers ----- */

/**
 * @brief Populates the Home Assistant discovery configuration for the component.
 *
 * This function builds a JSON object containing discovery configuration information
 * for the specified component. This information is used by Home Assistant to
 * automatically set up the entity.
 *
 * @param component Pointer to the component instance.
 * @param root Pointer to the cJSON object to populate.
 * @param device_uid Unique ID of the parent device to associate with the component.
 * @return esp_err_t ESP_OK on success, or an appropriate error code on failure.
 * 
 * @memberof HAMQTT_Component
 */
esp_err_t hamqtt_component_get_discovery_config(
        HAMQTT_Component*component, cJSON *root, const char *device_uid);

/**
 * @brief Dispatches an incoming MQTT message to the specified component.
 *
 * This function should be called when an MQTT message is received for a topic
 * that the component has subscribed to. The component will parse the message
 * and act on it accordingly.
 *
 * @param component Pointer to the component instance.
 * @param topic The topic string.
 * @param topic_len Length of the topic string.
 * @param data The message payload.
 * @param data_len Length of the payload.
 * 
 * @memberof HAMQTT_Component
 */
void hamqtt_component_handle_mqtt_message(
        HAMQTT_Component *component, const char *topic, const char *data);

/**
 * @brief Performs a periodic update for the component.
 *
 * This function is typically called in the main loop to allow the component
 * to publish state changes or perform maintenance tasks.
 *
 * @param component Pointer to the component instance.
 * @param client MQTT client handle to use for publishing.
 * 
 * @memberof HAMQTT_Component
 */
void hamqtt_component_update(
        HAMQTT_Component *component, esp_mqtt_client_handle_t client);

/**
 * @brief Returns the unique ID of the component.
 *
 * This ID is used for discovery, topic namespacing, and internal lookup.
 *
 * @param component Pointer to the component instance.
 * @return const char* Unique ID string.
 * 
 * @memberof HAMQTT_Component
 */
const char *hamqtt_component_get_unique_id(
        HAMQTT_Component *component);

/**
 * @brief Returns the list of topics the component wants to subscribe to.
 *
 * The component uses these topics to receive control or data messages.
 *
 * @param component Pointer to the component instance.
 * @param count Pointer to a size_t to receive the number of topics.
 * @return const char* const* Array of topic strings.
 * 
 * @memberof HAMQTT_Component
 */
const char * const *hamqtt_component_get_subscribed_topics(
        HAMQTT_Component *component, size_t *count);