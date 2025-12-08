---
id: index
title: D3 - Analysis
sidebar_label: D3 Analysis
---

# D3 - Analysis

## Market Analysis
Most market solutions are simple FTDI or CP210x chips on a breakout board. High-end logic analyzers are expensive and bulky. SerialyTTY fills the gap between a $5 dongle and a $500 analyzer.

## Requirements Analysis

### Functional Requirements
- **Bridge Mode:** Transparent bidirectional communication.
- **Auto-Baud:** Automatic detection of transmission speed.
- **Display:** 1.8" TFT screen for status and data.
- **Input:** Rotary encoder or buttons for menu navigation.
- **Power:** USB-C powered.

### Non-Functional Requirements
- **Latency:** Minimal latency in bridge mode (<1ms).
- **Boot Time:** Instant on (<2s).
- **Durability:** Robust PCB design for field use.

## Resource Analysis
- **Hardware:** ESP32-C6 (RISC-V, WiFi/BT/Zigbee).
- **Software:** ESP-IDF / Arduino framework via PlatformIO.
- **Budget:** Low-cost BOM to ensure accessibility.
