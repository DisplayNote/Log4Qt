TEMPLATE = subdirs

SUBDIRS +=  src

tests.depends = src
examples.depends = src

OTHER_FILES += LICENSE \
               Readme.md \
               .travis.yml \
               appveyor.yml \
               ChangeLog.md
