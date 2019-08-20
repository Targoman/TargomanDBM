/******************************************************************************
 * TargomanDBM: An extension to QSqlDatabase                                  *
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
 * @author S. Mohammad M. Ziabary <ziabary@targoman.com>
 */

#ifndef TARGOMAN_DBMANAGER_PRIVATE_INTFDACDRIVER_HPP
#define TARGOMAN_DBMANAGER_PRIVATE_INTFDACDRIVER_HPP

#include <QString>
#include "libTargomanCommon/Macros.h"
#include "Definitions.h"
#include "intfDACDriver.hpp"

namespace Targoman {
namespace DBManager {
namespace Private {

TARGOMAN_DEFINE_ENUM(enuSPParamDir,
                     In = 1,
                     Out = 2,
                     InOut = 3
        )

struct stuSPParam
{
    enuSPParamDir::Type             Dir;
    QString                         Name;
    enuSQLAbstractDataType::Type    VarType;
    QVariant                        Value;

    stuSPParam() { ; }

    stuSPParam(enuSPParamDir::Type _dir,
               const QString& _name,
               enuSQLAbstractDataType::Type _varType,
               const QVariant&    _value = QVariant()):
        Dir(_dir),
        Name(_name),
        VarType(_varType),
        Value(_value)
    { ; }
};

typedef  QList<stuSPParam>  SPParams_t;

class intfDACDriver
{

public:
    virtual ~intfDACDriver(){ ; }
    virtual enuSQLAbstractDataType::Type   abstractDataType(const QString& _typeStr) = 0;

    virtual QStringList bindSPQuery(const QString& _spName,
                                    const SPParams_t &_spParams,
                                    const QVariantList& _spArgs,
                                    QStringList& _boundingVars) = 0;

    virtual QString boundSPQuery(const QString& _spName,
                                 const QStringList& _boundingVars) = 0;

    virtual SPParams_t getSPParams(QSqlQuery& _connectedQuery,
                                   const QString _schema,
                                   const QString& _spName) = 0;

    virtual QStringList whichOnesAreUpdated(const QSqlDatabase & _dbc,
                                            const QStringList& _tableNames,
                                            QDateTime& _pLastCheckTime) =0;

    virtual bool wasConnectionLost(const QSqlError& _error) = 0;
};

}
}
}
#endif // TARGOMAN_DBMANAGER_PRIVATE_INTFDACDRIVER_HPP
