# pylonmonitor — Product Overview

**pylonmonitor** is a Pylontech LiFePO₄ battery pack monitor for the Raspberry Pi. It bridges a Pylontech battery system (up to 64 auto-detected modules) to an MQTT smart-home bus and a local web dashboard.

## Core Function

- Connects to one or more Pylontech batteries via the master module's RS-232 "Console" port (TTL level-shifted to the Pi's UART at 115200 baud)
- Polls each battery in turn using the Pylontech text-terminal protocol (`bat <N>` commands)
- Collects per-cell voltage, current, temperature, SoC, coulomb count, balance status, and fault states
- Publishes the data as JSON to an MQTT broker and writes a JSON file for a local Apache-served dashboard

## Outputs

- **MQTT**: structured JSON payloads on a configurable topic hierarchy (`<publishTopic>/<locationTopic>/<deviceTopic>/…`)
- **Local web dashboard**: single-page HTML/JS app at the Apache root; reads `wxdata/batteryinfo.json` updated every read cycle
- **Config web UI**: PHP-backed setup page at `/setup.html` to change MQTT broker settings at runtime without recompiling

## Target Hardware

Raspberry Pi Zero W (primary target — lowest power draw), but any Linux SBC works. The Pi's internal UART (`/dev/ttyAMA0`) must be remapped away from the Bluetooth subsystem; the serial login shell must be disabled.
