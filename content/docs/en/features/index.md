---
id: features
title: Features Overview
sidebar_position: 3
---

# Features Overview

SerialyTTY combines multiple advanced features into a comprehensive serial communication tool.

## Core Features

### 🎯 Intelligent Baud Rate Detection

Automatically detects the communication speed of connected devices without manual configuration.

**How it works:**
- GPIO interrupt-based bit timing measurement
- Analyzes falling edge patterns on RX line
- Calculates bit period from timing samples
- Supports standard baud rates: 9600, 19200, 38400, 57600, 115200

**Use cases:**
- Unknown device interrogation
- Automatic configuration
- Multi-device testing
- Educational demonstrations

**Technical details:**
- Precision: ±2% tolerance
- Detection time: <1 second (typical)
- Minimum data: 8 bytes required
- Interrupt latency: <10µs

### 🔄 Transparent Bridge Mode

Bidirectional serial data passthrough between USB and UART.

**Features:**
- Zero-latency forwarding (interrupt-driven)
- Real-time RX/TX byte counters
- Escape sequence (`~~~`) for mode exit
- Buffer management (1KB TX/RX)

**Performance:**
- Throughput: Up to 115200 bps
- Latency: <1ms typical
- Buffer overflow protection
- Error detection and logging

**Use cases:**
- Device programming
- Debugging embedded systems
- Protocol analysis
- Firmware updates

### 📺 TFT Display Integration

240x320 color display for visual feedback and status monitoring.

**Display Screens:**

1. **Boot Screen**
   - SerialyTTY logo
   - Firmware version
   - Hardware detection status
   - Initialization progress

2. **Menu Screen**
   - Command list (D, B, S, I, H)
   - Hardware status indicators
   - Navigation instructions
   - Current state display

3. **Analyzing Screen**
   - Baud detection progress
   - Real-time bit timing
   - Instruction prompts
   - Detection status

4. **Bridge Screen**
   - Live RX/TX counters
   - Current baud rate
   - Connection status
   - Escape sequence hint

**Technical Features:**
- DMA-accelerated rendering
- 8x8 bitmap font (custom)
- RGB565 color depth
- 40 MHz SPI clock
- 30+ FPS update rate
- Multi-line text support

### 💾 SD Card Logging

Comprehensive logging system with timestamped entries and structured data.

**Log Structure:**
```
[2025-12-08 14:32:15] [INFO] System boot complete
[2025-12-08 14:32:20] [BAUD] Detected: 115200 bps
[2025-12-08 14:32:25] [DATA] TX: 48 65 6C 6C 6F 20 57 6F 72 6C 64
[2025-12-08 14:32:30] [STATE] Entering bridge mode
```

**Log Levels:**
- `INFO` - System events and status changes
- `DATA` - Communication data (hex dump)
- `BAUD` - Baud rate detection results
- `STATE` - State machine transitions
- `TEST` - Testing and diagnostic data
- `ERROR` - Error conditions

**Features:**
- Auto-flush every 10 entries
- Binary data hex encoding
- Large packet handling (up to 128 bytes shown)
- Timestamped filenames: `log_YYYYMMDD_HHMMSS.txt`
- Directory auto-creation: `/sdcard/logs/`

**Storage:**
- FAT16/FAT32 filesystem
- Up to 32GB cards supported
- VFS (Virtual Filesystem) integration
- Safe unmount on shutdown

### 🖥️ Interactive Menu System

Professional ANSI-formatted terminal interface for device control.

**Commands:**

| Key | Command | Description |
|-----|---------|-------------|
| `D` | Detect Baud | Start automatic baud rate detection |
| `B` | Bridge Mode | Enter transparent passthrough mode |
| `S` | Settings | Configure device parameters (future) |
| `I` | Info | Display hardware and system information |
| `H` | Help | Show command reference |

**Menu Features:**
- ANSI color coding
- Clear screen control
- Keyboard navigation
- State-aware prompts
- Context-sensitive help

**System States:**
- `BOOTING` - Initialization in progress
- `MENU` - Main menu active
- `ANALYZING` - Baud detection running
- `FOUND_SPEED` - Baud rate detected
- `BRIDGE` - Passthrough mode active
- `TESTING` - Communication test mode

### 🔍 Hardware Auto-Detection

Intelligent scanning and configuration of connected peripherals.

**Detection Methods:**

1. **I2C Scan**
   - Probes address range 0x08-0x77
   - Identifies display controllers
   - Detects sensors and expanders
   - Reports found devices

2. **GPIO Detection**
   - Tests chip select (CS) lines
   - Verifies SPI peripheral presence
   - Validates pin states
   - Checks pull-up/pull-down

3. **SPI Probe**
   - Attempts device initialization
   - Reads manufacturer IDs
   - Verifies communication
   - Reports capabilities

**Graceful Degradation:**
- Continues operation without peripherals
- Adapts feature set to available hardware
- Clean boot with missing devices
- No blocking or errors

### 📡 Bluetooth LE Support (Stub)

Framework ready for Nordic UART Service (NUS) implementation.

**Planned Features:**
- Wireless serial bridging
- Mobile app connectivity
- Over-the-air configuration
- Remote monitoring

**Current Status:**
- BLE stack initialized
- Service UUID defined
- Characteristic handlers ready
- Requires implementation completion

## Advanced Capabilities

### State Management

Robust state machine with clean transitions:

```
BOOTING → MENU → ANALYZING → FOUND_SPEED → BRIDGE
   ↑         ↓                                  ↓
   └─────────┴──────────────────────────────────┘
```

**Features:**
- Atomic state transitions
- Callback-based event handling
- State history tracking
- Timeout management

### Error Handling

Comprehensive error detection and recovery:

- UART framing errors
- SPI communication failures
- SD card write errors
- Buffer overflow protection
- Watchdog timer (future)

### Performance Optimization

**DMA (Direct Memory Access):**
- Display buffer transfers
- UART TX/RX operations
- Reduced CPU overhead
- Higher throughput

**Interrupt-Driven I/O:**
- Sub-microsecond response
- Precise timing measurements
- Non-blocking operations
- Priority-based handling

**FreeRTOS Integration:**
- Task-based architecture
- Efficient scheduling
- Resource sharing
- Synchronization primitives

## Integration Features

### SPI Bus Sharing

Efficient sharing between TFT and SD card:

- Mutex-based arbitration
- Separate chip select (CS) management
- Different clock speeds per device
- Error state handling (`ESP_ERR_INVALID_STATE`)

### Dual UART Configuration

- **UART0**: USB serial (console, menu, logging)
- **UART1**: Target device (bridge, detection)
- Independent baud rates
- Isolated buffers
- Cross-communication support

### Timestamping

- RTC integration
- Microsecond precision
- ISO 8601 format
- Timezone support (configurable)

## Future Features (Roadmap)

### Phase 5 - Protocol Analysis
- [ ] I2C bus analyzer
- [ ] SPI protocol decoder
- [ ] Modbus RTU support
- [ ] Custom protocol parsers

### Phase 6 - Data Visualization
- [ ] Real-time waveform display
- [ ] Protocol timeline view
- [ ] Statistics graphs
- [ ] Packet inspection

### Phase 7 - Advanced Logging
- [ ] Circular buffer recording
- [ ] Trigger-based capture
- [ ] Data replay functionality
- [ ] Export to CSV/JSON

### Phase 8 - Connectivity
- [ ] WiFi web interface
- [ ] BLE UART service
- [ ] MQTT data publishing
- [ ] REST API

## Feature Comparison

| Feature | SerialyTTY | Basic USB-TTL | Logic Analyzer |
|---------|-----------|---------------|----------------|
| **UART Bridge** | ✅ | ✅ | ❌ |
| **Auto Baud** | ✅ | ❌ | ❌ |
| **Display** | ✅ | ❌ | ✅ (PC) |
| **SD Logging** | ✅ | ❌ | ✅ |
| **Live Stats** | ✅ | ❌ | ✅ |
| **Standalone** | ✅ | ✅ | ❌ |
| **Protocol Decode** | 🔄 | ❌ | ✅ |
| **Triggering** | 🔄 | ❌ | ✅ |
| **Cost** | ~$15 | ~$5 | $50-500 |

**Legend:** ✅ Supported | ❌ Not supported | 🔄 Planned

---

**Next:** Dive into the [Software Architecture](../architecture/index.md) to understand how it all works.
