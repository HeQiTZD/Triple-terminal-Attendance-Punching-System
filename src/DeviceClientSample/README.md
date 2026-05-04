# Device client sample (receiver)

This folder contains a **minimal reference** implementation of how a device client can receive sync data from `AttendanceServer`:

- **Control channel**: JSON messages delimited by `\n`
- **Binary channel**: after receiving `face_sync_item_header`, the server sends a binary frame of:
  - 4-byte big-endian length prefix
  - raw payload bytes (face feature vector)

Files:
- `DeviceSyncClient.h/.cpp`: a small `QTcpSocket` receiver with a state machine (JSON line -> binary length -> binary payload).

How to integrate:
- Copy the parsing/state-machine logic into your real attendance device app.
- Implement real local DB writes in:
  - `applyPersonSync(...)`
  - `applyFaceItem(...)`

Note:
- This sample is **not** compiled by the server app target; it's provided as a reference for the device side.

