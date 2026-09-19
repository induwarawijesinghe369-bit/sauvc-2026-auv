# SAUVC 2026 — Mission Summary

> A working summary of the **Singapore AUV Challenge 2026** rulebook for readers unfamiliar with the competition. For the authoritative version, see the official rulebook at [sauvc.org/rulebook](https://sauvc.org/rulebook/).

---

## What SAUVC is

SAUVC is an annual student underwater robotics competition held in Singapore. Teams build a fully **autonomous, untethered** underwater vehicle that must complete a set of tasks in an indoor swimming pool, within a prescribed time, without human intervention mid-run.

The 2026 competition is held in a **25 m × 16 m** pool. Teams that pass a video qualification stage are invited to Singapore to compete in the qualification and final rounds.

---

## The AUV rules that shape the design

| Constraint | Value |
|---|---|
| Maximum bounding box | 140 × 100 × 100 cm |
| Size bonus (if under) | 70 × 50 × 50 cm → +10 points |
| Maximum weight (in air) | 52 kg |
| Weight bonus (if under) | 42 kg → +10 points |
| Power source | Self-contained, ≤ 24 VDC |
| Tether | Not allowed (except for sensor calibration with the vehicle held stationary) |
| Kill switch | Required, must stop all thrusters, must be visibly marked and diver-accessible |
| Propellers | Must be shrouded |
| Compressed gas | ≤ 6 bar |
| Lasers | Class 2 or lower, and never pointed at spectators |
| Battery | Certified lithium allowed; no leaks permitted |

The rules effectively force a self-contained vehicle with its own power, sensing, control, and safety systems. No ground station can drive the sub — only a top-side module in the start zone can communicate.

---

## The mission — four tasks

### Task 1 — Navigation (mandatory)

The AUV starts from a marked **1 m-wide section of pool wall**, submerges, and swims through a **gate hanging from the water surface** approximately 10 m away (in qualifiers) / at the bottom of the pool ~16 m away (in finals).

- Gate: 150 cm wide, 100 cm tall, striped red/green on port/starboard.
- An **orange flare** (a vertical pole spanning the pool's full depth, ~15 cm diameter) may be placed before the gate — touching it causes an immediate abort.
- Passing completely through the gate earns **15 points**.

Navigation must be completed **before any other task can be attempted**. This means the AUV has to reliably find and swim through a narrow opening underwater, without a tether and without surfacing.

### Task 2 — Target Acquisition

Four coloured **drums** (60 cm diameter, 30 cm deep) sit on the pool floor:

- One drum is **blue** (randomly assigned)
- The other three are **red**
- One of the red drums contains an **acoustic pinger** (45 kHz, RJE International ULB-362B)

The AUV must **drop a ball** into a drum. Points:

| Drum choice | Points |
|---|---|
| Blue drum | 30 |
| Red drum with pinger | 50 |
| Any other red drum | 10 |

The AUV has to distinguish blue from red (vision) and locate the pinger (acoustics) — a two-modality sensing task.

### Task 3 — Target Reacquisition

Only attemptable after Task 2 succeeds and the AUV has passed through the gate once more.

The AUV must **reacquire the target drum** and **pick the dropped ball back up**, holding it until the end of the run.

- Worth **60 points**.

This requires a grasping mechanism and enough state memory to return to a previously visited location.

### Task 4 — Communication & Localization

Three coloured **flares** (red, yellow, blue) are placed somewhere in the arena. After the AUV completes Task 1, the team is told the order in which the flares must be bumped — for example, `R-B-Y`.

The team relays this order to the AUV via its top-side communication equipment.

- Each flare bumped (by contact or proximity) → **20 points**
- All three bumped in the commanded order → **+60 points**

This is the only task that explicitly requires **underwater communication** from top-side to the AUV during a run.

---

## Scoring and outcomes

- **Surfacing** at any time ends the attempt. A 5-point "end bonus" is awarded for surfacing cleanly.
- **Auto-abort** if the AUV touches the pool floor or wall cumulatively for > 10 s, or 5 discrete touches.
- **Timing bonus** if at least two tasks are completed:


- **Penalties:** 2 points per gate touch, 5 points per pool floor / wall touch.

The team's best attempt counts. Multiple attempts allowed within the 15-minute slot.

---

## Qualification

To qualify for the finals, an AUV must:

1. Swim from the starting line, **through the gate** (forward run)
2. Perform a **U-turn**
3. Pass **through the gate again** (reverse run)

That's **2 qualification points** for a full run. Top 15 teams by score advance to the finals.

The vehicle must complete this without surfacing, touching the bottom, touching the walls, or touching the gate.

---

## Video submission

All teams must submit a **30-second video** before the competition. The video must show:

- **Tether-less operation** — the AUV swimming underwater for **at least 10 seconds** with no physical link
- **Kill switch demonstration** — clearly showing that pressing the switch stops all thrusters immediately

Videos are reviewed by the organising committee; around 35 teams are shortlisted to attend the competition.

Team Diyakawa submitted this video and was shortlisted. Travel to Singapore was not possible in 2026, so the team did not attend the competition in person.

---

## Why this is a hard problem

Even a single SAUVC task requires:

- **Robust autonomous navigation** without GPS (underwater)
- **Real-time vision** in murky, variable-lighting water
- **Depth and heading control** using pressure + IMU sensing
- **Untethered power and communication**
- **Fail-safe behaviour** — because once the AUV is underwater, nobody can help it

The mission as a whole is a **systems integration problem** first and a robotics problem second. Every subsystem has to work reliably, in water, on the first try.


