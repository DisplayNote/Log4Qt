# Glossary

| Term | Meaning in this repo |
|---|---|
| Logger | Named node in the logger tree (`Logger`); names use `::` (Java-style `.` names are converted by `OptionConverter::classNameJavaToCpp`) |
| Root logger | Top of the tree (`LogManager::rootLogger()`); default level DEBUG after reset |
| Hierarchy / LoggerRepository | The container of all loggers and the global threshold (`hierarchy.*`) |
| Level | Severity: `NULL, ALL, TRACE, DEBUG, INFO, WARN, ERROR, FATAL, OFF` (`level.h`) |
| Effective level | A logger's own level, or the nearest ancestor's if it has none |
| Threshold | Minimum level on the repository (`log4j.threshold`) or on one appender (`threshold` property) |
| Additivity | Whether a logger also passes events to its parent's appenders (default `true`) |
| Appender | A sink: file, console, syslog, telnet, signal… (`AppenderSkeleton` subclasses) |
| Layout | Formatter that turns a `LoggingEvent` into text/binary/SQL record |
| Filter | Chain on an appender that accepts/denies/passes events (`spi/filter.h`, `varia/`) |
| LoggingEvent | One log record: message, level, logger, thread, time, NDC, MDC, source location, category |
| MDC / NDC | Mapped / nested diagnostic context — per-thread key/values or stack printed with `%X` / `%x` |
| LogLog / `logLogger()` | The library's own logger, named `Log4Qt`, writing to stdout/stderr; level from the `Debug` setting |
| `qtLogger()` | Logger named `Qt` that receives `qDebug`/`qWarning`/… when `setHandleQtMessages(true)` |
| `LogError` | Error object the library logs instead of throwing (`helpers/logerror.*`) |
| PropertyConfigurator | Configures loggers/appenders from log4j-style properties (file, `QSettings`, `Properties`) |
| Factory | Class-name registry that creates appenders/layouts/filters and sets their properties |
| TTCC | "Time, Thread, Category, Context" layout (`TTCCLayout`), also used for the internal logger |
| Binary logger | Loggers whose name carries `@@binary@@`; log `QByteArray` payloads (`binary*.h`) |
| WDC | Windows Debug Console — `WDCAppender` writing via `OutputDebugString` |
| qt-conan-ci | DisplayNote repo of Azure Pipelines templates used to build and publish this package |
| AB#<id> | Azure DevOps work item reference used in branch names and ChangeLog entries |
