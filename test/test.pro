################################################################################
#   QBuildSystem
#
#   Copyright(c) 2021 by Targoman Intelligent Processing <http://tip.co.ir>
#
#   Redistribution and use in source and binary forms are allowed under the
#   terms of BSD License 2.0.
################################################################################
HEADERS += \
    testDAC.hpp

# +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-#
SOURCES += \
    main.cpp

QT+=concurrent
################################################################################
include($$QBUILD_PATH/templates/testConfigs.pri)

DEFINES += UNITTEST_DB_HOST="127.0.0.1"
DEFINES += UNITTEST_DB_PORT=3306
DEFINES += UNITTEST_DB_USER="root"
DEFINES += UNITTEST_DB_PASSWORD="targoman123"
DEFINES += UNITTEST_DB_SCHEMA="mysql"
