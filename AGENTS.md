# AGENTS.md — AttendanceAdmin

## What is this

Qt 6.8+ / C++17 Windows desktop app. A TCP client that talks to a考勤设备服务端 via a custom JSON-over-TCP protocol. QML UI + C++ service layer. Single executable, no packages, no third-party deps beyond Qt.

## Build

```bash
# From repo root — the only supported build path:
rebuild_x64.bat
```

This calls VS2022 `vcvarsall.bat`, runs CMake with Ninja to `out/build/debug`, and builds. Requires:
- MSVC 2022 (Visual Studio 18 Insiders path is hardcoded in the .bat — adjust if using a different VS install)
- Qt 6.10.2 at `C:\Qt\6.10.2\msvc2022_64` (set via `-DCMAKE_PREFIX_PATH`)

Output: `out/build/debug/AttendanceAdmin.exe`

No CTest, no `cmake --test`, no linter, no formatter, no typecheck. If you add tests, register them in CMakeLists.txt.

## Architecture (3 layers)

```
QML UI (ui/)  →  C++ Services (src/)  →  TcpConnectionManager (src/Network/)  →  TCP server
```

1. **UI layer** (`ui/`): QML pages, components, models. Communicates with services via QML property bindings and `Q_INVOKABLE` methods.
2. **Service layer** (`src/`): One `*Server` or `*Service` class per domain (Person, Device, Attendance, Face, User, Rbac, Config, Event, Log). Each holds a `TcpConnectionManager*` and sends/receives JSON messages.
3. **Network layer** (`src/Network/TcpConnectionManager`): Single TCP connection, JSON line protocol, heartbeat, auto-reconnect, token refresh, pending request tracking by `msgId`.

All services are instantiated in `main.cpp` and injected into QML via `engine.setInitialProperties()`.

## Adding a new service module

1. Create `src/YourModule/YourServer.h` and `.cpp`
2. Inherit `QObject`, add `setTcpManager(TcpConnectionManager*)`, use `Q_INVOKABLE` for QML methods
3. Add message type constants in `src/Protocol/protocol.h`
4. Register in `CMakeLists.txt` under `SOURCES`
5. Instantiate and wire in `main.cpp`: `auto *svc = new YourServer(&app); svc->setTcpManager(tcpManager);`
6. Add to `engine.setInitialProperties({ ... })`

## Adding a new QML page

1. Create `ui/pages/PageYourFeature.qml`
2. Register in `CMakeLists.txt` under `QML_FILES`
3. Add navigation in `ui/Main.qml` (sidebar + PageHost route)

## QML models are singletons

`ui/models/*.qml` and `ui/theme/Theme.qml` are marked `QT_QML_SINGLETON_TYPE TRUE` in CMakeLists.txt. Do not add new QML models there without adding the same property.

## Protocol conventions

- All message type strings live in `src/Protocol/protocol.h`. Use the constants, not hardcoded strings.
- Message envelope: `{ type, msgId, from, to, ts, data }`. Responses use `inReplyTo` referencing the request's `msgId`.
- Request/response pattern: service calls `tcpManager->sendMessage(msg, callback)`. The callback receives the response object.
- Error codes are in `Protocol::ErrorCode` namespace (0 = success, 1001–6002 for various failures).
- Push messages come via `EventSubscriptionService::serverPushReceived` signal, dispatched by message type.

## Code style

- Qt naming conventions: PascalCase for classes, camelCase for methods/variables
- Qt5-style connects: `QObject::connect(sender, &Sender::signal, receiver, &Receiver::slot)`
- `Q_PROPERTY` for all state exposed to QML, with proper `NOTIFY` signals
- `Q_INVOKABLE` for methods callable from QML
- BOM marker (`﻿`) at the top of source files
- Use `QStringLiteral()` and `QStringLiteral("...")` for all string constants (no `""` raw literals in hot paths)

## Runtime environment

Set in `main.cpp`:
- `QT_QUICK_CONTROLS_STYLE=Fusion`
- `QSG_RHI_BACKEND=d3d11`
- `Qt::ColorScheme::Light` (forced)

## Gotchas

- `SessionManager` is a child of `TcpConnectionManager` (parent-child ownership in constructor) — this ensures destruction order.
- `g_logWriter` is a global pointer used by the custom `qtMessageHandler`. It must be set before QML loads.
- There is no database in this project. All state lives on the remote server; the client is stateless beyond session tokens.
