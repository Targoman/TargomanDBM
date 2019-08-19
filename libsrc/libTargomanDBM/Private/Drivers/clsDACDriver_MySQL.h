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

#ifndef TARGOMAN_DBMANAGER_PRIVATE_DRIVERS_CLSDACDRIVER_MYSQL_H
#define TARGOMAN_DBMANAGER_PRIVATE_DRIVERS_CLSDACDRIVER_MYSQL_H

#include <QHash>
#include <QSqlError>
#include "Private/intfDACDriver.hpp"

namespace Targoman {
namespace DBManager {
namespace Private{
namespace Drivers {

class clsDACDriver_MySQL : public intfDACDriver
{
public:
    clsDACDriver_MySQL();

    QStringList bindSPQuery(const QString& _spName,
                        const SPParams_t &_spParams,
                        const QVariantMap& _spArgs,
                        QStringList* _boundingVars);

    QString boundSPQuery(const QString& _spName,
                         const QStringList& _boundingVars);

    enuSQLAbstractDataType::Type   abstractDataType(const QString& _typeStr);

    SPParams_t getSPParams(QSqlQuery *_connectedQuery,
                           const QString _schema,
                           const QString& _spName) final;

    QStringList  whichOnesAreUpdated(const QSqlDatabase & _dbc,
                                    const QStringList& _tableNames,
                                    QDateTime& _pLastCheckTime);
    inline bool wasConnectionLost(const QSqlError &_error){
        return _error.number() == 2006 || _error.number() == 2013;
    }

private:
    QHash<QString, enuSQLAbstractDataType::Type> DataTypes;
};

}
}
}
}

#endif // TARGOMAN_DBMANAGER_PRIVATE_DRIVERS_CLSDACDRIVER_MYSQL_H
