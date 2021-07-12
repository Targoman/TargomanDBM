/******************************************************************************
 * TargomanDBM: Authentication, Authorization, Accounting framework           *
 *                                                                            *
 * Copyright 2014-2019 by Targoman Intelligent Processing <http://tip.co.ir>  *
 *                                                                            *
 * TargomanDBM is free software: you can redistribute it and/or modify        *
 * it under the terms of the GNU Lesser General Public License as published   *
 * by the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                        *
 *                                                                            *
 * TargomanDBM is distributed in the hope that it will be useful,             *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU Lesser General Public License for more details.                        *
 * You should have received a copy of the GNU Lesser General Public License   *
 * along with Targoman. If not, see <http://www.gnu.org/licenses/>.           *
 *                                                                            *
 ******************************************************************************/
/**
 * @author S. Mohammad M. Ziabary <ziabary@targoman.com>
 */

#include <QtTest>
#include "testDAC.hpp"
#include "libTargomanCommon/Logger.h"
#include "libTargomanDBM/clsDAC.h"

using namespace Targoman::DBManager;

int main(int argc, char *argv[])
{
    QCoreApplication App(argc, argv);
    Targoman::Common::Logger::instance().setVisible(true);

    ///TODO: when I comment these 3 lines, execQueryCacheable.toJson corrupts DACResult.WasSP
    qDebug() << "--------------------------------------------------";
    qDebug() << "-- test DAC --------------------------------------";
    qDebug() << "--------------------------------------------------";

    clsDAC::addDBEngine (enuDBEngines::MySQL);
//        clsDAC::setConnectionString ("HOST=192.168.0.240;PORT=3306;USER=root;PASSWORD=targoman1234;SCHEMA=mysql");
    clsDAC::setConnectionString("HOST=" TARGOMAN_M2STR(UNITTEST_DB_HOST) ";"
                                "PORT=" TARGOMAN_M2STR(UNITTEST_DB_PORT) ";"
                                "USER=" TARGOMAN_M2STR(UNITTEST_DB_USER) ";"
                                "PASSWORD=" TARGOMAN_M2STR(UNITTEST_DB_PASSWORD) ";"
                                "SCHEMA=" TARGOMAN_M2STR(UNITTEST_DB_SCHEMA) ";");

    bool BreakOnFirstFail = true;
    int FailedTests = 0;
    try {
        if (BreakOnFirstFail && !FailedTests) FailedTests += QTest::qExec(new testDAC, argc, argv);
//        if (BreakOnFirstFail && !FailedTests) FailedTests += QTest::qExec(new test???, argc, argv);
    } catch(std::exception &e) {
        qDebug()<<e.what();
    }
    if (FailedTests > 0) {
        qDebug() << "total number of failed tests: " << FailedTests;
    } else {
        qDebug() << "all tests passed :)";
    }

    clsDAC::shutdown();

    return FailedTests;
}


