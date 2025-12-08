---
id: index
title: D4 - Design
sidebar_label: D4 Design
---

# D4 - Design

## System Architecture
*Architect: Danylo*

The system is designed as a modular event-driven architecture.

### Core Modules
1.  **Kernel/Main Loop:** Handles task scheduling and watchdog.
2.  **Hardware Abstraction Layer (HAL):** Interfaces with ESP32 peripherals (UART, SPI, GPIO).
3.  **Managers:**
    - `DisplayManager`: Controls the TFT screen updates.
    - `BluetoothManager`: Handles BLE connections and GATT services.
    - `MenuSystem`: Manages UI navigation and state.
4.  **Drivers:** Specific implementations for ST7735 displays, SD cards, etc.

## Data Flow
Data flows from the UART ISR (Interrupt Service Routine) into a circular buffer. The `CommTester` or `BridgeMode` tasks consume this buffer and route data to:
- USB CDC (for PC connection).
- Display (for visualization).
- SD Card (for logging).

## UI/UX Design
The user interface is designed for navigation with a simple rotary encoder or 3-button setup (Up, Down, Select), ensuring usability even with gloves or in tight spaces.
