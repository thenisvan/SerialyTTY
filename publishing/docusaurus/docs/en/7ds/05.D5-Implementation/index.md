---
id: index
title: D5 - Implementation
sidebar_label: D5 Implementation
---

# D5 - Implementation

## Technology Stack
- **MCU:** Espressif ESP32-C6 (RISC-V).
- **Framework:** ESP-IDF (C++17).
- **Build System:** PlatformIO.
- **Documentation:** Docusaurus.

## Development Process
*Lead: Denis Ivan*

We follow a feature-branch workflow.
1.  **Core Logic:** Implemented first to ensure stable UART bridging.
2.  **Drivers:** Custom drivers written for the display to maximize frame rates.
3.  **Integration:** Combining Bluetooth and Serial tasks on separate FreeRTOS cores (or cooperative scheduling on single-core C6).

## Challenges Solved
- **High-Speed UART:** Optimized ISRs to handle 921600 baud without dropping packets.
- **Display Flicker:** Implemented DMA (Direct Memory Access) for screen updates to offload the CPU.
- **Memory Management:** Careful tuning of stack sizes for FreeRTOS tasks to fit within the ESP32-C6 RAM constraints.
