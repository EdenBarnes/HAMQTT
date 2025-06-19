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
 * @file hamqtt_component.c
 * @brief Integration for Home Assistant MQTT components.
 *
 * This file implements the dispatch helpers for `HAMQTT_Component`.
 *
 * @author Ethan Barnes
 * @date 2025
 * @copyright Apache License 2.0
 */

#include "HAMQTT/hamqtt_component_internal.h"

/* ----- Dispatch helpers ----- */

esp_err_t hamqtt_component_get_discovery_config(
        HAMQTT_Component *c, cJSON *root, const char *device_uid)
{
    return c->v->get_discovery_config(c, root, device_uid);
}

void hamqtt_component_handle_mqtt_message(
        HAMQTT_Component *c, const char *topic, const char *data)
{
    c->v->handle_mqtt_message(c, topic, data);
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