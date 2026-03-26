# Smart Traffic Light Controller (Adaptive FSM-Based System)

A real-time adaptive 4-way traffic control system built using Arduino.  
This project evolves from a basic finite state machine into a **sensor-driven, IoT-connected, and AI-enhanced intersection controller**.

---

## Project Vision

This project is designed as a foundation for real-world smart infrastructure, with applications in:

- Traffic optimization  
- Emergency response systems  
- Smart city integration  
- Data-driven urban planning  

---

## Phase 1 — Embedded Traffic Control System (Complete)

### Features

- 4-way intersection using a **Finite State Machine (FSM)**
- States: `S0 → S1 → S2 → S3 → S4`
- **HX711 load cell sensors per lane** detect vehicle presence
- **Adaptive green time** based on real traffic conditions
- Minimum and maximum green time constraints
- **All-red safety state** between transitions
- Serial debugging for real-time monitoring

**Deliverable:** Working physical intersection model

---

## FSM Design

### States

| State | Description |
|------:|-------------|
| S0 | All lights **RED** (safety phase) |
| S1 | Group A **GREEN** (L1 + L3) |
| S2 | Group A **YELLOW** |
| S3 | Group B **GREEN** (L2 + L4) |
| S4 | Group B **YELLOW** |

### Traffic Groups

- **Group A:** Lanes 1 & 3  
- **Group B:** Lanes 2 & 4  

Only one group is green at a time.

---

## Adaptive Traffic Logic

Unlike traditional systems, this controller:

- Reads **real-time lane weight data**
- Dynamically adjusts green time:
  - Extends green if traffic is present
  - Switches early if a lane is empty
- Prevents starvation using:
  - `MIN_GREEN_TIME`
  - `MAX_GREEN_TIME`

This transforms the system from:

> Fixed timing → Adaptive, traffic-aware control

---

## Timing Configuration

| Constant | Description | Default |
|---------|-------------|---------|
| `ALL_RED_TIME` | Safety delay | 3s |
| `YELLOW_TIME` | Yellow phase | 4s |
| `MIN_GREEN_TIME` | Minimum green | 20s |
| `MAX_GREEN_TIME` | Base green | 30s |
| `EXTEND_GREEN` | Extension step | +5s |
| `max_green` | Hard cap | 90s |

> All values are stored in milliseconds in code.

---

## Hardware (Phase 1)

- Arduino (controller)
- 4 × HX711 modules
- 4 × load cells (one per lane)
- LEDs (traffic lights)
- Breadboard + wiring

---

## Debugging & Validation

- Serial Monitor outputs:
  - Current state
  - Lane weights
  - Elapsed time
  - Extended green duration
- Sensor readiness checks at startup
- Individual calibration per load cell using known weights

---

# Project Roadmap

---

## Phase 1 — Embedded Traffic Control System

- FSM-based intersection control  
- Load cell–based vehicle detection  
- Adaptive green time logic  
- All-red safety transitions  
- Serial debugging  

**Deliverable:** Working physical intersection model  

---

## Phase 2 — Live Web Dashboard

- ESP32 bridges Arduino data over WiFi  
- Node.js + Express backend  
- Socket.io for real-time updates  
- Dashboard displays:
  - Current state  
  - Lane weights  
  - Green time remaining  
  - Car counts per lane  
- Basic traffic history graphs  

**Deliverable:** Real-time browser dashboard  

---

## Phase 3 — Emergency Vehicle Preemption System

- Custom ESP32 handheld device (GPS + touchscreen)  
- Secure authentication using hardware ID  
- Real-time distance calculation (Haversine formula)  
- Automatic override at 1km:
  - Forces correct lane green  
  - Sets others to red  
- Logging system:
  - Timestamp  
  - Device ID  
  - GPS coordinates  

**Deliverable:** Authenticated emergency override system  

---

## Phase 4 — Computer Vision Traffic Analytics

- Raspberry Pi + dual camera setup  
- YOLOv8 vehicle detection  
- Lane-based vehicle counting using ROI zones  
- Replace load cells with vision-based detection  
- PostgreSQL database for historical data  
- Dashboard analytics:
  - Traffic patterns  
  - Peak hour detection  
  - Congestion prediction  

**Deliverable:** AI-powered smart intersection  

---

## System Evolution

Phase 1: Arduino + Sensors

          ↓

Phase 2: + ESP32 + Web Dashboard

          ↓

Phase 3: + GPS + Emergency System

          ↓

Phase 4: + Computer Vision + AI + Database


---

## What This Project Demonstrates

- Embedded systems (Arduino, sensors)
- Real-time systems and FSM design  
- Hardware/software integration  
- Networking (ESP32, backend, sockets)  
- Distributed system design  
- Computer vision + AI integration  

---

## Author

**Simon Maarawi**  
Computer Engineering @ Penn State  

---