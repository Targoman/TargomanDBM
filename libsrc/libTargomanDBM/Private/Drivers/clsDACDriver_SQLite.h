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

#ifndef TARGOMAN_DBMANAGER_PRIVATE_DRIVERS_CLSDACDRIVER_SQLite_H
#define TARGOMAN_DBMANAGER_PRIVATE_DRIVERS_CLSDACDRIVER_SQLite_H

#include <QHash>
#include "Private/intfDACDriver.hpp"

namespace Targoman {
namespace DBManager {
namespace Private{
namespace Drivers {

class clsDACDriver_SQLite : public intfDACDriver
{
public:
    clsDACDriver_SQLite() { ; }

    QStringList bindSPQuery(const QString& _spName,
                            const SPParams_t &_spParams,
                            const QVariantMap& _spArgs,
                            QStringList* _boundingVars)
    {Q_UNUSED(_spName); Q_UNUSED(_spParams); Q_UNUSED(_spArgs) Q_UNUSED(_boundingVars); throw Common::exTargomanNotImplemented("SQLite Driver has not yet been developped");}

    QString boundSPQuery(const QString& _spName, const QStringList& _boundingVars)
    {Q_UNUSED(_spName); Q_UNUSED(_boundingVars); throw Common::exTargomanNotImplemented("SQLite Driver has not yet been developped");}


    enuSQLAbstractDataType::Type   abstractDataType(const QString& _typeStr)
    {Q_UNUSED(_typeStr); throw Common::exTargomanNotImplemented("SQLite Driver has not yet been developped");}

    SPParams_t getSPParams(QSqlQuery *_connectedQuery,
                           const QString _schema,
                           const QString& _spName)
    {Q_UNUSED(_connectedQuery); Q_UNUSED(_schema); Q_UNUSED(_spName); throw Common::exTargomanNotImplemented("SQLite Driver has not yet been developped");}


    QStringList whichOnesAreUpdated(const QSqlDatabase & _dbc,
                                    const QStringList& _tableNames,
                                    QDateTime& _pLastCheckTime)
    {Q_UNUSED(_dbc); Q_UNUSED(_tableNames); Q_UNUSED(_pLastCheckTime); throw Common::exTargomanNotImplemented("SQLite Driver has not yet been developped");}

    virtual bool wasConnectionLost(const QSqlError& _error)
    { Q_UNUSED(_error); throw Common::exTargomanNotImplemented("IBase Driver has not yet been developped"); }
};

}
}
}
}

#endif // TARGOMAN_DBMANAGER_PRIVATE_DRIVERS_CLSDACDRIVER_SQLite_H
