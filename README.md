# SAUVC 2026 Autonomous Underwater Vehicle

Autonomous Underwater Vehicle built for the **Singapore AUV Challenge 2026** by team **Diyakawa**.

**Status:** Video submission accepted — team shortlisted for the competition. Could not attend in person.

![Status](https://img.shields.io/badge/status-shortlisted-brightgreen)
![Competition](https://img.shields.io/badge/competition-SAUVC%202026-blue)
![Team](https://img.shields.io/badge/team-Diyakawa-purple)
![License](https://img.shields.io/badge/license-MIT-blue)

---

## 📌 About the project

The Singapore AUV Challenge (SAUVC) is an annual student competition where teams build a fully autonomous, untethered underwater vehicle that must complete a series of tasks in a 25 × 16 m indoor pool — all within a 15-minute run.

Each AUV must:

- Fit within a 140 × 100 × 100 cm box
- Weigh no more than 52 kg in air
- Run entirely untethered, powered by ≤ 24 VDC
- Have a visible **kill switch** that stops all thrusters
- Complete tasks without surfacing (surfacing ends the attempt)

The 2026 mission profile includes **navigation** (swim through a gate 16 m away), **target acquisition** (drop a ball into a specific drum), **target reacquisition** (retrieve the ball), and **communication & localization** (bump coloured flares in a commanded order). Full details in [`docs/sauvc-2026-mission.md`](docs/sauvc-2026-mission.md).

Team Diyakawa designed and built an AUV for this mission, submitted a qualifying video, and was shortlisted for the competition. Travel to Singapore was not possible, so the team could not compete in person.

---

## 🎯 My contribution — sensor integration

My role in the team was **sensor integration**, specifically:

- **MPU6050** — 6-axis IMU (accelerometer + gyroscope)
- **HMC5883L** — 3-axis magnetometer (for heading reference)
- **Pressure transducer** (0–0.5 MPa, G1/4, 5 VDC) — depth measurement

I wrote the sensor drivers and fusion logic in Python, ran bench validation, and was working on integrating the sensor stack into the AUV's main control loop when the project ended.

Full technical details in [`docs/sensors.md`](docs/sensors.md).



Full subsystem breakdown in [`docs/architecture.md`](docs/architecture.md).

---

## 📁 Mechanical design & CAD

The complete CAD assembly (SolidWorks parts and assemblies) is maintained in the team's shared repository:

🔗 **[diyakawaofficial/diyakawa_3.0 — Design folder](https://github.com/diyakawaofficial/diyakawa_3.0/tree/main/Design)**

Contents include:

- `Assem1.SLDASM` — main assembly
- `Bouancy.SLDASM` — buoyancy subassembly
- `MirrorThruster Shell.SLDPRT` — thruster housing
- `MirrorHeave top.SLDPRT` / `MirrorHeave bottom.SLDPRT` — heave mechanism
- Side panels, brackets, and support structures
- `Assembly.JPG` — rendered view of the assembled AUV

Maintained by the Diyakawa team. Not duplicated here.

---

## 🎬 Video submission

The team submitted a 30-second video demonstrating underwater, untethered operation of the AUV — the SAUVC qualification requirement.

📹 **Watch:** *(link to be added — will be uploaded to YouTube unlisted)*

Notes on what the video showed: [`docs/video-submission.md`](docs/video-submission.md).

---

## 📷 Photos

Photos of the AUV, sensor setup, and bench testing:

*(to be added to `images/`)*

---

## 👥 Team

**Team name:** Diyakawa

Built collaboratively as a group project. My specific contribution was sensor integration (IMU + magnetometer + pressure sensor).

Full roster: [`team.md`](team.md).

---

## 🗺️ Roadmap

- [x] Mechanical design and assembly (see linked CAD repo)
- [x] Thruster integration and testing
- [x] Electronics and power system
- [x] Qualifying video produced and submitted
- [x] Shortlisted for SAUVC 2026
- [x] Sensor drivers written and bench-validated (MPU6050, HMC5883L, pressure)
- [ ] Full sensor fusion integration into the flight controller
- [ ] Field testing in a competition-standard pool
- [ ] Attendance at SAUVC (deferred — travel not possible in 2026)

---

## 📄 License

MIT — see [`LICENSE`](LICENSE). CAD files are part of the Diyakawa team repository.

---

## 📬 Contact

**Induwara Wijesinghe**  
Mechanical Engineering Undergraduate — Mechatronics  
University of Moratuwa  
📧 induwarawijesinghe369@gmail.com

---


