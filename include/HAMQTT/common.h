#pragma once

#include "cJSON.h"

#include "esp_system.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "mqtt_client.h"

#define HAMQTT_DEVICE_MAX_COMPONENTS 16
#define HAMQTT_COMPONENT_MAX_TOPICS 32
#define HAMQTT_CHAR_BUF_SIZE 64

#define HAMQTT_MQTT_CONNECT_TIMEOUT_MS 10000