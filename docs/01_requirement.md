## SOUL Requirements Snapshot

> [!NOTE]
> This doc is the "where are we actually at now" version, not the super early v0 dream list.
> Some bits are still moving a lil bit, but the shape is finally real enough to write down proper-ish.

![SOUL Preview](SOUL_preview.gif)

## Project Goal

Build a small orbital telemetry sim in C++/Qt/OpenGL where fake satellites send framed TCP telemetry, the receiver stores it in SQLite, and the operator UI shows health, link state, and a live-ish orbit scene.

## What The App Does Right Now

SOUL currently has these main pieces working together:

- A TCP telemetry sender/client that can simulate `SAT_1`, `SAT_2`, and `SAT_3`
- A receiver/server that accepts framed packets and sends ACKs back
- SQLite storage for incoming telemetry rows
- A Qt operator UI with:
  - a satellite selection table
  - a telemetry table
  - detail status panels
- An OpenGL orbit scene with:
  - Earth day/night texturing
  - camera zoom + drag orbit
  - multiple visible satellites
  - orbit trails / glow helpers

## Current MVP Scope

### Must Have

1. Length-prefixed framed TCP telemetry packets
2. ACK reply from receiver after valid frame handling
3. SQLite persistence of received telemetry
4. Multi-satellite sender flow
5. Per-satellite link health based on stale packet age
6. Qt operator UI for selecting a satellite and reading its recent data
7. OrbitView scene with Earth plus satellite rendering

### Nice To Have

1. Better alerting and warning banners
2. More real packet-loss math instead of simple bursts
3. Slightly nicer satellite labels or scene overlays
4. More historical graphs in the UI, maybe later when we dont suffer

### Not In Scope Right Now

1. Real TLE propagation
2. Real RF / antenna / Doppler simulation
3. Hardware control
4. Full physics-grade orbital mechanics

## Satellite Behavior Rules

The current simulation layer uses simple behavior states instead of trying to be NASA on day one:

| Satellite | Mode | Behavior |
|---|---|---|
| `SAT_1` | `Nominal` | stable-ish battery and temperature |
| `SAT_2` | `PowerDrain` | battery drains faster than normal |
| `SAT_3` | `SignalLoss` | heats up slowly and sometimes skips sending frames |

That last one is kinda important because it lets the UI show `Degraded` and `Disconnected` from stale `received_ms` values without killing the whole client.

## UI Expectations

When telemetry is available:

- the satellite table should list the latest row per satellite
- the operator can click one satellite to focus the detail panel
- the telemetry table should show recent rows for the selected satellite
- OrbitView should show the selected satellite clearly

When no telemetry is available:

- the app should show `No active satellite connection`
- detailed panels stay hidden
- the scene should not pretend a selected live satellite exists

## Visual Expectations

OrbitView should stay readable first and fancy second:

- Earth should show a readable day/night split
- satellites should be visible even before selection
- selecting a satellite should make the scene less cluttered
- orbit paths should be different enough to tell apart

## Media

- Demo video: [SOUL.mp4](SOUL.mp4)
- Demo GIF: [SOUL_preview.gif](SOUL_preview.gif)
  - This one is the quick look version for the docs, which is honestly way easier on the eyes when you just wana peek fast.
