# CLAUDE.md — Log4Qt (DisplayNote fork)

@AGENTS.md

`AGENTS.md` is the source of truth (including the mandatory `## Security` section). This file only
adds the short imperative rules for Claude Code.

## DO

- Build the library the way CI does: qmake on `log4qt.pro` with Qt 6.8 (see
  `docs/runbooks/local-setup.md`). Use CMake when you need the tests.
- When adding a source file, update `src/log4qt/log4qt.pri`, `src/log4qt/CMakeLists.txt` and
  `src/log4qt/log4qt.qbs`, and add the forwarding header under `include/log4qt/`.
- Register new appenders/layouts/filters in `src/log4qt/helpers/factory.cpp`, inside the same
  `QT_SQL_LIB` / `QT_NETWORK_LIB` / `Q_OS_WIN` guards the build uses.
- Report library errors as `LogError` through `logger()->error(e)`, like `fileappender.cpp`.
- Add a `ChangeLog.md` entry under the current DisplayNote version for user-visible changes.
- Sanitise any log before reading it: `/log-sanitise <file>` (see
  `docs/runbooks/debugging-with-ai.md`).

## DON'T

- Don't edit `doc/`, `.travis.yml`, `appveyor.yml` or the upstream sections of `Readme.md` unless
  asked — they are upstream material.
- Don't add `QT += sql` or change `QT += network` in `src/log4qt/log4qt.pro` without flagging it:
  it adds/removes `DatabaseAppender` / `TelnetAppender` in every shipped package.
- Don't lower the Qt floor below 6.8 or reintroduce `qAsConst`, `Qt::TimeSpec`, `QVariant::Type`.
- Don't make library code log configuration values, credentials or full paths above DEBUG.
- Don't read raw `*.log` files or paste log content into a prompt.
