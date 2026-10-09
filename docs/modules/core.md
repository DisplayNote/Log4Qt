# Module: core (`src/log4qt/`)

## Purpose and boundaries

The logger tree and the event model: who decides whether a message is logged and which appenders
receive it. Formatting and output belong to [appenders-layouts.md](appenders-layouts.md);
configuration parsing belongs to [configuration.md](configuration.md).

## Public API

| Class / macro | File | Notes |
|---|---|---|
| `LogManager` | `logmanager.h/.cpp` | singleton; `rootLogger()`, `logger(name)`, `logLogger()` (`"Log4Qt"`), `qtLogger()` (`"Qt"`), `setHandleQtMessages()`, `setThreshold()`, `resetConfiguration()`, `shutdown()` (registered with `atexit`) |
| `Logger` | `logger.h/.cpp` | `trace/debug/info/warn/error/fatal` with `%1` arguments, `log()`, `logWithLocation()`, `additivity`, `level` |
| `LOG4QT_DECLARE_STATIC_LOGGER`, `LOG4QT_DECLARE_QCLASS_LOGGER` | `logger.h` | cached logger per class |
| `l4qTrace` … `l4qFatal` | `logger.h` | runtime level check, then log with `__FILE__`, `__LINE__`, `Q_FUNC_INFO` — nothing is compiled out |
| `LogStream` | `logstream.h` | `qDebug()`-style streaming |
| `Hierarchy` / `LoggerRepository` | `hierarchy.*`, `loggerrepository.*` | logger tree (`::` separates levels), threshold |
| `Level` | `level.*` | `NULL, ALL, TRACE, DEBUG, INFO, WARN, ERROR, FATAL, OFF`; `syslogEquivalent()` |
| `LoggingEvent` | `loggingevent.*` | message, level, logger, thread, timestamp, NDC, MDC properties, `MessageContext` (file/line/function), category |
| `MDC` / `NDC` | `mdc.*`, `ndc.*` | per-thread diagnostic context printed by `%X` / `%x` |
| `QmlLogger` | `qmllogger.*` | `Q_INVOKABLE` log methods for QML (context `"Qml"`) |
| `BinaryLogger`, `BinaryLogStream`, `BinaryLoggingEvent` | `binary*.h/.cpp` | binary payload logging; logger names carry the `@@binary@@` marker |

## Dependencies

- Upstream: QtCore only (`QThread`, `QMutex`, `QSettings`, `QDateTime`).
- Downstream: every appender (via `callAppenders`) and `PropertyConfigurator` (via `LogManager`).

## Testing in isolation

`tests/log4qttest/log4qttest.cpp` covers levels, loggers, hierarchy, MDC/NDC and the configurator,
capturing events with `ListAppender`; `tests/binaryloggertest/` covers the binary logger. See
[../runbooks/testing.md](../runbooks/testing.md).

## Typical changes

- New level-handling or startup behaviour: `logmanager.cpp` (`doConfigureLogLogger`, `welcome`,
  `doStartup`, `qtMessageHandler`).
- Keep `LogManager::welcome()` free of anything beyond version/timestamp at INFO: it runs before the
  app configures anything, and its DEBUG/TRACE branches already dump environment and `QSettings`
  values.
