# Hardware

| Folder | Content |
|---|---|
| [`bdc-rev1`](bdc-rev1) | REV1 (2022): schematic (PDF, 2 sheets), BOM, ESP32 pin assignment, EAGLE schematic and board in `source/` |
| [`bdc-rev2`](bdc-rev2) | REV2 (2023): schematic (PDF), BOM, ESP32 pin assignment, EAGLE schematic and board in `source/` |
| [`mechanical`](mechanical) | `encoder_knob_rev1.stl` for the rotary encoder of REV1, `protection_cap.stl` |

The Gerber, drill and pick-and-place files of both revisions are attached to the [releases](https://github.com/marvinramm/BDC/releases).

- **REV2:** the files come from the EAGLE CAM processor.
- **REV1:** no CAM output was left, so the files were generated from the EAGLE board with KiCad's EAGLE importer. Check them in a Gerber viewer before you order.

The EAGLE files use parts from my own libraries. EAGLE embeds them in the `.sch` / `.brd` files, so the designs open without the libraries.

## REV1 – all-in-one

Battery input via XT60 and balancer connector (up to 6S). Each cell has an HY2213 balancer IC that switches a 62 Ω bleed resistor. The cell voltages go through dividers to a CD74HC4067 multiplexer and on to the ESP32 ADC. An INA219 measures current and power across a 10 mΩ shunt. An AOD403 P-MOSFET switches the 100 W / 8 Ω aluminium resistor on top of the board, and a 40 mm fan cools it.

## REV2 – controller for an external load

Battery and load each connect to an XT60 (`OUTPUT 15A` on the silkscreen). An AOD403 P-MOSFET, driven by an SI2302, switches the load on the high side. An LM2937 supplies the ESP32 with 3.3 V. The pack voltage reaches GPIO39 through a voltage divider. Operation is with one button, RGB LED, buzzer and 0.96" OLED.

## License

The hardware design files and documentation are licensed under [CC BY 4.0](LICENSE). See [LICENSING.md](../LICENSING.md) for the attribution text.
