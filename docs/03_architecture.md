# Architecture

> [!WARNING]
> This is a minimal architecture snapshot. The design, data flow, and implementation details will likely change over the next few weeks as the project continues to evolve.

## Current Shape

SOUL is currently split into a small set of focused modules:

- `Common/` contains the shared telemetry frame model and frame codec
- `Hub/` acts as the telemetry sender/client
- `Server/` acts as the telemetry receiver/server and sends acknowledgements back
- `Database/` contains the SQLite setup for received-packet storage
- `scratchTests/` holds temporary checks today, with proper unit tests planned next
- `docs/` holds requirements, protocol notes, and planning assets

## Current Flow

```text
Hub/client
  -> create telemetry frame
  -> encode and send over TCP

Server/receiver
  -> decode frame
  -> track latest telemetry state
  -> persist received packets through SQLite work in Database/
  -> send acknowledgement back to the client
```

## Notes

- Miro is being used for research notes, timeline planning, and kanban tracking.
- Protocol details live in [02_protocol.md](02_protocol.md).
- This document is intentionally high level until the design settles.
