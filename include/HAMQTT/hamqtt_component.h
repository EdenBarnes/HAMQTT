#pragma once

typedef struct HAMQTT_Component HAMQTT_Component;

/* ----- V-Table (interface) ----- */

typedef struct {
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

} HAMQTT_Component_VTable;

/* ----- Base Object ----- */

struct HAMQTT_Component {
    const HAMQTT_Component_VTable *v;

    const char *subscribed_topics[HAMQTT_COMPONENT_MAX_TOPICS];
    size_t      subscribed_topics_count;
};

/* ----- Inline dispatch helpers ----- */

static inline esp_err_t hamqtt_component_get_discovery_config(
        HAMQTT_Component *c, cJSON *root, const char *device_uid)
{
    return c->v->get_discovery_config(c, root, device_uid);
}

static inline void hamqtt_component_handle_mqtt_message(
        HAMQTT_Component *c, const char *topic, int topic_len, const char *data, int data_len)
{
    c->v->handle_mqtt_message(c, topic, topic_len, data, data_len);
}

static inline void hamqtt_component_update(
        HAMQTT_Component *c, esp_mqtt_client_handle_t client)
{
    c->v->update(c, client);
}

static inline const char *hamqtt_component_get_unique_id(
        HAMQTT_Component *c)
{
    return c->v->get_unique_id(c);
}

static inline const char * const *hamqtt_component_get_subscribed_topics(
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