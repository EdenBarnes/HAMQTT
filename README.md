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
HAMQTT is a small C library that lets your ESP32 firmware expose esnors, buttons, and other components to Home Assistant over MQTT.

---

## Features

| Category     | Feature                                                                       |
| ------------ | ----------------------------------------------------------------------------- |
| Discovery    | Automatic generation & pubication of device and component discovery payloads. |
| Availability | Online / offline last-will handled for you                                    |
| Components   | `Binary_Sensor`, `Button` (more planned)                                      |
| Footprint    | ~2.9 KB flash / ~0 B static RAM (heap usage depends on component count)       |
| License      | Apache 2.0                                                                    |

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

All contributions require a DCO sign‑off (`git commit -s`).

---

## License

HAMQTT is licensed under the **Apache License 2.0**. See [`LICENSE`](LICENSE) for details.

---