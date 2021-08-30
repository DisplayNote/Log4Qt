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

warnings.filterwarnings('ignore')

class Log4QtConan(ConanFile):
    settings = 'os', 'compiler', 'build_type', 'arch'
    description = 'Log4Qt library'
    license = 'copyright'
    generators = 'qmake'

    def package(self):
        self.copy("log4qt.dll", src='log4qt/install/lib/bin', dst='log4qt/lib', keep_path=False)
        self.copy("log4qt.lib", src='log4qt/install/lib/bin', dst='log4qt/lib', keep_path=False)
        self.copy('*', src='log4qt/install/include', dst='log4qt/include', keep_path=False)
    
    def package_info(self):
        ## Add libraries stored in lib folder
        self.cpp_info.libdirs = ["log4qt/lib"]
        self.cpp_info.libs = tools.collect_libs(self, 'log4qt/lib')

        ## Extend the include path variable
        self.cpp_info.includedirs.extend(['log4qt/include'])

