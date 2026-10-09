# Troubleshooting

| Symptom | Root cause | Fix |
|---|---|---|
| `#error "Log4Qt requires Qt version 6.8.0 or higher"` | Building against Qt < 6.8 (`src/log4qt/log4qt.h`) | Use Qt 6.8.x |
| CMake: `Qt6Test` not found | Root `CMakeLists.txt` requires Core, Concurrent and Test | Install QtTest or point `CMAKE_PREFIX_PATH` at a full Qt |
| qmake tests fail to link `-llog4qt` | Library builds into `$$INSTALL_PREFIX/lib`, tests look in `../../bin` | Run tests with CMake ([testing.md](testing.md)) |
| App logs nothing | No configuration found by `LogManager::doStartup()` (or `DefaultInitOverride` set) — root logger has no appenders | Provide `log4qt.properties` / `[Log4Qt/Properties]` or configure in code ([../modules/configuration.md](../modules/configuration.md)) |
| Configuration errors invisible | Internal `Log4Qt` logger defaults to `ERROR` | Set `LOG4QT_DEBUG=DEBUG` (not on iOS) or `[Log4Qt] Debug=DEBUG` in `QSettings`, or `log4j.Debug=DEBUG`; output goes to stdout/stderr — sanitise before sharing |
| `Cannot convert to type 'QHostAddress' for property 'address'` | Factory only converts `bool`, `int`, `QString`, `Log4Qt::Level` | Set `TelnetAppender::setAddress()` in code |
| `Unable to create appender of class 'org.apache.log4j.DatabaseAppender'` | DisplayNote build has no `QT += sql` | Not available in the shipped package |
| Duplicate lines from library errors in the app's log | Internal logger is additive, so `Log4Qt::*` messages also reach root appenders | Expected; set `log4j.additivity.Log4Qt=false` in the app's properties if unwanted |
| Android: `mkdir: cannot create directory '/libs'` on install | Qt's Android mkspec install rule | Already disabled (`CONFIG -= android_install` in `src/log4qt/log4qt.pro`) |
| Windows/macOS/iOS package contains headers only | Old install list evaluated at qmake time | Fixed in 1.7.0 (`DESTDIR` into the install tree) |
| Compile error using `TTCCLayout::ABSOLUTE` / `RELATIVE` | Renamed to `ABSOLUTEDATE` / `RELATIVEDATE` in this fork | Use the new names |
