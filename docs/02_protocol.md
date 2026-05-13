# SOUL Telemetry Protocol

> [!TIP]
> This protocol is still intentionally small. Thats not a bug, thats us trying to not explode the project too early.

## Big Picture

SOUL uses a framed TCP connection.

That means:

1. the sender creates a telemetry JSON payload
2. the payload gets wrapped by the frame codec
3. the receiver decodes it
4. the receiver stores the data and sends an ACK back

The framing and ACK flow have stayed the same even after adding multiple satellites, which is honestly nice because that could have gotten messy super fast.

## Telemetry Payload

Each telemetry frame currently carries:

| Field | Meaning |
|---|---|
| `sat_id` | satellite identifier like `SAT_1` |
| `sequence` | per-satellite increasing frame number |
| `timestamp_ms` | time created on the sender side |
| `battery` | current battery level |
| `temp_c` | current temperature |

Example payload:

```json
{
  "sat_id": "SAT_2",
  "sequence": 14,
  "timestamp_ms": 1776092696028,
  "battery": 91.25,
  "temp_c": 42.50
}
```

## ACK Shape

The receiver replies with a tiny ACK object:

```json
{
  "type": "ack",
  "sequence": 14
}
```

It is very plain right now, but that is enough for the current sender loop.

## Multi-Satellite Sending

The sender now owns three simulation states:

- `SAT_1`
- `SAT_2`
- `SAT_3`

In each outer send loop:

1. generate the next frame for a satellite
2. if there is a frame, encode and send it
3. wait for the normal ACK
4. move to the next satellite

### Signal Loss Rule

`SAT_3` can enter short fake signal-loss windows.

During those ticks:

- it skips sending a frame
- it does **not** crash the client
- it does **not** disconnect the whole TCP session
- the UI later notices the stale telemetry and marks it degraded/disconnected

That part is kinda simple but also kinda neat.

## Database Mapping

When a frame arrives, the receiver stores:

| SQLite Column | Source |
|---|---|
| `Satellite_name` | `sat_id` |
| `Sequence` | `sequence` |
| `timestamp_ms` | `timestamp_ms` |
| `battery` | `battery` |
| `temperature` | `temp_c` |
| `received_ms` | receiver-side arrival time |

## Notes

- No schema change was needed for multi-satellite support
- Link state in the UI comes from `received_ms` age, not from an explicit protocol field
- Packet size shown in the UI is still estimated from the JSON payload shape, because we dont store it directly yet
