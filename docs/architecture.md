# System Architecture

> This document describes the AUV's subsystem design at the block level. Full mechanical CAD — including parts, assemblies and the rendered view — lives in the team repository: **[diyakawaofficial/diyakawa_3.0](https://github.com/diyakawaofficial/diyakawa_3.0/tree/main/Design)**.

---

## Design constraints (from SAUVC 2026)

| Constraint | Value |
|---|---|
| Bounding box | ≤ 140 × 100 × 100 cm |
| Weight (in air) | ≤ 52 kg |
| Power | Self-contained, ≤ 24 VDC |
| Tether | Not allowed (except during sensor calibration, vehicle held stationary) |
| Kill switch | Required — stops all thrusters, diver-accessible, brightly marked |
| Propellers | Shrouded |
| Comms | Untethered; top-side equipment allowed in the start zone only |

The rules force a **self-contained underwater robot** with its own power, computing, sensing, and safety systems. This shapes every other design decision below.

---

## Top-level block diagram

┌───────────────────────────────────────────────────────────────┐
│ TOP-SIDE (start zone only) │
│ Comms to AUV · Operator console · Kill relay │
└──────────────────────────┬────────────────────────────────────┘
│ underwater comms link
▼
┌───────────────────────────────────────────────────────────────┐
│ AUV (untethered, self-contained) │
│ │
│ ┌────────────┐ ┌────────────┐ ┌──────────────────┐ │
│ │ Thrusters │ │ Camera │ │ Sensors │ │
│ │ + ESCs │ │ (vision) │ │ · IMU (MPU6050) │ │
│ │ (vectored) │ │ │ │ · Mag (HMC5883L)│ │
│ └─────┬──────┘ └─────┬──────┘ │ · Pressure │ │
│ │ │ └────────┬─────────┘ │
│ │ │ │ │
│ └────────┬────────┴────────┬───────────┘ │
│ ▼ ▼ │
│ ┌──────────────────┐ ┌────────────────┐ │
│ │ Flight controller│ │ Power system │ │
│ │ · PID loops │ │ · Battery │ │
│ │ · Mission logic │ │ · Regulators │ │
│ │ · State machine │ │ · ESCs │ │
│ └──────────────────┘ └────────────────┘ │
│ │
│ ┌──────────────────────────────┐ │
│ │ KILL SWITCH (brightly │ │
│ │ marked, cuts all thrusters)│ │
│ └──────────────────────────────┘ │
│ │
│ ┌──────────────────────────────┐ │
│ │ Ball mechanism │ │
│ │ (drop for Task 2, │ │
│ │ retrieve for Task 3) │ │
│ └──────────────────────────────┘ │
└───────────────────────────────────────────────────────────────┘



---

## 1. Mechanical structure

See the team CAD repo for the full assembly. At the design level the AUV consists of:

- **Frame / hull** — provides structural rigidity and houses the electronics in a watertight volume. Sized to fit within the SAUVC bounding box, with buoyancy and weight distribution tuned for neutral buoyancy at operating depth.
- **Buoyancy subassembly** (`Bouancy.SLDASM`) — foam or air-filled volumes sized to offset the mass of the vehicle, tuned so the AUV hovers rather than sinks or floats when thrusters are idle.
- **Thruster housings** (`MirrorThruster Shell.SLDPRT`) — shrouded enclosures for the thrusters, satisfying the SAUVC safety requirement.
- **Heave mechanism** (`MirrorHeave top` / `MirrorHeave bottom`) — vertical motion subsystem.
- **Side panels, brackets, supports** — mounting points for electronics, sensors, thrusters, and ballast.
- **Handle** (`plastic-handle`) — for the divers to grab and pass the AUV during competition runs.

The design is modular: thrusters, sensor mounts, and the electronics tray can be removed independently for servicing.

---

## 2. Propulsion & actuators

- **Thrusters** — arranged for full 6-DoF control or a reduced set (surge, sway, heave, yaw). Vectoring configuration depends on the final assembly; see CAD for exact placement.
- **ESCs** — driven by PWM (or DShot) from the flight controller.
- **Motor safety** — kill switch routes through a hardware relay or MOSFET that cuts ESC power regardless of software state.

Propeller shrouds are mandatory under the SAUVC rules and are part of the thruster housing CAD.

---

## 3. Sensors

### Vision
- Camera (USB or Pi camera) mounted forward-facing.
- Used for:
  - **Gate detection** — Task 1
  - **Drum colour / position detection** — Task 2
  - **Flare detection** — Task 4

### Inertial (my contribution)
- **MPU6050** — 6-axis IMU (3-axis accelerometer + 3-axis gyroscope), I²C.
- **HMC5883L** — 3-axis magnetometer, I²C — used as a heading reference to correct gyro drift over time.
- Fusion: complementary or extended Kalman filter, producing roll, pitch, and yaw estimates.

Full details in [`sensors.md`](sensors.md).

### Depth
- **Pressure transducer** (0–0.5 MPa, G1/4, 5 VDC) — measures hydrostatic pressure.
- Converted to depth using the standard relation `p = ρgh`, calibrated at the water surface before each run.

### Acoustics
- The SAUVC pinger for Task 2 operates at **45 kHz** (RJE International ULB-362B). Detection requires a hydrophone or receiver tuned to that band.
- *Status:* subsystem design stage; not fully integrated in the current build.

---

## 4. Electronics & power

| Block | Purpose |
|---|---|
| **Main controller** | Runs mission state machine, PID loops, vision, and sensor fusion. |
| **Battery** | Lithium-based, certified, ≤ 24 VDC system voltage. |
| **Power regulators** | Step-down rails for controller, sensors, and ESCs. |
| **ESCs** | Drive the thrusters. |
| **Kill switch** | Hardware cutoff for all thruster power. Routes through a relay or high-side MOSFET so pressing it stops the motors regardless of software. |
| **Underwater comms** | Top-side ↔ AUV link, used for Task 4 flare order. |

The kill switch is a hard requirement. It must cut thruster power even if the software has crashed — so it interrupts the ESC power bus physically, not via a software command.

---

## 5. Software & control

┌──────────────────────────────────────────────────────┐
│ Mission state machine │
│ Submerge → Navigate → Acquire → Reacquire → Comm │
└─────────────────────────┬────────────────────────────┘
│ desired depth, heading, velocity
▼
┌──────────────────────────────────────────────────────┐
│ Control loops (PID) │
│ · Depth hold (from pressure sensor) │
│ · Heading hold (from IMU + magnetometer) │
│ · Velocity (from vision or dead-reckoning) │
└─────────────────────────┬────────────────────────────┘
│ thruster commands
▼
┌──────────────────────────────────────────────────────┐
│ ESCs → Thrusters │
└──────────────────────────────────────────────────────┘


- **PID controllers** for depth and heading are the core low-level loops.
- **Vision pipeline** feeds target detections into the mission state machine.
- **Sensor fusion** (IMU + magnetometer + pressure) provides the state estimate that the PID loops and the state machine both consume.

---

## 6. Subsystem status

Honest snapshot of what was built and what was in progress at the end of the project:

| Subsystem | Status |
|---|---|
| Mechanical frame and assembly | ✅ Built (see CAD repo) |
| Buoyancy design | ✅ Built |
| Thruster integration | ✅ Built and tested |
| Power system | ✅ Built |
| Kill switch | ✅ Implemented |
| Qualifying video produced | ✅ Submitted and accepted |
| Vision pipeline | 🔶 In progress |
| IMU + magnetometer fusion | 🔶 Bench-validated; AUV integration in progress |
| Pressure / depth sensing | 🔶 Bench-validated; AUV integration in progress |
| Acoustics (pinger detection) | 🔶 Design stage |
| Full mission autonomy | 🔶 Not reached |
| Competition attendance | ❌ Not possible (travel constraints) |

---

## 7. What integration would have needed

If the team had continued to competition, the critical path was:

1. **Sensor fusion running on the flight controller** at full rate — producing reliable roll/pitch/yaw/depth at ≥ 50 Hz.
2. **Depth and heading hold PID tuning** in real water.
3. **Vision pipeline reliable** in variable lighting and turbidity.
4. **Full state machine** — transitions between tasks without operator input.
5. **Kill switch verification** in a live test environment.

The mechanical and electrical systems were largely complete; the remaining work was on the software integration side, where my sensor contribution sat.




