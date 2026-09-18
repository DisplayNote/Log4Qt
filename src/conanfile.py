##---------------------------------------------------------------------------------------------
##
## Copyright (C) Displaynote Technologies Ltd. - All Rights Reserved
##
## Unauthorized copying of this file, via any medium is strictly prohibited
## Proprietary and confidential
##
##---------------------------------------------------------------------------------------------

import os

from conans import ConanFile
from conans import tools
from conans.errors import ConanException

## Android ABI that qmake appends to the library name (TARGET = log4qt_$${QT_ARCH}),
## keyed by the conan arch setting.
ANDROID_ABI = {
    'armv7': 'armeabi-v7a',
    'armv8': 'arm64-v8a',
    'x86': 'x86',
    'x86_64': 'x86_64',
}

class Log4QtConan(ConanFile):
    settings = 'os', 'compiler', 'build_type', 'arch'
    description = 'Log4Qt library'
    license = 'copyright'
    generators = 'qmake'

    def _source_folder(self):
        ## qt-conan-ci stages each build under <Platform>/<BuildType>, and macOS
        ## under Macos/<arch>/<BuildType> so that the x86_64 and arm64 stages do
        ## not overwrite each other in the shared build-folder artifact. That arch
        ## level is named with conan's own settings.arch value ("x86_64", "armv8"),
        ## so it drops in with no translation table.
        if self.settings.os == 'Macos':
            return os.path.join(str(self.settings.os),
                                str(self.settings.arch),
                                str(self.settings.build_type))

        return os.path.join(str(self.settings.os), str(self.settings.build_type))

    def package(self):
        src = self._source_folder()

        ## self.copy() from a directory that is not there copies nothing and still
        ## succeeds, so a renamed or missing stage folder would publish a silently
        ## empty package that only fails at link time on a consumer machine.
        if not os.path.isdir(os.path.join(self.build_folder, src)):
            raise ConanException("No build output at '{}'".format(src))

        if self.settings.os == 'Android':
            ## All three ABI stages install into the same Android/<BuildType>
            ## folder; they are kept apart by the ABI suffix qmake puts in the
            ## library name, so each package takes only its own .so and the lib
            ## folder stays flat (liblog4qt_arm64-v8a.so, ...).
            abi = ANDROID_ABI.get(str(self.settings.arch))
            if abi is None:
                raise ConanException("Android architecture not supported = {}".format(self.settings.arch))

            self.copy('*', src=os.path.join(src, 'include'), dst='include')
            self.copy('*{}*'.format(abi), src=os.path.join(src, 'lib'), dst='lib', keep_path=False)
        else:
            ## Windows, iOS and macOS keep the include/ + lib/ layout produced by
            ## the qmake install step.
            self.copy('*', src=src)

    def package_info(self):
        ## Add libraries stored in lib folder
        self.cpp_info.libdirs = ["lib"]
        self.cpp_info.libs = tools.collect_libs(self, 'lib')

        ## Extend the include path variable
        self.cpp_info.includedirs.extend(['include'])
