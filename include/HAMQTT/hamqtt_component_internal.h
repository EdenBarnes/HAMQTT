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
 * @file hamqtt_component_internal.h
 * @brief Internal declarations for the HAMQTT_Component system.
 *
 * This internal header defines the `HAMQTT_Component_VTable` structure used for
 * polymorphic behavior and the internal fields of the `HAMQTT_Component` base type.
 * This header should only be included by component implementations or core library code.
 *
 * @note This file is not part of the public API and may change without notice.
 * @author Ethan Barnes
 * @date 2025
 * @copyright Apache License 2.0
 */

#pragma once
#include "hamqtt_component.h"

typedef struct HAMQTT_Component_VTable HAMQTT_Component_VTable;

/* ----- V-Table (interface) ----- */

/**
 * @internal
 * @brief Virtual function table for HAMQTT_Component interface.
 *
 * Each function pointer provides a component-specific implementation of core behavior.
 */
struct HAMQTT_Component_VTable {
    esp_err_t (*get_discovery_config)(HAMQTT_Component *component, 
                                      cJSON *root,
                                      const char *device_unique_id);

    void (*handle_mqtt_message)(HAMQTT_Component *component,
                                const char *topic,
                                int topic_len,
                                const char *data,
                                int data_len);

    void (*update)(HAMQTT_Component *component,
                   esp_mqtt_client_handle_t mqtt_client);

    const char *(*get_unique_id)(HAMQTT_Component *component);
    
    const char *const *(*get_subscribed_topics)(HAMQTT_Component *component,
                                                size_t *count);

};

/* ----- Base Object ----- */

/**
 * @brief Base struct representing a generic Home Assistant MQTT component.
 */
struct HAMQTT_Component {
    const HAMQTT_Component_VTable *v;

    const char *subscribed_topics[HAMQTT_COMPONENT_MAX_TOPICS];
    size_t      subscribed_topics_count;
};

/* ----- Dispatch helpers ----- */

esp_err_t hamqtt_component_get_discovery_config(
        HAMQTT_Component *c, cJSON *root, const char *device_uid)
{
    return c->v->get_discovery_config(c, root, device_uid);
}

void hamqtt_component_handle_mqtt_message(
        HAMQTT_Component *c, const char *topic, int topic_len, const char *data, int data_len)
{
    c->v->handle_mqtt_message(c, topic, topic_len, data, data_len);
}

void hamqtt_component_update(
        HAMQTT_Component *c, esp_mqtt_client_handle_t client)
{
    c->v->update(c, client);
}

const char *hamqtt_component_get_unique_id(
        HAMQTT_Component *c)
{
    return c->v->get_unique_id(c);
}

const char * const *hamqtt_component_get_subscribed_topics(
        HAMQTT_Component *c, size_t *count)
{
    if (c->v->get_subscribed_topics)
        return c->v->get_subscribed_topics(c, count);

    if (c->subscribed_topics_count > 0) {
        *count = c->subscribed_topics_count;
        return c->subscribed_topics;
    }
    *count = 0;
    return NULL;
}