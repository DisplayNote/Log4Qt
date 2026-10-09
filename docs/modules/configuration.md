# Module: configuration (`propertyconfigurator.*`, `basicconfigurator.*`, `helpers/`)

## Purpose and boundaries

Turns settings into a running logger tree: where configuration is looked up, how property strings
become appenders/layouts/filters, and how the internal logger is set up. It does not format or
write log events.

## Where configuration comes from

Settings read by `InitialisationHelper::setting()` (`helpers/initialisationhelper.cpp`): first the
environment variables `LOG4QT_DEBUG`, `LOG4QT_DEFAULTINITOVERRIDE`, `LOG4QT_CONFIGURATION` (not read
on iOS), then `QSettings` group `[Log4Qt]` keys `Debug`, `DefaultInitOverride`, `Configuration`
(only once a `QCoreApplication` exists).

`LogManager::doStartup()` (`logmanager.cpp`) then configures from the first match:

1. `DefaultInitOverride` set to anything but `false` → no automatic configuration (the app
   configures in code).
2. `Configuration` names an existing file → `PropertyConfigurator::configure(file)`.
3. `QSettings` group `[Log4Qt/Properties]` exists → configure from those keys.
4. `<application file path>.log4qt.properties`, then the same name without `.exe.`.
5. `log4qt.properties` in the executable's directory.
6. `log4qt.properties` relative to the current working directory.

Nothing found → the package stays unconfigured (no appenders on the root logger). The app can also
call `PropertyConfigurator::configure()` / `configureAndWatch()` or `BasicConfigurator::configure()`
(root → stdout with a TTCC `PatternLayout`) itself.

## Properties format

Java-log4j compatible keys (`log4j.rootLogger`, `log4j.appender.<name>=<class>`,
`log4j.appender.<name>.<property>`, `log4j.logger.<name>`, `log4j.additivity.<name>`) plus global
keys `log4j.reset`, `log4j.Debug`, `log4j.threshold`, `log4j.handleQtMessages`,
`log4j.watchThisFile`, `log4j.qtLogging.filterRules`, `log4j.qtLogging.messagePattern`
(`PropertyConfigurator::configureGlobalSettings`). Example:
`examples/propertyconfigurator/propertyconfigurator.exe.log4qt.properties`.

## Public API

| Class | File | Role |
|---|---|---|
| `PropertyConfigurator` | `propertyconfigurator.*` | `configure(file / QSettings / Properties)`, `configureAndWatch()` |
| `BasicConfigurator` | `basicconfigurator.*` | default console setup |
| `Factory` | `helpers/factory.*` | class-name registry for appenders/filters/layouts; sets `Q_PROPERTY`s from strings (`bool`, `int`, `QString`, `Log4Qt::Level` only) |
| `ConfiguratorHelper` | `helpers/configuratorhelper.*` | watched configuration file (`QFileSystemWatcher`) and last configure errors |
| `InitialisationHelper` | `helpers/initialisationhelper.*` | environment/`QSettings` settings, start time, metatype registration |
| `Properties`, `OptionConverter` | `helpers/properties.*`, `helpers/optionconverter.*` | parsing and string → level/bool/int/target conversion |

## Internal logger (LogLog)

`LogManager::doConfigureLogLogger()` attaches two `ConsoleAppender`s with a `TTCCLayout` to the
`Log4Qt` logger: stdout for ≤ INFO, stderr for ≥ WARN. Its level comes from the `Debug` setting
(default `ERROR`) and can be changed by `log4j.Debug`. At DEBUG/TRACE it logs configuration file
paths, every property set by the factory and every property loaded; see
[../runbooks/debugging-with-ai.md](../runbooks/debugging-with-ai.md).

## Testing in isolation

`tests/log4qttest/` (configurator and factory sections) and `tests/filewatcher/` (qmake-only test of
`QFileSystemWatcher` behaviour and `ConfiguratorHelper::setConfigurationFile`). See [../runbooks/testing.md](../runbooks/testing.md).
