---
id: index
title: D6 - Verification & Testing
sidebar_label: D6 Verification
---

# D6 - Verification & Testing

## Testing Strategy

### Unit Testing
We use the `Unity` framework for unit testing core logic components like the circular buffer and protocol parsers.

### Integration Testing
*Lead: Denis Ivan*
- **Loopback Tests:** Connecting TX to RX to verify data integrity.
- **Stress Tests:** Sending continuous streams of data at max baud rate for hours.

### User Acceptance Testing (UAT)
*Lead: Ondrej*
- Verifying the menu navigation flow.
- Checking documentation against actual device behavior.
- Usability testing with new users.

## CI/CD
We utilize GitHub Actions to:
1.  Build the firmware on every commit.
2.  Build and deploy this documentation site.
