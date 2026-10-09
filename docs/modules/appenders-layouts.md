# Module: appenders, filters and layouts (`src/log4qt/`, `varia/`, `spi/`)

## Purpose and boundaries

Everything after the logger decides to log: per-appender threshold and filters, formatting, and the
sink. These classes never pick a log location on their own — the consumer's configuration (or code)
sets file names, ports and service names.

## Appenders

| Class (properties name) | Sink | Built by DisplayNote CI? |
|---|---|---|
| `ConsoleAppender` (`target` = `STDOUT_TARGET` default / `STDERR_TARGET`) | stdout / stderr | yes |
| `ColorConsoleAppender` | console with ANSI colours | Windows only |
| `FileAppender` (`file`, `appendFile`, `bufferedIo`) | one file; creates missing parent dirs | yes |
| `RollingFileAppender` (`maxFileSize`, `maxBackupIndex`) | size-rolled files `<file>.1 … .N` | yes |
| `DailyRollingFileAppender` (`datePattern`) | time-rolled files | yes |
| `DailyFileAppender` (`datePattern`, `keepDays`) | `<base><date>.<ext>` per day; deletes files older than `keepDays` in the same dir | yes |
| `BinaryFileAppender`, `RollingBinaryFileAppender` | binary log files | yes |
| `SystemLogAppender` (`serviceName`, default app name) | `syslog()` facility `LOG_DAEMON` (Unix incl. macOS/iOS/Android builds) / Windows Event Log `ReportEventW` | yes |
| `WDCAppender` | `OutputDebugString` (no-op stub off Windows) | Windows only |
| `DebugAppender` (`varia/`) | `OutputDebugStringW` on Windows, `std::cerr` elsewhere | yes |
| `TelnetAppender` (`port` default 23, `address` default `QHostAddress::Any`, `immediateFlush`) | TCP listener; writes every formatted event to every connected client | yes (`QT += network`) |
| `DatabaseAppender` (`connection`, `table`) + `DatabaseLayout` | rows in an existing `QSqlDatabase` connection | **no** (needs `QT += sql`) |
| `SignalAppender` | Qt signal `appended(int level, QString message)` | yes |
| `AsyncAppender`, `MainThreadAppender` | forward to child appenders on a worker / the main thread | yes |
| `ListAppender`, `NullAppender` (`varia/`) | in-memory list / discard (tests, configurator errors) | yes |

There is no Android logcat or Apple `os_log` appender in this repo.

## Layouts

`PatternLayout` (conversion characters handled in `helpers/patternformatter.cpp`: `%c %d %m %p %r
%t %x %X %F %M %L %l`), `TTCCLayout`, `SimpleLayout`, `SimpleTimeLayout`, `XMLLayout`,
`BinaryLayout`, `BinaryToTextLayout`, `DatabaseLayout` (sql builds only).

## Filters

`spi/filter.h` base; `varia/` `DenyAllFilter`, `LevelMatchFilter`, `LevelRangeFilter`,
`StringMatchFilter`, `BinaryEventFilter`.

## Dependencies

- Upstream: core (`LoggingEvent`, `Level`), QtCore, QtConcurrent (`DailyFileAppender` cleanup),
  QtXml stream writer (`XMLLayout`), QtNetwork (`TelnetAppender`), QtSql (`DatabaseAppender`).
- Downstream: `helpers/factory.cpp` creates them by class name for the configurator.

## Testing in isolation

`tests/dailyfileappendertest/` (temp dir, date patterns, `keepDays` deletion) and the appender
sections of `tests/log4qttest/`. See [../runbooks/testing.md](../runbooks/testing.md).

## Extension points

New appender/layout/filter: see "Where to add X" in [`../../AGENTS.md`](../../AGENTS.md). Only
`bool`, `int`, `QString` and `Log4Qt::Level` `Q_PROPERTY`s can be set from a properties file
(`Factory::doSetObjectProperty`); others (e.g. `TelnetAppender::address`) are code-only.
