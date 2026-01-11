# Smart Traffic Light Controller (FSM-Based)

A smart 4-way traffic light system built using Arduino and a Finite State Machine (FSM) architecture. This project is designed to simulate and later control a real-world intersection using clean timing logic, modular code structure, and future sensor/IoT extensions.

---

## Project Overview

This system controls a four-lane intersection:

- Lanes **1 & 3** -> Group A
- Lanes **2 & 4** -> Group B

Only one group is allowed to be green at a time, with yellow and all-red safety phases between transitions.

The controller is implemented using:
- A Finite State Machine (FSM)
- Non-blocking timing using `millis()`
- Modular function design
- Safety handling for unconnected hardware

---

## Current Features

- FSM-based traffic control logic
- Configurable timing constants
- All-red safety state
- Yellow transition states
- Separation between logic and hardware output
- Safe GPIO handling for placeholder pins
- Serial debugging support (optional)

---

## FSM Design

### States

| State | Description |
|------:|-------------|
| S0 | All lights **RED** (safety phase) |
| S1 | Group A **GREEN** (L1 + L3) |
| S2 | Group A **YELLOW** (L1 + L3) |
| S3 | Group B **GREEN** (L2 + L4) |
| S4 | Group B **YELLOW** (L2 + L4) |

### State Flow

S0 (ALL_RED) -> S1 (A_GREEN) -> S2 (A_YELLOW) -> S3 (B_GREEN) -> S4 (B_YELLOW) -> S0


---

## Timing Configuration

All timing values are configurable.

| Constant | Description | Default |
|---------|-------------|---------|
| `ALL_RED_TIME` | All lights red duration | 30s |
| `YELLOW_TIME` | Yellow phase duration | 4s |
| `MIN_GREEN_TIME` | Minimum green time (for sensor logic later) | 20s |
| `MAX_GREEN_TIME` | Maximum green time | 30s |

**Note:** Timing values in code are stored in milliseconds.

---

## Hardware Plan (Coming Soon)

Planned hardware components:
- 12 LEDs (4 lanes x {red, yellow, green})
- Resistors for each LED
- Breadboard + jumper wires
- (Later) sensors per lane (ex: ultrasonic / IR / weight sensors)
- (Later) ESP32 / Wi-Fi integration for a web dashboard

**Pins are currently set to -1 as placeholders** until the hardware wiring is finalized.

---

## How It Works

- `setup()` initializes Serial, configures LED pins as outputs, and applies the initial state.
- `handleState()` uses `millis()` to measure elapsed time in the current state and decides when to transition.
- `applyState()` updates the traffic lights based on the current FSM state.
- `loop()` runs continuously and triggers state transitions without blocking delays.

---

## Roadmap / Next Steps

- [ ] Wire LEDs and assign real pin numbers
- [ ] Validate timing + transitions on real hardware
- [ ] Add lane sensors (start with one sensor, then scale)
- [ ] Implement sensor-based green time adjustments using `MIN_GREEN_TIME`
- [ ] Add LCD countdown display
- [ ] Add ESP32/web dashboard to display active state + countdown
- [ ] Add VIP/emergency override logic (future feature)

---

## Notes

This project is being developed incrementally with clean commits (each function / layer pushed separately) to keep the GitHub history readable and professional.

---

## Author

**Simon Maarawi**
