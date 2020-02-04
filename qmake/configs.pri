################################################################################
#   TargomanDBM: An extension to QSqlDatabase
#
#   Copyright 2014-2019 by Targoman Intelligent Processing <http://tip.co.ir>
#
#   TargomanDBM is free software: you can redistribute it and/or modify
#   it under the terms of the GNU Lesser General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   TargomanDBM is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public License
#   along with Targoman. If not, see <http://www.gnu.org/licenses/>.
################################################################################
include(version.pri)

#+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-
CONFIG(debug, debug|release): DEFINES += TARGOMAN_SHOW_DEBUG=1
CONFIG(release){
    QMAKE_CXXFLAGS_RELEASE -= -O2
    QMAKE_CXXFLAGS_RELEASE += -O3
}
DEFINES += TARGOMAN_DEBUG_PROCESS_LINE=1
DEFINES += TARGOMAN_SHOW_WARNING=1
DEFINES += TARGOMAN_SHOW_INFO=1
DEFINES += TARGOMAN_SHOW_HAPPY=1
DEFINES += TARGOMAN_SHOW_NORMAL=1

DEFINES += PROJ_VERSION=$$VERSION

QMAKE_CXXFLAGS+=-Wno-unknown-pragmas

#+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-
contains(QT_ARCH, x86_64){
    LibFolderPattern     = $$PREFIX/lib64
} else {
    LibFolderPattern     = $$PREFIX/lib
}
LibIncludeFolderPattern = ./include
BinFolderPattern        = ./bin
BuildFolderPattern      = ./build
TestBinFolder           = ./test
UnitTestBinFolder       = ./unitTest
ConfigFolderPattern     = ./conf

BaseLibraryFolder        = $$PRJDIR/out/$$LibFolderPattern
BaseLibraryIncludeFolder = $$PRJDIR/out/$$LibIncludeFolderPattern
BaseBinFolder            = $$PRJDIR/out/$$BinFolderPattern
BaseTestBinFolder        = $$PRJDIR/out/$$TestBinFolder
BaseUnitTestBinFolder    = $$PRJDIR/out/$$UnitTestBinFolder
BaseBuildFolder          = $$PRJDIR/out/$$BuildFolderPattern/$$ProjectName
BaseConfigFolder         = $$PRJDIR/out/$$ConfigFolderPattern

#+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-
INCLUDEPATH += $$PRJDIR \
               $$PRJDIR/src \
               $$PRJDIR/libsrc \
               $$BaseLibraryIncludeFolder \
               $$PREFIX/include \
               $(HOME)/local/include \
               $$DependencyIncludePaths/

#+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-
DependencyLibPaths      +=   $$BaseLibraryFolder \
                             $$PRJDIR/out/lib64 \
                             $$PRJDIR/out/lib \
                             $$PREFIX/lib64 \
                             $$PREFIX/lib \
                             $(HOME)/local/lib \
                             $(HOME)/local/lib64 \

#+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-
win32: DEFINES += _WINDOWS WIN32_LEAN_AND_MEAN NOMINMAX
FullDependencySearchPaths = $$DependencyLibPaths
unix:
  FullDependencySearchPaths+=  /usr/lib \
                               /usr/lib64 \
                               /usr/local/lib \
                               /usr/local/lib64 \
                               /lib/x86_64-linux-gnu


QMAKE_LIBDIR +=  $$FullDependencySearchPaths

#+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-
defineTest(addSubdirs) {
    for(subdirs, 1) {
        entries = $$files($$subdirs)
        for(entry, entries) {
            name = $$replace(entry, [/\\\\], _)
            SUBDIRS += $$name
            eval ($${name}.subdir = $$entry)
            for(dep, 2):eval ($${name}.depends += $$replace(dep, [/\\\\], _))
            export ($${name}.subdir)
            export ($${name}.depends)
        }
    }
    export (SUBDIRS)
}
#+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-
DEPS_BUILT = $$PRJDIR/out/.depsBuilt
Dependencies.target  = $$DEPS_BUILT
Dependencies.depends = FORCE
unix: Dependencies.commands = $$PRJDIR/buildDependencies.sh $$PRJDIR $$DEPS_BUILT $$EXTERNAL_DEPS;
win32: error(submodule auto-compile has not yet been implemented for windows)

PRE_TARGETDEPS += $$DEPS_BUILT
QMAKE_EXTRA_TARGETS += Dependencies
QMAKE_DISTCLEAN += $$DEPS_BUILT

HEADERS+= $$VERSIONINGHEADER


