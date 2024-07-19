## Install rules ##

BUILD_TYPE=Debug
CONFIG(release, debug|release) {
    BUILD_TYPE=Release
}

# Installation prefix and library directory
isEmpty(INSTALL_PREFIX) {
    INSTALL_PREFIX = $$PWD/../install/
    android: INSTALL_PREFIX = $$INSTALL_PREFIX/Android/$$BUILD_TYPE
    ios:     INSTALL_PREFIX = $$INSTALL_PREFIX/iOS/$$BUILD_TYPE
    macx:    INSTALL_PREFIX = $$INSTALL_PREFIX/Macos/$$BUILD_TYPE
    win32:   INSTALL_PREFIX = $$INSTALL_PREFIX/Windows/$$BUILD_TYPE
}

# Path where install the static library
target.path = $$INSTALL_PREFIX/lib

# Path and files for install the headers
headers.path = $$INSTALL_PREFIX/include
headers.files = $$HEADERS_BASE

INSTALLS += target headers
