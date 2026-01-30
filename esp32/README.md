# ESP32-CAM Biped Robot Setup

## Wiring (servo control wires)
Use a dedicated 5V supply capable of handling servo stall current. Connect **all servo grounds** to the ESP32-CAM ground.

| Servo | Function | ESP32-CAM GPIO | Notes |
| --- | --- | --- | --- |
| Servo 1 | Left leg (hip) | GPIO 12 | Otto/Bob left leg |
| Servo 2 | Right leg (hip) | GPIO 13 | Otto/Bob right leg |
| Servo 3 | Left foot (ankle) | GPIO 14 | Otto/Bob left foot |
| Servo 4 | Right foot (ankle) | GPIO 15 | Otto/Bob right foot |
| Servo 5 | Head/camera pan | GPIO 2 | Limit to ±45° in code |

**Power:**
- Servos: 5V external supply (do **not** power servos from the ESP32-CAM 5V pin).
- ESP32-CAM: 5V to 5V pin (separate from servo rail recommended).
- Ground: tie ESP32-CAM GND to servo power ground.

**Camera module:**
- This sketch assumes the AI Thinker ESP32-CAM pinout.
- Avoid using the SD card while driving servos on these pins.

## Flashing
1. Open `esp32_cam_biped.ino` in the Arduino IDE.
2. Select **AI Thinker ESP32-CAM** as the board.
3. Flash and open the serial monitor to verify Wi-Fi connection.

## Network
The sketch connects to:
- SSID: `disney`
- Password: `tommyboy`

Once connected, the site should access:
- `http://<esp32-ip>/status`
- `http://<esp32-ip>/stream`
- `http://<esp32-ip>/command`
