# Chart GUI SPA (Single Page Application)

## IoT platform

This SPA works on the following iot platform:

==> **[iot-wireless](https://github.com/araobp/iot-wireless)**

## Architecture

```
  [ChartGUI/HTML5]<--MQTT/WebSocket---[mosquitto]<--MQTT---[gateway.py]<--BLE---[MEMS-mic/STM32]
           ^                               |
           |                               V
           +---------REST------------[API server]--[SQLite]
```

## SPA code

The code is maintained in the "iot-wireless" project.

==> **[code](https://github.com/araobp/iot-wireless/tree/master/html5)**

## Screen shots

<img src="./screencapture.jpg" width=400>

<img src="./screencapture2.jpg" width=400>
