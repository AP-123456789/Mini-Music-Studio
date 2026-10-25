# Mini Music Studio MIDI Controller

A compact, custom-designed USB MIDI controller built completely from scratch around the Waveshare ESP32-S3-Zero microcontroller. Designed as an intensive beginner learning project in KiCad 10.0 and PlatformIO, this project documents a 35-hour journey of trial, error, troubleshooting, and iteration in custom PCB design, schematic capture, dual-layer trace routing, and embedded C++ firmware development.

---

## What the Product Does
The **Mini Music Studio** is an interactive hardware input device that connects directly to a computer via USB to interface with Digital Audio Workstations (DAWs) and music production software. It serves several core functions:
* **Tactile Note Triggering:** Features 12 Kailh Choc low-profile mechanical key switches arranged in a 3x4 multiplexed anti-ghosting diode matrix to play musical notes, trigger drum samples, or launch audio clips.
* **Continuous Parameter Control:** Integrates two slide potentiometers to allow live, continuous adjustment of audio parameters such as track volume, filter cutoff, modulation, or expression.
* **Navigation & Effect Adjustment:** Incorporates two EC11 rotary encoders with push switches to scroll through menus, select instrument patches, or fine-tune effect intensities.
* **Dynamic Per-Key Backlighting:** Utilizes 12 addressable SK6812 MINI-E RGB NeoPixels mounted in a serial daisy-chain to provide real-time visual feedback, key identification, and custom lighting animations.
* **Auxiliary Touch Controls:** Employs four capacitive-touch pad headers to execute quick macro commands, switch control banks, or toggle lighting modes.
* **Native USB Processing:** Powered by an onboard Waveshare ESP32-S3-Zero running custom C++ firmware that processes all hardware inputs and transmits standard USB MIDI messages over USB.

---

## Development Journey & Process (35-Hour Breakdown)
Building this project as a beginner required immense patience, troubleshooting, and continuous reference to official documentation and developer communities:
1. **Project Initialization & Library Setup (Hours 0–1):** Opened KiCad 10.0 for the first time, established the project draft, and struggled through path substitutions to register external custom symbol and footprint libraries from the [Hackclub KiCad Libraries Repository](https://github.com/hackclub/Hackclub-KiCad-Libraries).
2. **Schematic Capture & Component Placement (Hours 1–5):** Placed 12 push-button switch symbols, the ESP32-S3-Zero microcontroller (`U1`), slide potentiometers (`RV1`, `RV2`), rotary encoders (`SW13`, `SW14`), 12 SK6812 MINI-E LEDs (`D1`–`D12`), and touch headers (`J1`–`J4`). Cross-checked pin mappings against the [Waveshare ESP32-S3-Zero Wiki & Pinout Guide](https://www.waveshare.com/wiki/ESP32-S3-Zero).
3. **Switch Matrix & Diode Wiring (Hours 5–13):** Paired 12 `D_Small` anti-ghosting diodes (`D13`–`D24`) with the switches and organized complex routing using clean `ROW1`–`ROW3` and `COL1`–`COL4` net labels. Wired encoder quadrature signals (`ENC1_A`, `ENC1_B`, `ENC2_A`, `ENC2_B`) and analog wiper outputs (`POT1_OUT`, `POT2_OUT`).
4. **LED Daisy-Chaining & ERC Cleanup (Hours 13–20):** Serialized the SK6812 MINI-E LEDs from `DOUT` to `DIN`, handled pin conflicts by re-assigning GPIOs, and resolved electrical rules check (ERC) warnings by adding `PWR_FLAG` symbols and No Connect markers.
5. **Footprint Assignment & PCB Layout (Hours 20–30):** Matched abstract schematic symbols to exact physical footprints, arranged components on the canvas within an `Edge.Cuts` board boundary, and routed dual-layer copper traces (`F.Cu` and `B.Cu`). Resolved track width violations and closed-loop outline errors to achieve a zero-error Design Rules Check (DRC).
6. **Manufacturing Files & 3D Render (Hours 30–32):** Inspected the board in KiCad's 3D Viewer, plotted production Gerbers and drill files, and generated BOM/CPL packages using the [JLCPCB Fabrication Toolkit](https://jlcpcb.com/).
7. **Firmware Development & USB MIDI (Hours 32–35):** Configured PlatformIO in VS Code using the [PlatformIO ESP32 Build System Guide](https://docs.platformio.org/). Wrote modular C++ drivers for matrix polling, FastLED animations (referencing the [FastLED Animation Library Documentation](https://fastled.io/docs/)), and integrated USB MIDI message transmission following the [MIDI Association USB MIDI Specification](https://www.midi.org/specifications).

---

## Bill of Materials (BOM) & Estimated Costs

| No. | Item | Quantity | Purpose | Unit Price | Link / Sourcing | Cost |
| :--- | :--- | :---: | :--- | :---: | :--- | :---: |
| **1** | Waveshare ESP32-S3-Zero | 1 | Main MCU controller & USB MIDI interface | $4.50 | [Waveshare Store](https://www.waveshare.com/esp32-s3-zero.htm) | $4.50 |
| **2** | Kailh Choc Low-Profile Switches | 12 | Primary musical key switches | $0.75 | [Adafruit #5118](https://www.adafruit.com/product/5118) | $9.00 |
| **3** | SK6812 MINI-E NeoPixels | 12 | Per-key RGB backlight illumination | $0.20 | [Adafruit #4960](https://www.adafruit.com/product/4960) | $2.40 |
| **4** | 10k Slide Potentiometers | 2 | Continuous analog parameter controls | $1.10 | [Adafruit #4219](https://www.adafruit.com/product/4219) | $2.20 |
| **5** | EC11 Rotary Encoders w/ Switch | 2 | Parameter adjustment & navigation | $1.00 | [Adafruit #377](https://www.adafruit.com/product/377) | $2.00 |
| **6** | 1N4148 SOD-123 Diodes | 12 | Matrix anti-ghosting switching diodes | $0.03 | [Adafruit #1928](https://www.adafruit.com/product/1928) | $0.36 |
| **7** | Custom Printed PCB (5-pack share) | 1 | Dual-layer circuit board fabrication | $2.54 | [JLCPCB](https://jlcpcb.com/) | $2.54 |
| **8** | Touch Pad Connector Headers | 4 | Placeholders for capacitive touch areas | $0.15 | [Adafruit #392](https://www.adafruit.com/product/392) | $0.60 |

---

## Firmware Setup & Usage
The firmware is written in C++ using PlatformIO on the Arduino framework.

1. Open the project folder in Visual Studio Code with the PlatformIO extension installed.
2. Connect the Waveshare ESP32-S3-Zero to your computer via USB.
3. Build and upload the firmware binary:
   ```bash
   pio run --target upload
