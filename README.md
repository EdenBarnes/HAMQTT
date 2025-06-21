<p align="center">
  <a href="https://github.com/EdenBarnes/HAMQTT" target="_blank" rel="noopener noreferrer">
    <img src="https://raw.githubusercontent.com/EdenBarnes/HAMQTT/refs/heads/main/docs/logo.svg" alt="EdenElectronics Logo" width="200">
  </a>
</p>

<h1 align="center">HAMQTT</h1>

<p align="center">
  <b>ESP-IDF component for integrating devices with Home Assistant over MQTT</b>
</p>

---

## Introduction
HAMQTT is a small C library that lets your ESP32 firmware expose sensors, buttons, and other components to Home Assistant over MQTT.

---

## Features

| Category     | Feature                                                                       |
| ------------ | ----------------------------------------------------------------------------- |
| Discovery    | Automatic generation & pubication of device and component discovery payloads. |
| Availability | Online / offline last-will handled for you                                    |
| Components   | `Binary_Sensor`, `Button` (more planned)                                      |
| Footprint    | ~2.9 KB flash / ~0 B static RAM (heap usage depends on component count)       |
| License      | Apache 2.0                                                                    |

[Click here for documentation](https://edenbarnes.github.io/HAMQTT/html/index.html)

---

## Roadmap to v1.0.0

> [!NOTE]
> These will be completed as I need them for my projects, and not necessarily in the order listed here.

| Status    | Component type       | Description                                  |
| --------- | --------------------| --------------------------------------------|
| ✅ Done    | **Binary Sensor**    | Basic On/Off                                |
| ✅ Done    | **Button**           | Basic pressable button                      |
| ⏳ Planned | **Switch**           | On/Off control + state                      |
| ⏳ Planned | **Sensor**           | Generic sensor (temperature, humidity, etc.)|
| ⏳ Planned | **Number**           | Publish numeric value / accept setpoint    |
| ⏳ Planned | **Select**           | Dropdown selection entity                   |
| ⏳ Planned | **Notify**           | Send notifications                          |
| ⏳ Planned | **Text**             | Publish / accept text values                |
| ⏳ Planned | **Tag Scanner**      | RFID/NFC tag reader                         |
| ⏳ Planned | **Alarm Control Panel** | Arm/disarm alarm system                    |
| ⏳ Planned | **Device Trigger**   | Trigger automations based on events        |
| ⏳ Planned | **Event**            | Publish custom event data                   |
| ⏳ Planned | **Device Tracker**   | Track device presence/location              |
| ⏳ Planned | **Cover**            | Open/close/stop (e.g., garage door, blinds)|
| ⏳ Planned | **Fan**              | Control fan speed and oscillation          |
| ⏳ Planned | **Humidifier**       | Control humidifier devices                  |
| ⏳ Planned | **Light**            | Brightness / RGB / color control            |
| ⏳ Planned | **Lock**             | Lock/unlock doors                           |
| ⏳ Planned | **Siren**            | Trigger audible alarms                      |
| ⏳ Planned | **Valve**            | Control valves for water/gas                |
| ⏳ Planned | **Water Heater**     | Manage water heater temperature/settings   |
| ⏳ Planned | **Camera**           | Stream or snapshot camera images            |
| ⏳ Planned | **Lawn Mower**       | Control robotic lawn mower                  |
| ⏳ Planned | **Scene**            | Activate preconfigured scenes               |
| ⏳ Planned | **Update**           | Firmware/software update over MQTT          |
| ⏳ Planned | **Vacuum**           | Control robotic vacuum cleaners             |

> **Release 1.0.0** will ship once most of the table above is ✅.

---

## Installation (ESP‑IDF v5.x)

``` bash
mkdir -p components
git submodule add https://github.com/EdenBarnes/HAMQTT.git components/HAMQTT
```

---

## Quick start (minimal example)

[Click here for documentation](https://edenbarnes.github.io/HAMQTT/html/index.html)

```c
#include "HAMQTT.h"

/* Your GPIO / sensor read function */
bool door_open_get_state(void *arg)
{
    // TODO: read GPIO or sensor
    return false; // placeholder
}

void app_main(void)
{
    // 1. Configure the device (MQTT broker, IDs, etc.)
    HAMQTT_Device_Config dev_cfg = hamqtt_device_config_default();
    dev_cfg.mqtt_uri   = "mqtt://<BROKER‑IP>";   // TODO
    dev_cfg.unique_id  = "esp32‑hall‑node";       // Must be unique per device
    dev_cfg.name       = "Hall Sensor Node";

    HAMQTT_Device *device = hamqtt_device_create(&dev_cfg);

    // 2. Add a binary‑sensor component
    HAMQTT_Binary_Sensor_Config bin_cfg = hamqtt_binary_sensor_config_default();
    bin_cfg.unique_id = "hall_door_state";
    bin_cfg.name      = "Hall Door";

    HAMQTT_Binary_Sensor *door = hamqtt_binary_sensor_create(&bin_cfg,
                                                             door_open_get_state,
                                                             NULL);
    hamqtt_device_add_component(device, (HAMQTT_Component *)door);

    // TODO: You must connect to WiFi before attempting to connect to the MQTT broker
    // 3. Connect to MQTT (blocks until connected & discovery published)
    hamqtt_device_connect(device);

    // 4. Main loop (call regularly or from freertos task)
    while (true) {
        hamqtt_device_loop(device);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
```

### What happens behind the scenes?

1. The device publishes its discovery config on
   ```
   <mqtt_config_topic_prefix>/device/<unique_id>/config
   ```
2. Each component publishes its own JSON in the `cmps` section.
3. The library subscribes to any command topics and schedules `update()` calls.
4. Home Assistant → MQTT automatically shows the entities.

---

## Contributing

1. Fork & clone the repo.
2. Create a topic branch: `git checkout -b feature/my‑new‑component`.
3. Follow the coding style in existing files.
4. Run `idf.py build` on an ESP32‑S3 or ESP32‑C3 target.
5. Open a PR.

Feedback and assistance on this project is greatly appreciated. If you have any ideas on how to improve this project, or simply want to tell me how I've done everything in the worst way possible, I'd love to hear it. HAMQTT is still in its infancy, and I am not shy to rewriting the entire thing for the sake of improving it.

If there’s a component missing from [the list above](#roadmap-to-v100) that you wish existed, feel free to implement it and submit a PR! I’ve tried to make the codebase self-explanatory and well-documented, but here’s a detailed guide to help you get started building your own component:

### Component Architecture

Every component in HAMQTT is defined by a C `struct` that holds its internal state and behavior. To integrate with the framework, it must follow a few conventions.

#### 1. Start with a base component
Your component's struct **must** have a `HAMQTT_Component` as its first field. This lets the framework treat it as a generic component and cast it safely when needed:

```c
typedef struct {
  HAMQTT_Component base;
  // Your component's internal state here
} HAMQTT_MyComponent;
```
This design lets the `HAMQTT_Device` interact with all components uniformly.

#### 2. Implement the VTable
Each component must define a HAMQTT_Component_VTable struct, which contains function pointers used by the framework to interact with the component.

```c
struct HAMQTT_Component_VTable {
    esp_err_t (*get_discovery_config)(HAMQTT_Component *component, 
                                      cJSON *root,
                                      const char *device_unique_id);

    void (*handle_mqtt_message)(HAMQTT_Component *component,
                                const char *topic,
                                const char *data);

    void (*update)(HAMQTT_Component *component,
                   esp_mqtt_client_handle_t mqtt_client);

    const char *(*get_unique_id)(HAMQTT_Component *component);
    
    const char *const *(*get_subscribed_topics)(HAMQTT_Component *component,
                                                size_t *count);
};
```
Each function serves a specific role in the lifecyclle of a component:
| Function name           | Purpose |
| ----------------------- | ------- |
| `get_discovery_config`  | Called at startup to populate a cJSON object representing this component's discovery config. This object will be nested under the component's entry in the cmps section of the device discovery message. You must populate all required fields for the component type (see the [MQTT discovery docs](https://www.home-assistant.io/integrations/mqtt/#configuration)). |
| `handle_mqtt_message`   | Called when a subscribed MQTT topic for this component receives a new message. This is where you handle commands from Home Assistant (e.g. turning a switch on).                                                                                                   |
| `update`                | Called periodically in the main loop to publish state updates. You are responsible for formatting and publishing the MQTT payload.                                                                                                                                 |
| `get_unique_id`         | Returns a unique string identifier for this component. This will be used to build discovery topic paths and for de-duplication inside Home Assistant.                                                                                                              |
| `get_subscribed_topics` | Returns a pointer to an array of topic strings and a count. These topics are automatically subscribed to and routed to `handle_mqtt_message`


---

### Steps to Add a New Component

1. **Define a struct** for your component with `HAMQTT_Component base;` as the first member.
2. **Implement all five functions** in the vtable.
3. **Create a factory function**, e.g. `hamqtt_my_component_create()`, that:
   - Allocates and initializes the struct.
   - Sets the `base.v` field to point to your vtable.
4. **Register the component** using `hamqtt_device_add_component()`.

Your component will now be included in device discovery, state updates, and command routing.

---

If you need a reference, take a look at the existing `binary_sensor` or `button` components for a simple example of a full implementation.

---

## License

HAMQTT is licensed under the **Apache License 2.0**. See [`LICENSE`](LICENSE) for details.

---