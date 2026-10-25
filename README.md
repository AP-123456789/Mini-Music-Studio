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

![M1](https://halflife.hackclub-assets.com/sessions/OcYWjcDouGEzidswYkZNmkY7iSde7Zal/cc751d6f505168eb9c50464379d9b8d13ed1891d4ece7118996b29551f3ea02f.jpg)

2. **Schematic Capture & Component Placement (Hours 1–5):** Placed 12 push-button switch symbols, the ESP32-S3-Zero microcontroller (`U1`), slide potentiometers (`RV1`, `RV2`), rotary encoders (`SW13`, `SW14`), 12 SK6812 MINI-E LEDs (`D1`–`D12`), and touch headers (`J1`–`J4`). Cross-checked pin mappings against the [Waveshare ESP32-S3-Zero Wiki & Pinout Guide](https://www.waveshare.com/wiki/ESP32-S3-Zero).

![image](https://halflife.hackclub-assets.com/sessions/OcYWjcDouGEzidswYkZNmkY7iSde7Zal/186967f13f15693f380298cdeb7924549beea41607dfac9e76e550dc13926f1f.png)

3. **Switch Matrix & Diode Wiring (Hours 5–13):** Paired 12 `D_Small` anti-ghosting diodes (`D13`–`D24`) with the switches and organized complex routing using clean `ROW1`–`ROW3` and `COL1`–`COL4` net labels. Wired encoder quadrature signals (`ENC1_A`, `ENC1_B`, `ENC2_A`, `ENC2_B`) and analog wiper outputs (`POT1_OUT`, `POT2_OUT`).

![image](https://halflife.hackclub-assets.com/sessions/OcYWjcDouGEzidswYkZNmkY7iSde7Zal/6c54d830175ee81d0bbb84404e03096d28fafab96ee7f603c265204afa0b590b.png)

4. **LED Daisy-Chaining & ERC Cleanup (Hours 13–20):** Serialized the SK6812 MINI-E LEDs from `DOUT` to `DIN`, handled pin conflicts by re-assigning GPIOs, and resolved electrical rules check (ERC) warnings by adding `PWR_FLAG` symbols and No Connect markers.

![image](https://halflife.hackclub-assets.com/sessions/OcYWjcDouGEzidswYkZNmkY7iSde7Zal/31a0f4105f9d86d99972f8a9d394dd2465495bbc54bafbb8e1f8fda3a0c8209e.png)

5. **Footprint Assignment & PCB Layout (Hours 20–30):** Matched abstract schematic symbols to exact physical footprints, arranged components on the canvas within an `Edge.Cuts` board boundary, and routed dual-layer copper traces (`F.Cu` and `B.Cu`). Resolved track width violations and closed-loop outline errors to achieve a zero-error Design Rules Check (DRC).

![image](https://halflife.hackclub-assets.com/sessions/OcYWjcDouGEzidswYkZNmkY7iSde7Zal/10ade81b107250fe5b3b5c23659ceafd8ca7f1a0b50ccbf8afcfd5d9c795baf5.png)
  
6. **Manufacturing Files & 3D Render (Hours 30–32):** Inspected the board in KiCad's 3D Viewer, plotted production Gerbers and drill files, and generated BOM/CPL packages using the [JLCPCB Fabrication Toolkit](https://jlcpcb.com/).

![ImageX5](https://halflife.hackclub-assets.com/sessions/OcYWjcDouGEzidswYkZNmkY7iSde7Zal/62cfc15720eb0bb88c9e9191c7dcd6075da97edf3bee8a0be81ae058ba096c2f.jpg)

7. **Firmware Development & USB MIDI (Hours 32–35):** Configured PlatformIO in VS Code using the [PlatformIO ESP32 Build System Guide](https://docs.platformio.org/). Wrote modular C++ drivers for matrix polling, FastLED animations (referencing the [FastLED Animation Library Documentation](https://fastled.io/docs/)), and integrated USB MIDI message transmission following the [MIDI Association USB MIDI Specification](https://www.midi.org/specifications).

![ImageX9](https://halflife.hackclub-assets.com/sessions/OcYWjcDouGEzidswYkZNmkY7iSde7Zal/57defc4888c7a0b18f5801430313d9a36ed6b7b51a1732ec01cabbe82c7dc950.jpg)
---

## Bill of Materials (BOM) & Estimated Costs

| Item No. | Item | Designator | Quantity | Purpose | Unit Price ($) | Total Price ($) | Vendor | Vendor Link |
| :---: | :--- | :---: | :---: | :--- | :---: | :---: | :---: | :--- |
| **1** | ESP32-S3 Microcontroller Module | U1 | 1 | Main MCU controller & USB MIDI interface | $4.50 | $4.50 | JLCPCB / LCSC | [JLCPCB Sourcing](https://jlcpcb.com/) |
| **2** | Kailh Choc Low-Profile Switches | SW1–SW12 | 12 | Primary musical key switches | $0.75 | $9.00 | JLCPCB / LCSC | [JLCPCB Sourcing](https://jlcpcb.com/) |
| **3** | SK6812 MINI-E RGB LEDs | D1–D12 | 12 | Per-key RGB backlight illumination | $0.20 | $2.40 | JLCPCB / LCSC | [JLCPCB Sourcing](https://jlcpcb.com/) |
| **4** | 10k Slide Potentiometers | RV1, RV2 | 2 | Continuous analog parameter controls | $1.10 | $2.20 | JLCPCB / LCSC | [JLCPCB Sourcing](https://jlcpcb.com/) |
| **5** | EC11 Rotary Encoders w/ Switch | SW13, SW14 | 2 | Parameter adjustment & navigation | $1.00 | $2.00 | JLCPCB / LCSC | [JLCPCB Sourcing](https://jlcpcb.com/) |
| **6** | 1N4148 SOD-123 Diodes | D13–D24 | 12 | Matrix anti-ghosting switching diodes | $0.03 | $0.36 | JLCPCB / LCSC | [JLCPCB Sourcing](https://jlcpcb.com/) |
| **7** | 1x01 Pin Headers (Touch Pads) | J1–J4 | 4 | Placeholders for capacitive touch areas | $0.15 | $0.60 | JLCPCB / LCSC | [JLCPCB Sourcing](https://jlcpcb.com/) |
| **8** | Custom Printed PCB (Dual-Layer) | PCB | 1 | Circuit board fabrication (5-pack share) | $2.54 | $2.54 | JLCPCB | [JLCPCB Fabrication](https://jlcpcb.com/) |

---

## Firmware Setup & Usage
The firmware is written in C++ using PlatformIO on the Arduino framework.

1. Open the project folder in Visual Studio Code with the PlatformIO extension installed.
2. Connect the Waveshare ESP32-S3-Zero to your computer via USB.
3. Build and upload the firmware binary:
   ```bash
   pio run --target upload
