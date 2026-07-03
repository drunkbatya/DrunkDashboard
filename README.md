# DrunkDashboard

## About

An STM32-based automotive dashboard with a sunlight-readable pixel display, running a port of the [Flipper Zero OS](https://github.com/flipperdevices/flipperzero-firmware).

### Front
<picture>
    <img alt="" src="/hardware/production/DrunkDashboard_v2/pcb_main_render.png">
</picture>

### Back
<picture>
    <img alt="" src="/hardware/production/DrunkDashboard_v2/pcb_back_render.png">
</picture>

### Specs
- MCU: stm32u585
- Display: Winstar WG24064, RA6963-based
- Canbus
- 8xADC
- 8x12v logic level inputs

## Hardware
Kicad source and production files lives in [hardware](./hardware) directory.

## Building
### Tools
arm-none-eabi-gcc toolchain needs to be installed, also make, python3 and icon converter deps:
```bash
python3 -m pip install -r scripts/requirements.txt 
```
openocd is required for flashing and arm-none-eabi-gdb for debugging

### Build
```bash
git submodule update --init --recursive
make
```
