# Sensor Code

Arduino sketches and library header for the AUV's sensor integration work.

## Files

| File | Purpose |
|---|---|
| `gyro_calibration.ino` | Measures gyro bias at rest — outputs `gx_bias`, `gy_bias`, `gz_bias` |
| `acceleration_calibration.ino` | Measures accelerometer offsets at rest — outputs `ax_off`, `ay_off`, `az_off` |
| `magnetometer_calibration.ino` | Tracks X/Y/Z min/max while the sensor is rotated, for hard-iron and soft-iron derivation |
| `calibration.ino` | Applies the resulting hard-iron offsets and soft-iron scale factors to live magnetometer readings |
| `AUV_imu.ino` | Main IMU pipeline: MPU9250 → calibration → Madgwick filter → Euler angles |
| `Madgwick.h` | Header-only Madgwick AHRS implementation (6-axis `updateIMU()` and 9-axis `update()`) |

## Hardware

- **MPU9250** — 9-axis IMU (accelerometer + gyroscope + magnetometer), used here for accel + gyro only
- **QMC5883L** on GY-271 breakout — external magnetometer for heading reference
- **Pressure transducer** 0–0.5 MPa, G1/4, 5 VDC — depth measurement

## Arduino libraries required

Install via Library Manager:

- `MPU9250_asukiaaa` (MPU9250 driver)
- `QMC5883LCompass` (QMC5883L magnetometer driver)

## Calibration values used

Gyro bias (from `gyro_calibration.ino`):

gx_bias = 1.01
gy_bias = -1.83
gz_bias = -0.38


Accel offsets (from `acceleration_calibration.ino`):

ax_off = -0.00
ay_off = -0.02
az_off = -0.11


Magnetometer hard-iron offsets and soft-iron scales (from `magnetometer_calibration.ino`):

x_off = 2428 y_off = 652.5 z_off = -4255
x_scale = 1.036 y_scale = 0.98 z_scale = 0.99


These values are hard-coded into the sketches. They were derived on the bench; when the sensor is mounted on the vehicle they should be re-derived with the AUV assembled and thrusters idle, because the surrounding steel and magnets change the local field.

## Full documentation

See [`../../docs/sensors.md`](../../docs/sensors.md) for the design, fusion pipeline, and retrospective.

