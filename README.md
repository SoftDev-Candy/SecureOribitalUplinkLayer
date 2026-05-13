<div align="center">

# SOUL
### Secure Orbital Uplink Layer

**A C++/Qt satellite telemetry project where fake satellites send live data, a receiver stores it, and the operator UI shows what is going on in real time.**

![C++](https://img.shields.io/badge/C%2B%2B-20-blue?style=for-the-badge&logo=c%2B%2B)
![Qt](https://img.shields.io/badge/Qt-Widgets%20%7C%20OpenGL-2F8D46?style=for-the-badge&logo=qt)
![Boost](https://img.shields.io/badge/Boost-Asio%20%7C%20JSON-orange?style=for-the-badge&logo=boost)
![SQLite](https://img.shields.io/badge/SQLite-Database-003B57?style=for-the-badge&logo=sqlite)
![Status](https://img.shields.io/badge/Status-Active%20Project-yellow?style=for-the-badge)

![SOUL demo](docs/SOUL.gif)

</div>

## What This Project Is

SOUL is a college-style space systems project built in C++.

The idea is pretty simple:

- satellites generate telemetry
- the sender pushes that telemetry over framed TCP
- the receiver stores it in SQLite
- the Qt operator dashboard reads the database and shows the current status
- the OpenGL orbit scene gives a visual view of the Earth and the satellites moving around it

So instead of being just a backend app or just a UI app, SOUL tries to connect the whole little pipeline together in one place.

## What It Does Right Now

Right now the project simulates **three satellites**:

- `SAT_1` acts mostly normal
- `SAT_2` drains battery faster
- `SAT_3` heats up and can go quiet for short signal-loss windows

Each satellite sends telemetry frames using the same framed TCP protocol.  
The receiver stores the packets in SQLite, and the UI shows:

- a satellite selection table
- live link status based on how fresh the last packet is
- recent telemetry history for the selected satellite
- an Earth scene with satellites orbiting around it

When nothing is selected, the scene can still show the satellites so the operator is not just staring into the void, which is honestly a bit nicer.

## Project Flow In Normal Words

1. The sender makes telemetry frames for `SAT_1`, `SAT_2`, and `SAT_3`.
2. Frames are sent over TCP with the existing length-prefixed packet format.
3. The receiver reads the frame, stores the telemetry row in SQLite, and sends an ACK back.
4. The Qt UI reads the newest rows from the database and updates the tables, labels, and orbit view.

That is basically the full loop. Nothing too magical, just a lot of moving parts cooperating hopefully.

## Main Features

- Multi-satellite telemetry simulation with simple behavior states
- Framed TCP messaging with ACK handling
- SQLite-backed telemetry storage
- Qt Widgets operator dashboard
- Satellite list plus detailed telemetry table
- OpenGL orbit scene with Earth, satellites, orbit trails, glow markers, and camera controls
- Small reusable rendering helpers like `TexturedMesh`

## Demo

- GIF preview: [docs/SOUL.gif](docs/SOUL.gif)
- Video: [docs/SOUL.mp4](docs/SOUL.mp4)
- Kanban board: [docs/KanbanBoard.png](docs/KanbanBoard.png)
- Timeline view: [docs/TimeLineview.png](docs/TimeLineview.png)

## Project Layout

```text
SOUL/
|-- Common/                Shared telemetry frame + framing helpers
|-- simulation/            Fake satellite state and behavior logic
|-- SatelliteSender/       TCP sender/client side
|-- ReceiverServer/        TCP receiver/server side
|-- Database/              SQLite database setup and inserts
|-- ui/                    Qt Widgets operator dashboard
|-- OrbitalRendering/      OpenGL Earth/satellite rendering code
|-- docs/                  Project writeups, media, and planning boards
|-- scratchTests/          Small local checks and experiments
`-- external/sqlite/       Bundled SQLite source
```

## Main Build Targets

- `SOUL`  
  Receiver/server side of the telemetry system

- `Client`  
  Sender/client side that simulates multiple satellites

- `UserInterface`  
  Qt dashboard with the telemetry tables and orbit scene

- `Test`  
  Scratch validation target for small checks

## Tech Stack

- C++20
- Qt Widgets
- OpenGL
- Boost.Asio
- Boost.JSON
- SQLite
- CMake

## Docs

- Requirements: [docs/01_requirement.md](docs/01_requirement.md)
- Protocol notes: [docs/02_protocol.md](docs/02_protocol.md)
- Architecture notes: [docs/03_architecture.md](docs/03_architecture.md)

## Running The Pieces

At a high level, the normal flow is:

1. build the project
2. start the receiver/server
3. start the Qt UI
4. start the sender/client
5. watch telemetry appear in the UI and select a satellite to inspect it

If the database is receiving rows, the UI should pick them up and show link state changes based on how old the latest packet is.

## Current Notes

- The UI hides the detailed telemetry panel until a satellite is selected
- The renderer reuses one satellite mesh for multiple satellites
- Some status fields in the UI are computed from stored data, like link freshness and latency
- The project is still growing, so a few rough edges are still there, but the core loop is working pretty nice now

## Documentation Style Note

The docs in this repo are written to be readable first.  
The goal is not to sound ultra-corporate. The goal is to explain what the project is doing without making it feel scary for the next person reading it.
