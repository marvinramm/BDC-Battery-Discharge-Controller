# BDC OS

The firmware of the Battery Discharge Controller. It runs on the ESP32 of the board.

| Sketch | Board | Version |
|---|---|---|
| [`BDC-OS`](BDC-OS) | BDC REV2 | 0.5.3 (September 2023) |
| [`BDC-OS-REV1`](BDC-OS-REV1) | BDC REV1 | 0.4 (October 2022), prototype |

## Operation (REV2)

| Input | Idle / done | While discharging |
|---|---|---|
| Short press | Target voltage +0.05 V (3.60 → 3.90 V, then back to 3.60 V) | Switch the display page |
| Long press (> 1 s) | Start discharging | Stop |

| LED | State |
|---|---|
| Blue | Idle |
| Red | Discharging |
| Green | Done |

The defaults are in `settings.h`: target voltage (`STD_DISCHARGE_VOLTAGE`), long-press time, buzzer on/off and the time until light sleep.

## Build

1. Install the [ESP32 Arduino core](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html). Tested with **2.0.17**.
2. Install the libraries:

   | Library | Used in |
   |---|---|
   | [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library), [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306) | both |
   | [INA219_WE](https://github.com/wollewald/INA219_WE) | REV1 |
   | [CD74HC4067](https://github.com/waspinator/CD74HC4067) | REV1 |

3. Board **ESP32 Dev Module**, default settings (4 MB flash, default partition scheme).

```bash
arduino-cli compile --upload -p /dev/ttyUSB0 --fqbn esp32:esp32:esp32 firmware/BDC-OS
```

The board has no USB port. Connect a 3.3 V USB-to-UART adapter (e.g. CP2102) to the programming header: `TX`, `RX`, `3V3`, `GND`. To enter the bootloader, pull `BOOT` (GPIO0) to GND while powering up.

## Flash a release binary

Every [release](https://github.com/marvinramm/BDC/releases) has two images:

- `*_full.bin`: bootloader, partition table and application in one file, flash to **`0x0`**
- `*.bin`: application only, flash to **`0x10000`**

```bash
esptool.py --chip esp32 --baud 460800 write_flash 0x0 BDC-OS-v0.5.3-REV2_full.bin
```

## Bluetooth LE interface

The BDC advertises as a BLE peripheral. All values are UTF-8 strings and are sent as notifications every 500 ms while a device is connected.

Service `d4f94552-2959-496e-bc0f-d3e4f4e2da2d`

| Characteristic UUID | Value | Example |
|---|---|---|
| `fd57b195-58af-4479-8907-e5f4fdc9e8ed` | Discharge state (0 idle, 1 discharging, 2 done) | `1` |
| `10483f42-46f9-4789-92b4-2406b40a629d` | Pack voltage [V] | `15.824` |
| `d4b0046c-6717-4514-a36b-0e9c21f14990` | Average cell voltage [V] | `3.956` |
| `f3826d07-101e-4e53-9b7b-c43741c31c9e` | Target voltage per cell [V] | `3.75` |
| `99f5f192-b42d-439f-a9d2-993d0efa765b` | Cell count | `4` |
| `96d9f8ed-3091-47c1-a0d5-b24ed1bd09d3` | Progress [%] | `46` |
| `dbd25b9d-2ec4-481d-8a3b-b2ba13b2292b` | Firmware version | `0.5.3` |
| `4bf2958b-59f6-4f70-b0e5-4992d2a24b5b` | Target minus cell voltage [V] (≤ 0) | `-0.21` |

Any BLE app that can subscribe to notifications (e.g. nRF Connect) can read the values.

## Calibration

The pack voltage is measured through a voltage divider on GPIO39. `measure.cpp` converts the averaged ADC value to volts with a table of measured points and linear interpolation between them. If you build your own board, measure a few points with a lab power supply and replace the table.
