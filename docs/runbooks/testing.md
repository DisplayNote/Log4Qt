# Testing

## What exists

Unit tests only, all Qt Test executables under `tests/`. No integration, UI or end-to-end tests, and
**DisplayNote CI does not build or run them**: `azure-pipelines.yml` builds `log4qt.pro`, whose
`SUBDIRS` is `src` only.

| Suite | Path | Covers | Build systems |
|---|---|---|---|
| `log4qttest` | `tests/log4qttest/` | levels, loggers, hierarchy, MDC/NDC, layouts, appenders, factory, property configurator | CMake, qmake, qbs |
| `binaryloggertest` | `tests/binaryloggertest/` | binary logger, layouts, appenders | CMake, qmake, qbs |
| `dailyfileappendertest` | `tests/dailyfileappendertest/` | `DailyFileAppender` date patterns and `keepDays` deletion (in a `QTemporaryDir`) | CMake, qmake, qbs |
| `tst_filewatchertest` | `tests/filewatcher/` | `QFileSystemWatcher` behaviour and `ConfiguratorHelper` file watch | qmake only |

## Run (CMake — recommended)

```bash
cmake -S . -B ../log4qt-cmake -DCMAKE_PREFIX_PATH=<Qt 6.8 prefix>
cmake --build ../log4qt-cmake -j8
ctest --test-dir ../log4qt-cmake --output-on-failure
# or a single suite with Qt Test options
../log4qt-cmake/bin/log4qttest -v2
```

The qmake test projects (`tests/tests.pro`) link against `../../bin`, but the library now builds
into `$$INSTALL_PREFIX/lib` (`src/log4qt/log4qt.pro`), so they do not find it without adjusting
`LIBS`. Prefer CMake.

## Files the tests write

`log4qttest` creates a `Log4QtTest_*` directory under `QDir::tempPath()` (`tests/log4qttest/log4qttest.cpp`)
and file appenders write there; `dailyfileappendertest` and `tst_filewatchertest` use a
`QTemporaryDir`. All content is synthetic test messages.

## Adding a test

Add a private slot (plus `_data` slot for data-driven cases) to `tests/log4qttest/log4qttest.{h,cpp}`
and capture events with the existing `ListAppender` member (`mpLoggingEvents`) or a file in the
temp directory, as the neighbouring tests do. For a new suite, copy `tests/dailyfileappendertest/`
(its `CMakeLists.txt`, `.pro`, `.qbs`) and add it to `tests/CMakeLists.txt`, `tests/tests.pro` and
`tests/tests.qbs`.
