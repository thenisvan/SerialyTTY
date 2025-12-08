---
id: index
title: D7 - Lessons Learned
sidebar_label: D7 Lessons Learned
---

# D7 - Lessons Learned

## Retrospective

### What Went Well
- **ESP32-C6 Choice:** The RISC-V architecture proved powerful and efficient.
- **Team Synergy:** Clear role division (Denis on HW, Danylo on Arch, Ondrej on Docs) prevented stepping on toes.
- **Docusaurus:** Moving to Docusaurus significantly improved our documentation quality.

### Challenges
- **DMA Complexity:** Setting up SPI DMA for the display was harder than expected but worth the performance gain.
- **Power Management:** Balancing WiFi/BT power usage with battery life requires further optimization.

### Future Improvements
- **Modular Hardware:** Plan to design a custom PCB with add-on modules.
- **Web Flasher:** Implement a web-based tool to flash the firmware directly from the browser.

## Best Practices Established
- **Documentation First:** Writing docs (thanks to Ondrej) helps clarify requirements before coding.
- **Automated Builds:** CI saves time and catches compilation errors early.
