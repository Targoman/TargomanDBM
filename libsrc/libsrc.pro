################################################################################
#   QBuildSystem
#
#   Copyright(c) 2021 by Targoman Intelligent Processing <http://tip.co.ir>
#
#   Redistribution and use in source and binary forms are allowed under the
#   terms of BSD License 2.0.
################################################################################
DIST_HEADERS += \
    libTargomanDBM/Definitions.h \
    libTargomanDBM/clsDAC.h \

PRIVATE_HEADERS += \
    libTargomanDBM/Private/intfDACSecurity.hpp \
    libTargomanDBM/Private/intfDACDriver.hpp \
    libTargomanDBM/Private/Drivers/clsDACDriver_MySQL.h \
    libTargomanDBM/Private/clsDACPrivate.h \
    libTargomanDBM/Private/DACImpl.h \
    libTargomanDBM/Private/Drivers/clsDACDriver_DB2.h \
    libTargomanDBM/Private/Drivers/clsDACDriver_IBase.h \
    libTargomanDBM/Private/Drivers/clsDACDriver_ODBC.h \
    libTargomanDBM/Private/Drivers/clsDACDriver_Oracle.h \
    libTargomanDBM/Private/Drivers/clsDACDriver_PSQL.h \
    libTargomanDBM/Private/Drivers/clsDACDriver_SQLite.h

# +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-#
SOURCES += \
    libID.cpp \
    libTargomanDBM/Private/Drivers/clsDACDriver_MySQL.cpp \
    libTargomanDBM/Private/DACImpl.cpp \
    libTargomanDBM/clsDAC.cpp \

OTHER_FILES += \

################################################################################
include($$QBUILD_PATH/templates/libConfigs.pri)
