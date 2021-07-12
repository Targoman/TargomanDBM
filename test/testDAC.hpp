/******************************************************************************
 * TargomanAAA: Authentication, Authorization, Accounting framework           *
 *                                                                            *
 * Copyright 2014-2019 by Targoman Intelligent Processing <http://tip.co.ir>  *
 *                                                                            *
 * TargomanAAA is free software: you can redistribute it and/or modify        *
 * it under the terms of the GNU Lesser General Public License as published   *
 * by the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                        *
 *                                                                            *
 * TargomanAAA is distributed in the hope that it will be useful,             *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU Lesser General Public License for more details.                        *
 * You should have received a copy of the GNU Lesser General Public License   *
 * along with Targoman. If not, see <http://www.gnu.org/licenses/>.           *
 *                                                                            *
 ******************************************************************************/
/**
 * @author S. Mehran M. Ziabary <ziabary@targoman.com>
 * @author Kambiz Zandi <kambizzandi@gmail.com>
 */

#ifndef TESTDAC_HPP
#define TESTDAC_HPP

//#include <iostream>
//#include <unistd.h>
#include <QtDebug>
//#include <QCoreApplication>
//#include <QJsonObject>
#include <QtConcurrent/QtConcurrent>
#include "libTargomanDBM/clsDAC.h"
//#include "libTargomanCommon/CmdIO.h"
#include "libTargomanCommon/Logger.h"

using namespace Targoman::DBManager;

class testDAC: public QObject
{
    Q_OBJECT

private slots:
    //    void test_clsDAC_execQuery_1()
    //    {
    //        clsDAC DAC;
    //        clsDACResult result = DAC.execQuery(
    //                                  "",
    //                                  "SELECT 1, prdID FROM Advert.tblAccountProducts");
    //        qDebug() << result.toJson(false);
    //    }
    //    void test_clsDAC_execQuery_2()
    //    {
    //        clsDAC DAC;
    //        clsDACResult result = DAC.execQuery(
    //                                  "",
    //                                  "SELECT 2, prdID FROM Advert.tblAccountProducts");
    //        qDebug() << result.toJson(false);
    //    }
    //    void test_clsDAC_execQuery_3()
    //    {
    //        clsDAC DAC;
    //        clsDACResult result = DAC.execQuery(
    //                                  "",
    //                                  "SELECT 3, prdID FROM Advert.tblAccountProducts");
    //        qDebug() << result.toJson(false);
    //    }
        void test_clsDAC_execQueryCacheable_1()
        {
            clsDAC DAC;
            clsDACResult result = DAC.execQueryCacheable(
                                      15*60,
                                      "",
                                      "SELECT 1, prdID FROM Advert.tblAccountProducts");
            qDebug() << result.toJson(false);
        }
    //    void test_clsDAC_execQueryCacheable_2()
    //    {
    //        clsDAC DAC;
    //        clsDACResult result = DAC.execQueryCacheable(
    //                                  15*60,
    //                                  "",
    //                                  "SELECT 2, prdID FROM Advert.tblAccountProducts");
    //        qDebug() << result.toJson(false);
    //    }
    //    void test_clsDAC_execQueryCacheable_3()
    //    {
    //        clsDAC DAC;
    //        clsDACResult result = DAC.execQueryCacheable(
    //                                  15*60,
    //                                  "",
    //                                  "SELECT 3, prdID FROM Advert.tblAccountProducts");
    //        qDebug() << result.toJson(false);
    //    }

private:
    void test_1()
    {
        clsDAC DAC;
        qDebug() << "***************************";
        qDebug() << DAC.execQuery("", "SELECT * FROM user")
                  .toJson(false).toJson().constData();

        qDebug() << "***************************";
        qDebug() << DAC.execQuery("", "SELECT * FROM user WHERE user.User=?",{{"root"}})
                  .toJson(false).toJson().constData();

        qDebug() << "***************************";
        qDebug() << DAC.execQuery("", "SELECT * FROM user WHERE user.User=:user",QVariantMap({{":user","root"}}))
                  .toJson(false).toJson().constData();

        /*        qDebug() << "***************************"<<"Test.spNoOutput";
        qDebug() << DAC.callSP ("","Test.spNoOutput", {{"Param1","1"}}).toJson(false).toJson().constData();

        qDebug() << "***************************"<<"Test.spSelectNoInput";
        qDebug() << DAC.callSP ("","Test.spSelectNoInput").toJson(false).toJson().constData();
        qDebug() << "***************************"<<"Test.spJustDirectOut";
        qDebug() << DAC.callSP ("","Test.spJustDirectOut").toJson(false).toJson().constData();
        qDebug() << "***************************"<<"Test.spFullInOut";
        qDebug() << DAC.callSP ("","Test.spFullInOut", {{"Param1","1"}, {"Param3", " test"}}).toJson(false).toJson().constData();
        qDebug() << "***************************"<<"Test.spFullInOut";
        qDebug() << DAC.callSP ("","Test.spFullInOut", {{"Param1","1"}, {"Param2", " test"}}).toJson(false).toJson().constData();
        qDebug() << "***************************"<<"Test.spFullInOut";
        qDebug() << DAC.callSP ("","AAA.sp_login", {
                                  {"iLogin", "09126174250"},
                                  {"iPass", "da1234a56dac001ba01e1cc61aed0ba7"},
                                  {"iSalt", "MySALT"},
                                  {"iInfo", QJsonObject()}
                              }).toJson(false).toJson().constData();
                              */

        auto future = QtConcurrent::run([=](){
           clsDAC DAC("ssss");
           qDebug() << DAC.execQuery("", "SELECT * FROM user")
                     .toJson(false).toJson().constData();

        });

        future.waitForFinished();
    }
};

#endif // TESTDAC_HPP
