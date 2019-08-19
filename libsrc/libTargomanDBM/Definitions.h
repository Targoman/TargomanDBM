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

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariantHash>

#include "libTargomanCommon/Macros.h"
#include "libTargomanCommon/exTargomanBase.h"

#ifndef TARGOMAN_DBMANAGER_DEFINITIONS_H
#define TARGOMAN_DBMANAGER_DEFINITIONS_H

namespace Targoman {
namespace DBManager {

/**
 * \exception exTargomanDBM Base exception class for All DBmanager exceptions
 * \extends Targoman::Common::exTargomanBase
 */
TARGOMAN_ADD_EXCEPTION_HANDLER(exTargomanDBM, Targoman::Common::exTargomanBase);
/**
 * \exception exTargomanDBMEngineNotSupported Exception class used to report that required engine type is not available
 * \extends exTargomanDBM
 */
TARGOMAN_ADD_EXCEPTION_HANDLER(exTargomanDBMEngineNotSupported, exTargomanDBM);
/**
 * \exception exTargomanDBMEngineNotRegistered Exception class used to report that DB connection is not registered previously
 * \extends exTargomanDBM
 */
TARGOMAN_ADD_EXCEPTION_HANDLER(exTargomanDBMEngineNotRegistered, exTargomanDBM);
/**
 * \exception exTargomanDBMInvalidConnectionString Exception class used to report that the specified connection string is
 * invalid or ther is a problem connection to DB using this connection string
 * \extends exTargomanDBM
 */
TARGOMAN_ADD_EXCEPTION_HANDLER(exTargomanDBMInvalidConnectionString, exTargomanDBM);
/**
 * \exception exTargomanDBMUnableToExecuteQuery Exception class used to report that there is a problem executing specified
 * query or calling stored procedure
 * \extends exTargomanDBM
 */
TARGOMAN_ADD_EXCEPTION_HANDLER(exTargomanDBMUnableToExecuteQuery, exTargomanDBM);
/**
 * \exception exTargomanDBMUnableToPrepareQuery Exception class used to report that there is a problem preparing query for execution
 * \extends exTargomanDBM
 */
TARGOMAN_ADD_EXCEPTION_HANDLER(exTargomanDBMUnableToPrepareQuery, exTargomanDBM);
/**
 * \exception exTargomanDBMUnableToBindQuery Exception class used to report that there is a problem preparing query binding specified value
 * \extends exTargomanDBM
 */
TARGOMAN_ADD_EXCEPTION_HANDLER(exTargomanDBMUnableToBindQuery, exTargomanDBM);
/**
 * \exception exTargomanDBMUnableToExecuteQuery Exception class used to report the query has been failed and must be retried
 * \extends exTargomanDBM
 */
TARGOMAN_ADD_EXCEPTION_HANDLER(exTargomanDBMConnectionLost, exTargomanDBMUnableToExecuteQuery);
/**
 * \exception exTargomanDBMQueryResultNotReady Exception class used to report that Result is not ready yet. Usefull in
 * asynchronous result chekings.
 * \extends exTargomanDBM
 */
TARGOMAN_ADD_EXCEPTION_HANDLER(exTargomanDBMQueryResultNotReady, exTargomanDBM);
/**
 * \exception exTargomanDBMNotEnoughPrivileges Exception class used to report that security provider has restricted executing
 * specified query or calling specified Stored procedures with provided parameters
 * \extends exTargomanDBM
 */
TARGOMAN_ADD_EXCEPTION_HANDLER(exTargomanDBMNotEnoughPrivileges, exTargomanDBM);

/**
 * @brief DEFAULT_DB_NAME A const name to be added to all connection names in order to specialize all ETS connections
 */
const QString DEFAULT_DB_NAME="TGMNDAC";

/**
 *  @enum enuDBEngines Enumeration to be used in order to specify engine types.
 *  @value     MySQL  MySQL, MariaDB
 *  @value     ODBC   MSSQL, Access
 *  @value     Oracle Oracle
 *  @value     DB2    IBM DB2
 *  @value     PostgreSQL PostgreSQL
 *  @value     SQLite     SQLite
 *  @value     IBase      Borland DB
 */
TARGOMAN_DEFINE_ENHANCED_ENUM(enuDBEngines,
                              MySQL,
                              ODBC,
                              Oracle,
                              DB2,
                              PSQL,
                              SQLite,
                              IBase,
                              )

/**
 *  @enum enuSQLAbstractDataType Enumeration used to convert different DBMS data types to similar abstract data types
 *  in order to ease and generalize data manipulation. Type conversion will be done using DBMS drivers
 *  @value     Numeric  MySQL, MariaDB
 *  @value     Binary   MSSQL, Access
 *  @value     String Oracle
 *  @value     Unknown    IBM DB2
 */
TARGOMAN_DEFINE_ENUM(enuSQLAbstractDataType,
                     Numeric,
                     Binary,
                     String,
                     Unknown
                     )

class clsDAC;
namespace Private{
    class clsDACResultPrivate;
    class clsDACPrivate;
    class DACImpl;
}

/**
 *  @class clsDACResult
 *  @inmodule Targoman::DBManager
 *  @brief The clsDACResult class is used to wrap QSQLQuery and provide just readonly access to it. Also it provides
 *  an structure to report Direct SP outputs used for OUT and INOUT SP params. For more info about this class methods
 *  see Qt documentation on QSqlQuery.
 */
class clsDACResult
{
public:
    explicit clsDACResult(const QSqlDatabase& _dbc);
    ~clsDACResult();
    /*int at();
    bool first();
    bool isNull(int _field);
    bool isSelect();
    bool isValid();
    bool last();
    QSqlError lastError() const;
    QVariant lastInsertId() const;
    QString lastQuery() const;
    bool next();
    bool nextResult();
    int numRowsAffected() const;
    QSql::NumericalPrecisionPolicy numericalPrecisionPolicy() const;
    bool previous();
    QSqlRecord record() const;
    bool seek(int _index, bool _relative=false);
    int size() const;
    QVariant value(int _index) const;
    QVariant value(const QString& _colName) const;
    int colIndex(const QString& _colName);
    */
private:
    QSharedDataPointer<Private::clsDACResultPrivate> d;
    friend class Targoman::DBManager::Private::DACImpl;
};
}
}
#endif // TARGOMAN_DBMANAGER_DEFINITIONS_H
