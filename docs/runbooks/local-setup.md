# Local setup

From a clean machine to a built `liblog4qt`. Upstream build notes (qmake/CMake variants, static
build, macOS architectures) are in [`../../Readme.md`](../../Readme.md); this page is the short
DisplayNote path.

## 1. Clone

```bash
git clone git@github.com:DisplayNote/log4qt.git
cd log4qt
```

Run `/secret-scan-setup` once per clone (or `--global` once per machine for new clones) — commits containing secrets are blocked locally and in CI.
`/secret-scan-setup` is a Claude Code command from DisplayNote's `displaynote-engineering` plugin (install it with `/plugin install displaynote-engineering`); it is not part of this repository.

## 2. Prerequisites

- Qt **6.8** or newer with QtCore, QtNetwork, QtXml and QtConcurrent (DisplayNote CI uses Qt 6.8.8
  LTS). QtTest is required by the CMake build (tests are always added).
- A C++17 compiler: MSVC 2019+ on Windows, Xcode clang on macOS/iOS, the Android NDK that matches
  your Qt for Android.
- `qmake` from that Qt on `PATH`, or CMake ≥ 3.3 with `CMAKE_PREFIX_PATH` pointing at the Qt
  prefix.
- Conan is only needed to reproduce packaging (`src/conanfile.py`); CI does that.

## 3. Build the library (qmake — what CI ships)

```bash
mkdir -p ../log4qt-build && cd ../log4qt-build
qmake ../log4qt/log4qt.pro PREFIX=$PWD/install
make -j8
make install
```

The library lands in `install/lib` (`DESTDIR = $$INSTALL_PREFIX/lib$$LIB_SUFFIX`), headers in
`install/include/log4qt/{,helpers,spi,varia}`. Without `PREFIX`, the install tree is
`src/log4qt/install/` inside the checkout (gitignored). macOS architecture selection: see the macOS
section of `Readme.md` (`QMAKE_APPLE_DEVICE_ARCHS`). On Windows use `nmake`/`jom` instead of `make`.

## 4. Build library + tests + examples (CMake)

```bash
cmake -S . -B ../log4qt-cmake -DCMAKE_PREFIX_PATH=<Qt 6.8 prefix>
cmake --build ../log4qt-cmake -j8
```

Build out of source, as `Readme.md` requires (the root `CMakeLists.txt` includes
`cmake/MacroEnsureOutOfSourceBuild.cmake` but never calls the macro, so nothing enforces it). Options:
`-DBUILD_WITH_TELNET_LOGGING=ON` (matches the DisplayNote qmake build), `-DBUILD_WITH_DB_LOGGING=ON`,
`-DBUILD_STATIC_LOG4CXX_LIB=ON`, `-DBUILD_WITH_DOCS=ON` (Doxygen). Executables go to
`<build>/bin`.

## 5. Use it from an app without conan

Add `include(<checkout>/src/log4qt/log4qt.pri)` to the app's `.pro`, or link the installed
library and add `install/include` to the include path.
