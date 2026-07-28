# AGENTS.md — AttendanceAdmin

## What is this

Qt 6.8+ / C++17 Windows desktop app. TCP client that talks to a 考勤设备服务端 via JSON-over-TCP. QML UI + C++ service layer. No database — all state lives on the remote server. No third-party deps beyond Qt.

## Build (only supported path)

```bat
rebuild_x64.bat
```

Calls VS2022 `vcvarsall.bat`, CMake+Ninja to `out/build/debug`, produces `AttendanceAdmin.exe`. Requires:
- MSVC 2022 — path is `C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\...` (adjust if using standard VS)
- Qt 6.10.2 at `C:\Qt\6.10.2\msvc2022_64`
- CMakeLists.txt mandates `qt_standard_project_setup(REQUIRES 6.8)` — minimum Qt 6.8

No test, lint, formatter, or typecheck infra exists.

## Architecture

```
QML UI (ui/)  →  C++ Services (src/)  →  TcpConnectionManager  →  TCP server
```

**3 layers:**
1. **UI** (`ui/`): QML pages + components + singleton models. Talks to services via `Q_INVOKABLE` + property bindings.
2. **Services** (`src/`): One class per domain. Naming: `*Server` (Person, Device, ConfigDeploy, Face, Rbac, User) or `*Service` (Attendance, EventSubscription). Each holds a `TcpConnectionManager*`.
3. **Network** (`src/Network/TcpConnectionManager`): Single TCP connection, JSON line protocol (`\n` delimited), 4-byte-big-endian-length-prefixed binary frames (for face data sync), heartbeat (80% of server-advertised interval), auto-reconnect (exponential backoff, max 30s), JWT token refresh (5 min before expiry), pending request tracking by `msgId`.

All instantiated in `main.cpp:56-167`, injected into QML via `engine.setInitialProperties({...})`.

## Service naming — watch for mismatch

- `*Server`: Person, Device, ConfigDeploy, Face, Rbac, User
- `*Service`: Attendance, EventSubscription

When adding a new service, use the existing pattern but double-check the suffix.

## Protocol notes

- All type strings live as `inline const QString` in `src/Protocol/protocol.h:5-197`. Never hardcode.
- Envelope: `{ type, msgId, from, to, ts, data }`. Response uses `inReplyTo` = request's `msgId`.
- `responseType(reqType)` helper (protocol.h:170-172) appends `".response"` — use it.
- Error codes: `Protocol::ErrorCode` namespace (0=success, 1001–6002). See protocol.h:176-193.
- Push messages: `EventSubscriptionService::serverPushReceived` signal, dispatched by message type.
- Binary frames: 4-byte big-endian length prefix + payload. Used for face sync (`face_sync_*`).
- Password is always sanitized to `"***"` in history logging (TcpConnectionManager.cpp:519-530).
- `msgId` is `QUuid::createUuid().toString(QUuid::WithoutBraces)` — UUID without braces.
- `SessionManager` is a child of `TcpConnectionManager` (parent ownership in constructor main.cpp:82) — destruction order depends on this.
- `g_logWriter` global pointer (main.cpp:26) must be set before QML loads; used by custom `qtMessageHandler`.

## Key conventions

- Qt naming (PascalCase classes, camelCase methods/vars). Qt5-style connects (`QObject::connect(sender, &S::sig, recv, &R::slot)`).
- `Q_PROPERTY` + `NOTIFY` for QML state. `Q_INVOKABLE` for QML-callable methods.
- `QStringLiteral()` for all string constants.
- UTF-8 BOM (`0xEF 0xBB 0xBF`) at top of most (but not all) source files — convention, not enforced.
- `fprintf(stderr, ...) + fflush(stderr)` debug prints throughout `main.cpp` — startup tracing.
- Default connection: `127.0.0.1:9527`, clientId `admin_001`, heartbeat 30s, reconnect max 10, timeout 10s.
- Runtime: `Fusion` style, `d3d11` RHI backend, `Qt::ColorScheme::Light` (forced).

## QML singleton models

Files under `ui/models/*.qml` and `ui/theme/Theme.qml` have `QT_QML_SINGLETON_TYPE TRUE` set in CMakeLists.txt. Any new model QML file added there must also get this property.

## Adding a service

1. Create `src/YourModule/YourServer.h` + `.cpp`, inherit `QObject`, add `setTcpManager(TcpConnectionManager*)`
2. Add message types in `protocol.h`
3. Register sources in `CMakeLists.txt` under `qt_add_qml_module(AttendanceAdmin SOURCES ...)`
4. Instantiate + wire in `main.cpp`
5. Add to `engine.setInitialProperties({...})` + `Main.qml` required properties

## Adding a QML page

1. Create `ui/pages/PageYourFeature.qml`
2. Register in `CMakeLists.txt` under `QML_FILES`
3. Wire navigation: `Main.qml` sidebar + `PageHost` `pageKey` routing

## Gotchas

- Only `/.vs` is gitignored — `out/` build artifacts are tracked if added. Be careful not to commit them.
- `.claude/settings.local.json` has pre-approved commands (git add/commit, cmake build). Edit there to bypass repetitive approval.
- `SessionManager` has a `devLogin()` Q_INVOKABLE — used for development shortcuts.
- Code has mixed Chinese/English comments — don't treat this as a signal to write in either language.
- There are Chinese-language design docs under `docs/superpowers/` — these are specs/plans, not source of truth for architecture.
