# xiao_nrf52840-bluetooth-tracker
Arduino sketch for a Bluetooth tracker that emits a buzz when triggered by a Bluetooth client. This sketch is built for the Seeed XIAO nRF52840 board.

# Arduino IDE Notes
1. In Arduino IDE, use the following board manager: https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json.
2. The board manager used is the "Seeed nRF52 Boards" package.
3. After selecting the proper board/board manager, no further libraries need installed.
4. This sketch uses Bluefruit for BLE implementation, which should not need installed if the proper board manager is selected.

# Usage
1. This sketch was built for the XIAO nRF52840 board, though any board that is supported by the Adafruit Bluefruit library should work with minimal adjustments.
2. The sketch assumes that the buzzer is connected to pin D10. The pin itself may not provide enough power for the buzzer to emit an audible sound. In this case, use a transister to wire the buzzer directly to the 3v3 pin with D10 wired to the transister's base.
3. The name of the Bluetooth device is "Buzz". Once you are connected with a client (you may either build your own app or use a BLE Scanner application), writing any non-zero value to the Custom Service > Custom Characteristic will toggle the D10 pin.
4. The BLE battery service is not accurate and should only be used as a rough indicator of a near-dead battery.
