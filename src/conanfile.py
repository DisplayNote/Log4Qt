##---------------------------------------------------------------------------------------------
##
## Copyright (C) Displaynote Technologies Ltd. - All Rights Reserved
##
## Unauthorized copying of this file, via any medium is strictly prohibited
## Proprietary and confidential
##
##---------------------------------------------------------------------------------------------

import os
import warnings

from conans import ConanFile
from conans import tools

class Log4QtConan(ConanFile):
    settings = 'os', 'compiler', 'build_type', 'arch'
    description = 'Log4Qt library'
    license = 'copyright'
    generators = 'qmake'

    def package(self):
        self.copy('*', src='log4qt/install/include', dst='include')
        if self.settings.os == 'Windows':
            self.copy("log4qt.dll", src='log4qt/install/lib', dst='lib', keep_path=False)
            self.copy("log4qt.lib", src='log4qt/install/lib', dst='lib', keep_path=False)
        elif self.settings.os == 'Macos':
            self.copy("*.dylib", src='log4qt/install/lib', dst='lib', keep_path=False)
        elif self.settings.os == 'iOS':
            self.copy("*.a", src='log4qt/install/lib', dst='lib', keep_path=False)
        elif self.settings.os == 'Android':
            if self.settings.arch == 'multiarch':
                self.copy("*.so", src='log4qt/install/lib', dst='lib', keep_path=False)
            else:
                self.copy("*.so", src='log4qt', dst='lib', keep_path=False)

    def package_info(self):
        ## Add libraries stored in lib folder
        self.cpp_info.libdirs = ["lib"]
        self.cpp_info.libs = tools.collect_libs(self, 'lib')

        ## Extend the include path variable
        self.cpp_info.includedirs.extend(['include'])