
### Goal (1 sentence):

To make a satellite telemetry application with TCP protocol and telemetry ingestion server
with link impairment simulation , health state detection  and Multi-Client handling in C++20

### MVP demo (1 paragraph):

Make an application that shows real time data converted from a stateliness telemetry data 
into data user can understand in clear manner with clear distinction of what we are working.

#### Must-have (max 6 bullets): 

1. TCP multi-client ingestion (concurrent handling).
2. Structured telemetry frame (length-prefixed JSON).
3. Sequence number tracking.
4. Timestamp-based staleness detection.
5. Link impairment engine (loss + latency simulation).
6. Link health state machine (UP / DEGRADED / DOWN).
7. Per-satellite metrics tracking.
8. Clean shutdown & error handling.
9. Basic logging.
10. Unit tests for frame parsing + impairment engine.
11. Burst loss simulation (contested environment)
12. Outage windows
13. Spoofed frame detection (sequence mismatch)
14. Replay detection (old timestamp)
15. Telemetry freshness alarms
16. Health-state transitions logging

#### Nice-to-have (max 4 bullets):

1. A Nice Qt dashboard
2. Simple orbit animation ( circular path - Mock Would work initially )
3. Historical telemetry buffer (ring buffer)
4. Alert threshold configuration



#### Not in v0 (max 4 bullets):

1. Real RF modulation modeling
2. Real Doppler shift computation
3. Real TLE propagation
4. Real antenna physics
5. Hardware-level simulation

- Note :
No Orbit Math in V0 don't do that to yourself be safe