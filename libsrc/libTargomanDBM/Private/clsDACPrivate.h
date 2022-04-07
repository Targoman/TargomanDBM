/******************************************************************************
 * TargomanDBM: An extension to QSqlDatabase                                  *
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
 * @author Kambiz Zandi <kambizzandi@gmail.com>
 */

#ifndef TARGOMAN_DBMANAGER_PRIVATE_CLSDACPRIVATE_H
#define TARGOMAN_DBMANAGER_PRIVATE_CLSDACPRIVATE_H

#include <QScopedPointer>
#include <QSharedData>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMutex>
#include <QFuture>
#include "intfDACSecurity.hpp"
#include "intfDACDriver.hpp"
#include "Definitions.h"

namespace Targoman {

namespace AAA::Private {
class clsAuthorization;
}

namespace DBManager::Private {

class clsDACPrivate
{
public:
    explicit clsDACPrivate(const QSqlDatabase& _db);
    ~clsDACPrivate();

public:
    QSqlDatabase DB;
    intfDACDriver* Driver;
};

class clsDACResultPrivate : public QSharedData {
public:
    explicit clsDACResultPrivate(const QSqlDatabase& _dbc) :
        Query(_dbc),
        Database(_dbc),
        AffectedRows(0),
        WasSP(false),
        WasCached(false),
        IsValid(false)
    { ; }

//    clsDACResultPrivate(const clsDACResultPrivate &_other)
//        : QSharedData(_other),
//          Query(_other.Query),
//          Database(_other.Database),
//          SPDirectOutputs(_other.SPDirectOutputs),
//          AffectedRows(_other.AffectedRows),
//          WasSP(_other.WasSP),
//          WasCached(_other.WasCached),
//          IsValid(_other.IsValid)
//    { ; }

    ~clsDACResultPrivate();

public:
    QSqlQuery       Query;
    QSqlDatabase    Database;
    QVariantMap     SPDirectOutputs;
    qint32          AffectedRows;
    bool            WasSP;
    bool            WasCached;
    bool            IsValid;
};

}
} //namespace DBManager::Private

#endif // TARGOMAN_DBMANAGER_PRIVATE_CLSDACPRIVATE_H
