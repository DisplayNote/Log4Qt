QT += core xml network concurrent
include(../../build.pri)
include(../../g++.pri)
include(log4qt.pri)

CONFIG += c++17 \
          hide_symbols

contains(DEFINES, LOG4QT_STATIC) {
    message(Building static log4qt...)
    CONFIG += staticlib
}

TEMPLATE = lib

QT -= gui
android:versionAtLeast(QT_VERSION, 5.14) {
    TARGET = log4qt_$${QT_ARCH}
}
else {
    TARGET = log4qt
}

macx {
    QMAKE_APPLE_DEVICE_ARCHS = x86_64
}

ios {
    QMAKE_IOS_DEPLOYMENT_TARGET = 14.0
}

# .. is needed for msvc since it is treating '.' as the directory of the current file
# and not the directory where the compiled source is found
INCLUDEPATH += .. .

DEFINES += NOMINMAX QT_DEPRECATED_WARNINGS QT_NO_CAST_FROM_BYTEARRAY QT_USE_QSTRINGBUILDER
DEFINES += LOG4QT_LIBRARY

isEmpty(INSTALL_PREFIX) {
    INSTALL_PREFIX = $$PWD/install/
}

android:versionAtLeast(QT_VERSION, 5.14) {
    DESTDIR = $$INSTALL_PREFIX/lib$$LIB_SUFFIX
}
else {
    DESTDIR = ../../bin
    target.files = $$files($$DESTDIR/*)
    target.path = $$INSTALL_PREFIX/lib$$LIB_SUFFIX
    INSTALLS = target
}

header_base.files = $$HEADERS_BASE
header_base.path = $$INSTALL_PREFIX/include/log4qt
INSTALLS += header_base
header_helpers.files = $$HEADERS_HELPERS
header_helpers.path = $$INSTALL_PREFIX/include/log4qt/helpers
INSTALLS += header_helpers
header_spi.files = $$HEADERS_SPI
header_spi.path = $$INSTALL_PREFIX/include/log4qt/spi
INSTALLS += header_spi
header_varia.files = $$HEADERS_VARIA
header_varia.path = $$INSTALL_PREFIX/include/log4qt/varia
INSTALLS += header_varia

