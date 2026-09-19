# Sensor Integration

> My contribution to the Diyakawa AUV. This document covers the sensors I was responsible for, the fusion approach, and the state of integration when the project ended.

---

## Scope

The flight controller needs a reliable estimate of the AUV's **orientation** and **depth** in order to close the low-level control loops (heading hold, depth hold). Without these, the mission state machine can't do anything useful.

My role was to build that estimate from:

| Sensor | Measures | Interface | Purpose |
|---|---|---|---|
| **MPU9250** | 3-axis acceleration + 3-axis angular rate | I²C | Fast attitude updates |
| **QMC5883L** (GY-271) | 3-axis magnetic field | I²C | Absolute heading reference |
| **Pressure transducer** | 0–0.5 MPa hydrostatic pressure | Analog (0–5 V) | Depth |

The MPU9250 provides fast but drifting orientation estimates. The QMC5883L magnetometer corrects gyro drift on the yaw axis. The pressure sensor converts depth into a control variable for the heave axis.

The sensor stack runs on a single-board computer, with the fusion output feeding the flight controller's PID loops.

---

## 1. MPU9250 — inertial measurement unit

### Overview

- 9-axis MEMS package (accelerometer + gyroscope + magnetometer in one chip)
- I used the **accelerometer and gyroscope** from this chip; the internal magnetometer was not used (see §2 for why)
- I²C interface (`0x68` default address)
- 16-bit signed output per axis
- Library used: `MPU9250_asukiaaa`

### What I did

**Driver and raw reading**

- Initialised the sensor over I²C with `mpu.setWire(&Wire)`, then `mpu.beginAccel()` and `mpu.beginGyro()`.
- Read updated values in the main loop via `mpu.accelUpdate()` and `mpu.gyroUpdate()`, pulling physical units directly from the library.


**Calibration — the important part**

I wrote two dedicated calibration sketches:

1. **`gyro_calibration.ino`** — with the sensor held completely still, sum the gyro outputs over many samples and divide by the sample count to get the **bias** on each axis. After calibration, the corrected readings are:

gx_bias = 1.01
gy_bias = -1.83
gz_bias = -0.38


These are subtracted from every raw reading before fusion.

2. **`acceleration_calibration.ino`** — with the sensor flat and stationary, sum the accelerometer outputs and divide by the sample count to get the **accel offsets** (which account for small manufacturing bias). After calibration:

3. ax_off = -0.00
ay_off = -0.02
az_off = -0.11


These offsets are subtracted from raw accel readings. The Z axis reads approximately 1 g when flat, confirming the calibration.

**Angle estimation — Madgwick filter**

Rather than a complementary filter, I implemented the **Madgwick AHRS algorithm** in `Madgwick.h`. Key design choices:

- Sample rate: **100 Hz**
- Algorithm gain: **β = 0.04**
- Two modes implemented:
- `updateIMU()` — gyro + accel only (6-axis)
- `update()` — gyro + accel + magnetometer (9-axis)

The Madgwick filter uses **gradient descent optimisation** on the error between the predicted orientation and the measured accelerometer/magnetometer vector. This is more robust than a complementary filter because it converges to a proper quaternion rather than blending two independent angle estimates.

The filter outputs a quaternion `(q0, q1, q2, q3)`, which I convert to Euler angles for the flight controller:


float getRoll() {
 return atan2(2*(q0*q1 + q2*q3),
              1 - 2*(q1*q1 + q2*q2)) * RAD_TO_DEG;
}

float getPitch() {
 float v = constrain(2*(q0*q2 - q3*q1), -1.0f, 1.0f);
 return asin(v) * RAD_TO_DEG;
}

float getYaw() {
 return atan2(2*(q0*q3 + q1*q2),
              1 - 2*(q2*q2 + q3*q3)) * RAD_TO_DEG;
}

In AUV_imu.ino I ran the 6-axis variant (updateIMU) on the bench; the 9-axis variant was ready but the magnetometer was not yet feeding into the live loop at the time the project ended.

Known limitations
The accelerometer is sensitive to linear (non-gravity) acceleration, so during fast thruster bursts the accel-derived correction briefly degrades. The Madgwick filter's β parameter controls how strongly the accel correction is applied — 0.04 was tuned to be responsive without being jumpy on the bench.

The 6-axis variant can't correct yaw drift at all — that's why the magnetometer matters.

2. QMC5883L — magnetometer (GY-271 module)
Overview
3-axis magnetoresistive sensor on the GY-271 breakout board

I²C interface (0x0D default address)

Library used: QMC5883LCompass

Used as an absolute heading reference to correct yaw drift

Why the QMC5883L and not the MPU9250's internal magnetometer
The MPU9250 does include a magnetometer (AK8963), but on most breakout boards it is either not routed out, poorly isolated from the accel/gyro, or produces noisier readings than a dedicated external magnetometer. The GY-271 module I used gives cleaner data and — critically — can be mounted away from the MPU9250, which matters because the magnetometer needs to be as far as possible from current-carrying wires and motors.

What I did
Driver and raw reading

Initialised with mag.init().

Read raw X, Y, Z values in the loop with mag.read() followed by mag.getX(), mag.getY(), mag.getZ().

Hard-iron calibration — magnetometer_calibration.ino

The hard-iron effect is a constant magnetic offset caused by nearby ferromagnetic materials. It shifts the centre of the sensor's response sphere away from the origin.

I wrote a calibration sketch that:

Continuously reads X, Y, Z

Tracks the running minimum and maximum on each axis

Prints them live so I could rotate the sensor through all orientations

After rotating through 360° in all axes, the min/max on each axis gives the hard-iron offset:

x_offset = (x_max + x_min) / 2
y_offset = (y_max + y_min) / 2
z_offset = (z_max + z_min) / 2


Soft-iron calibration

Soft-iron effects distort the response sphere into an ellipsoid — the scale factors differ between axes.

Using the same min/max data:

avg_radius = ((x_max - x_min) + (y_max - y_min) + (z_max - z_min)) / 3
x_scale = avg_radius / (x_max - x_min)
y_scale = avg_radius / (y_max - y_min)
z_scale = avg_radius / (z_max - z_min)

Final calibration values (in calibration.ino):

x_off = 2428       y_off = 652.5     z_off = -4255
x_scale = 1.036    y_scale = 0.98    z_scale = 0.99


These are applied to every reading before normalising the vector to unit length for the Madgwick filter.

Known limitations
The calibration values above were obtained on the bench, away from the AUV. Once the magnetometer is mounted on the vehicle, the thrusters' permanent magnets and the steel in the frame will re-introduce hard-iron and soft-iron distortion. The values need to be re-derived with the sensor in situ.

The plan was to mount the QMC5883L as far as physically possible from the thrusters and ESCs, and to recalibrate with the AUV assembled but idle.

3. Pressure transducer — depth
Overview
Industrial pressure transducer

Range: 0–0.5 MPa (0–5 bar), corresponding to roughly 0–50 m of water depth

Thread: G1/4 (standard pipe fitting for waterproof mounting)

Output: 0–5 V analog, linear with pressure

Supply: 5 VDC

What I did
Signal acquisition

Routed the analog output to the flight controller's ADC.

Sampled at ~20 Hz and applied a moving average to reduce noise from thruster vibration.

Pressure → depth conversion

The transducer reports gauge pressure (relative to atmosphere), so the reading corresponds to the water column above the sensor:

depth_m = P_gauge_Pa / (ρ · g)

With:

ρ = 1000 kg/m³ (fresh water — competition pools vary slightly)

g = 9.81 m/s²

Zeroing: before each run, the sensor is held at the surface and the ambient reading stored as the zero point. This accounts for atmospheric drift and sensor offset.

Calibration: the analog output was mapped using the manufacturer's transfer function (0 V = 0 MPa, 5 V = 0.5 MPa), verified against a known depth in a test tank.

Control output

Depth is fed to the PID loop that drives the heave axis. A setpoint of e.g. -1.5 m means the sub holds 1.5 m below the surface.

Known limitations
The transducer's 0.5 MPa full-scale corresponds to ~50 m of depth, but SAUVC's pool is only ~2 m deep. Resolution at the depth actually used was therefore a small fraction of the sensor's range. A 0–0.1 MPa sensor would have been a better choice.

Temperature coefficient of the transducer was not compensated — negligible at the depth range used.

        ┌────────────┐                  ┌─────────────────┐
        │  MPU9250   │                  │   QMC5883L      │
        │ accel+gyro │                  │  magnetometer   │
        └─────┬──────┘                  └────────┬────────┘
              │                                   │
      ┌───────▼────────┐                 ┌────────▼────────┐
      │ Apply calib:   │                 │ Apply calib:    │
      │ bias subtract  │                 │ hard-iron off   │
      │ unit-normalise │                 │ soft-iron scale │
      └───────┬────────┘                 │ unit-normalise  │
              │                           └────────┬────────┘
              │                                    │
              └──────────────┬─────────────────────┘
                             ▼
                    ┌────────────────────┐
                    │  Madgwick filter   │
                    │  update / updateIMU│
                    │  → quaternion      │
                    └─────────┬──────────┘
                              │
                              ▼
                    ┌────────────────────┐
                    │  Roll, Pitch, Yaw  │
                    │  (Euler angles)    │
                    └─────────┬──────────┘
                              │
        ┌───────────────┐     │
        │ Pressure      │     │
        │ transducer    │     │
        │ (0–0.5 MPa)   │     │
        └───────┬───────┘     │
                │             │
                ▼             │
        ┌───────────────┐     │
        │ Depth (m)     │     │
        │ from ρ·g·h    │     │
        └───────┬───────┘     │
                │             │
                └──────┬──────┘
                       ▼
        ┌────────────────────────────┐
        │   State estimate:          │
        │     roll, pitch, yaw,      │
        │     depth                  │
        └──────────────┬─────────────┘
                       │
                       ▼
        ┌────────────────────────────┐
        │   PID loops                │
        │   + mission state machine  │
        └────────────────────────────┘


The attitude pipeline (AUV_imu.ino) was bench-validated with the MPU9250 alone using the 6-axis Madgwick variant. The magnetometer fusion (update() variant of the filter, using the QMC5883L and the calibration values above) was implemented and tested in isolation but the wiring into the AUV's live control loop was still in progress when the project ended.


5. Code
All sensor code is in ../src/sensors/:

File	Purpose
acceleration_calibration.ino	Averages accel readings at rest to derive ax_off, ay_off, az_off
gyro_calibration.ino	Averages gyro readings at rest to derive gx_bias, gy_bias, gz_bias
magnetometer_calibration.ino	Tracks min/max on each axis while rotating the sensor, for hard/soft-iron derivation
calibration.ino	Applies the hard/soft-iron values to live magnetometer readings
AUV_imu.ino	Runs the MPU9250 → Madgwick → Euler angles pipeline at 100 Hz
Madgwick.h	Header-only implementation of the Madgwick AHRS algorithm (both 6-axis and 9-axis variants)


Final calibration values (in calibration.ino):
