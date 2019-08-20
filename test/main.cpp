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


#include <iostream>
#include <unistd.h>
#include "libTargomanDBM/clsDAC.h"
#include "libTargomanCommon/CmdIO.h"
#include "libTargomanCommon/Logger.h"

using namespace Targoman::DBManager;
int main(int argc, char *argv[])
{
    Targoman::Common::Logger::instance().setVisible(true);
    Q_UNUSED(argc); Q_UNUSED(argv);
    try{

        clsDAC::addDBEngine (enuDBEngines::MySQL);
        clsDAC::setConnectionString ("HOST=172.17.0.1;PORT=3306;USER=root;PASSWORD=1;SCHEMA=mysql");

        clsDAC DAC;
        qDebug("%s\n",DAC.execQuery("", "SELECT * FROM user")
                .toJson(false).toJson().constData());
        qDebug("***************************");
        qDebug("%s\n",DAC.execQuery("", "SELECT * FROM user WHERE user.User=?",{{"root"}})
               .toJson(false).toJson().constData());
        qDebug("***************************");
        qDebug("%s\n",DAC.execQuery("", "SELECT * FROM user WHERE user.User=:user",QVariantMap({{":user","root"}}))
               .toJson(false).toJson().constData());
        qDebug("***************************");
        qDebug("%s\n",DAC.callSP ("","Test.spNoOutput", {{"Param1","1"}}).toJson(false).toJson().constData());
        qDebug("***************************");
        qDebug("%s\n",DAC.callSP ("","Test.spSelectNoInput").toJson(false).toJson().constData());
        qDebug("***************************");
        qDebug("%s\n",DAC.callSP ("","Test.spJustDirectOut").toJson(false).toJson().constData());
        qDebug("***************************");
        qDebug("%s\n",DAC.callSP ("","Test.spFullInOut", {{"Param1","1"}, {"Param3", " test"}}).toJson(false).toJson().constData());
        qDebug("***************************");
        qDebug("%s\n",DAC.callSP ("","Test.spFullInOut", {{"Param1","1"}, {"Param2", " test"}}).toJson(false).toJson().constData());
    }catch(std::exception &e){
        TargomanError(e.what());
        return 1;
    }

    return 0;
}


