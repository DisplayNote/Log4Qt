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
    isEmpty(QMAKE_APPLE_DEVICE_ARCHS): QMAKE_APPLE_DEVICE_ARCHS = $$QMAKE_HOST.arch
}

ios {
    QMAKE_IOS_DEPLOYMENT_TARGET = 14.0
}

# .. is needed for msvc since it is treating '.' as the directory of the current file
# and not the directory where the compiled source is found
INCLUDEPATH += .. .

DEFINES += NOMINMAX QT_DEPRECATED_WARNINGS QT_NO_CAST_FROM_BYTEARRAY QT_USE_QSTRINGBUILDER
DEFINES += LOG4QT_LIBRARY

# qt-conan-ci passes the staging directory as PREFIX (see common/build-unix.yml
# and common/build-win.yml), so honour it before falling back to an in-tree path.
isEmpty(INSTALL_PREFIX): INSTALL_PREFIX = $$PREFIX

isEmpty(INSTALL_PREFIX) {
    INSTALL_PREFIX = $$PWD/install/
}

# Qt's Android mkspec (mkspecs/features/android/android.prf) forces
# target.path = /libs/$$ANDROID_TARGET_ARCH and adds it to INSTALLS for every
# non-static lib. qt-conan-ci runs `make install` without INSTALL_ROOT, so that
# rule tries to create /libs at the filesystem root:
#     mkdir: cannot create directory '/libs': Permission denied
# DESTDIR already stages the .so where the conan recipe looks for it, so drop the
# implicit Android target install. Same fix as miracast-qt and googlecast-qt.
android: CONFIG -= android_install

# Build straight into the install tree on every platform, as the Android branch
# already did. The previous non-Android branch built into ../../bin and installed
# with target.files = $$files($$DESTDIR/*); $$files() is evaluated at qmake time,
# when that directory does not exist yet, so the list was empty and Windows,
# macOS and iOS installed no library at all.
DESTDIR = $$INSTALL_PREFIX/lib$$LIB_SUFFIX

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

