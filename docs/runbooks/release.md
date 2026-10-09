# Release

## How a version ships

1. Work lands on `master` through a GitHub pull request on `DisplayNote/log4qt` (branches
   `feature/<AB id>_<desc>`; see `git log --merges`). PRs trigger `azure-pipelines.yml` (`pr:` on
   all branches, except changes only to `doc/`, `Readme.md`, `ChangeLog.md`), which builds every
   platform.
2. Add the changes to `ChangeLog.md` under the current section (`[v1.7.0]` today).
3. Tag the merge commit on `master`. DisplayNote tags are plain numbers (`2.0.0`, `2.1.0`); the
   upstream `v1.x.y` tags are history. Pushing any tag triggers the pipeline (`trigger: tags: include: '*'`).
4. The pipeline runs the `DisplayNote/qt-conan-ci` templates pinned to tag `2.4.0`:
   `common/setup.yml`, one build stage per platform (`macos-x86_64`, `macos-arm64`, `ios-arm64`,
   `android-multiabi`, `windows-x86_64`) on `Log4Qt/log4qt.pro`, then `common/deploy-develop.yml`,
   which packages `Log4Qt/src/conanfile.py` as conan package `log4qt` for the profiles
   `msvc19.x86_64`, `macos.x86_64`, `macos.arm64`, `ios.arm64`, `android.arm64-v8a`,
   `android.armeabi-v7a`, `android.x86_64`.
5. Consumers (Montage) bump their conan requirement to the new package.

The conan remote, user/channel and the version string given to the package are defined inside the
qt-conan-ci templates and the `montage-client-environment-variables` variable group, not in this
repo.

## Version numbers

- Library version (`LogManager::version()`): `build.pri` (`LOG4QT_VERSION_*`), `CMakeLists.txt`
  (`project(Log4Qt VERSION …)`) and `src/log4qt/log4qt.qbs` — keep the three in sync. Currently
  `1.7.0`.
- DisplayNote git tag: independent of the library version (`2.1.0` is the tag on the 1.7.0 code).

## Packaging details

`src/conanfile.py` copies `<OS>/<build_type>` (macOS: `Macos/<arch>/<build_type>`) from the CI
build folder; on Android it takes only the ABI-suffixed library (`liblog4qt_<abi>.so`). A missing
stage folder fails packaging on purpose (`ConanException`).
