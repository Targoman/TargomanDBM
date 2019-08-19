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

#ifndef TARGOMAN_DBMANAGER_PRIVATE_INTFDACSECURITY_HPP
#define TARGOMAN_DBMANAGER_PRIVATE_INTFDACSECURITY_HPP

#include <QString>
#include <QVariantMap>
#include <QSqlQuery>

namespace Targoman {
namespace DBManager {
namespace Private {

class intfDACSecurity
{
public:
    virtual ~intfDACSecurity(){ ; }
    virtual bool isSPCallAllowed(const QString& _operatorID, const QString& _sp, const QVariantMap& _params) = 0;
    virtual bool isQueryAllowed(const QString& _operatorID, const QString& _query, const QVariantList& _params) = 0;
    virtual bool isQueryAllowed(const QString& _operatorID, const QString& _query, const QVariantMap& _params) = 0;
};

}
}
}
#endif // TARGOMAN_DBMANAGER_PRIVATE_INTFDACSECURITY_HPP
