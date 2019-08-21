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

#ifndef TARGOMAN_DBMANAGER_DAC_H
#define TARGOMAN_DBMANAGER_DAC_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariantMap>

#include "libTargomanDBM/Definitions.h"

namespace Targoman {
namespace DBManager {

/**
 * @class DAC
 * @inmodule Targoman::DBManager
 * @brief This is a wrapper class for QSqlDatabase. Which makes the DAC a single instance class enabling to
 * initialize essential parts of the framework. Also this class provides fast methods to create parameters
 * for different SQL DB engines. As you see many of QSqlDatabse methods are reimplemented as inline call
 * functions. This is because of QSqlDatabse class restrictions on inheritance which makes inherited classes
 * unusable.
 *
 * As there are many SQL wrappers included in Qt take the time to read the Qt's documentation on SQL
 *
 * For more information:
 *     http://www.mysql.com/news-and-events/newsletter/2004-01/a0000000297.html
 *
 */
class clsDAC
{
public:
    /**
     * @brief clsDAC constructor which will create a database for the specieid \a _domain, \a _entityName, \a _target
     * @param _domain Specify the domain where to look for registered DBs
     * @param _entityName Specify Entity in the specified Domain where to look for registered DBs
     * @param _target Specify specialized taget in the Entity to look for registered DBs
     */
    explicit clsDAC(const QString& _domain = "",
                    const QString& _entityName = "",
                    const QString& _target = "",
                    bool _clone = false);
    ~clsDAC();

    /**
     * @brief getDBEngine A method to retrive registered DB Item. It is supposed that you have registered some
     * databases previously using \a addDBEngine. This method will try to find a registered DB by matching all the
     * specified parameters \a _domain, \a _entityName, \a _target. But if there is no registered DB with all the
     * parameters it will try to find registered DB with less parameters starting to first discarding \a _target, next
     * \a _entityName and last \a _domain. If No registered DB is found an invalid DB will be returned. Sometimes there
     * is needed to clone connection in order to enable multithreaded queries this can be done using the \a _clone
     * parameter. If you need to know the DB engine type registered for this connection you can provide a engine type
     * storage to \a _engineType.
     *
     * @param _domain Specify the domain where to look for registered DBs
     * @param _entityName Specify Entity in the specified Domain where to look for registered DBs
     * @param _target Specify specialized taget in the Entity to look for registered DBs
     * @param _clone If set to true causes cloning on DB connection. Usefull for multithreaded applications. To clone
     * a DB connection you must specify it's target also. Clonning targeted DB connections is not allowed.
     * @param _engineType An storge to report registered DB Engine Type
     * @return A valid SQLDB if connection is found. Throw exception if required DB is not registered
     * @sa addDBEngine()
     */
    static QSqlDatabase getDBEngine(const QString& _domain = "",
                                    const QString& _entityName = "",
                                    const QString& _target = "",
                                    bool _clone = false,
                                    enuDBEngines::Type* _engineType = nullptr,
                                    bool _returnBase = false);

    /**
     * @brief addDBEngine A method that Registers a new DB connection for the specified \a _domain,
     * \a _entityName, \a _target
     * @param _engineType Type of the engine to be used in this connection
     * @param _domain Domain of the connection to be registered
     * @param _entityName Entity in the specified Domain to register connection.
     * @param _target Tareget in the specified entity to register connection
     * @sa getDBEngine()
     */
    static void addDBEngine(enuDBEngines::Type _engineType,
                            const QString& _domain = "",
                            const QString& _entityName = "",
                            const QString& _target = "");

    /**
     * @brief setConnectionString A method to initialize and open connection for the specified \a _domain,
     * \a _entityName, \a _target using a human readeable connection string.
     * @param _conStr Connection string used to initialize and open connection. It must have following sections joined
     * by smicolon:
     *     Host: (Hostname or IP of DBMS)
     *     User: (username to be used for connection)
     *     Password: (Password to be used for connection)
     *     Schema: (Default schema to be used in this connection)
     * ex. "HOST=127.0.0.1;User=user;Password=1234;schema=test"
     *
     * @param _domain Specify the domain where to look for registered DBs
     * @param _entityName Specify Entity in the specified Domain where to look for registered DBs
     * @param _target Specify specialized taget in the Entity to look for registered DBs
     * @return A valid and open DB connection if all the parameters are Ok. Throw errror if there is something wrong
     */
    static void setConnectionString(const QString& _conStr,
                                    const QString& _domain = "",
                                    const QString& _entityName = "",
                                    const QString& _target = "");
    /**
     * @brief runQuery A general static method to execute a Query String
     * @param _opertaorID ID of the operator who has requested execution of query. This can be ActorName, Session ID
     * (Started with #) or NULL (Started and ended with $)
     * @param _dbc Database connection to be used to execute this query
     * @param _queryStr Query String in SQL language to be executed
     * @param _params list of parameters to be replaced in query (if any)
     * @param _purpose An string to be shown in log and error messages before Query string or error messages
     * @param _executionTime An optional storage to be used to report query execution time.
     * @return if Query is select number of rows retrived else it will report Number of affected rows
     */
    clsDACResult execQuery(const QString& _opertaorID,
                           const QString &_queryStr,
                           const QVariantList &_params = QVariantList(),
                           const QString& _purpose = "",
                           quint64* _executionTime = nullptr);

    /**
     * @brief runQuery A general static method to execute a Query String
     * @param _opertaorID ID of the operator who has requested execution of query. This can be ActorName, Session ID
     * (Started with #) or NULL (Started and ended with $)
     * @param _dbc Database connection to be used to execute this query
     * @param _queryStr Query String in SQL language to be executed
     * @param _params list of parameters to be replaced in query (if any)
     * @param _purpose An string to be shown in log and error messages before Query string or error messages
     * @param _executionTime An optional storage to be used to report query execution time.
     * @return if Query is select number of rows retrived else it will report Number of affected rows
     */
    clsDACResult execQuery(const QString& _opertaorID,
                           const QString &_queryStr,
                           const QVariantMap &_params,
                           const QString& _purpose = "",
                           quint64* _executionTime = nullptr);

    /**
     * @brief callSP A complex method to be used to call stored procedures. This method will ease calling stored
     * procedures by ordering parameters, preparing outputs, binding and bounding all params, error checking, etc.
     * @param _opertaorID opertaorID ID of the operator who has requested execution of query. This can be ActorName, Session ID
     * (Started with #) or NULL (Started and ended with $)
     * @param _resultStorage An storage to be used to store results of the executed query. This can be null if the
     * calling stored procedure has no direct or indirect outputs
     * @param _spName Name of the stored procedure to be called
     * @param _spArgs Input arguments of the stored procedure in aribitary order. this method will order them and also
     * generate needed output variables of the SP. There is no need to provide ouput vriables but input value must be
     * provided for INOUT params
     * @param _purpose An string to be shown in log and error messages before Query string or error messages
     * @param _executionTime An optional storage to be used to report SP Call and execution time
     */
    clsDACResult callSP(const QString& _agentID,
                const QString& _spName,
                const QVariantMap& _spArgs = QVariantMap(),
                const QString& _purpose = "",
                quint64* _executionTime = nullptr);


    /**
     * @brief whichOnesAreUpdated A static method used to check wheter specified tables are updated or not. Depending
     * on the DB engine used by the specified connection there will be some restrictions and maybe not always accurate.
     * See dependent driver documentation.
     * @param _tableNames list of tabes to be checked if thy are updated or not.
     * @param _pLastCheckTime Specifies last checking time. This will be used in order to check if tables has been
     * updated after this time or not. Current time of DBMS will be reported in return.
     * @return List of tables updated. This list will have always less or equal items compared to \a _tableNames
     */
    QStringList whichTablesAreUpdated(const QStringList& _tableNames,
                                      QDateTime& _pLastCheckTime);

    /**
     * @brief areSimilar this static method checks wheter two DB connections are similar or not. As QSQLDatabase
     * comparing methods will check also for connection name this method will be used to ignore connection name and just
     * compare their connection string.
     * @param _firstDBC First DB connection to be checked
     * @param _secondDBC Second DB connection to be checked.
     * @return
     */
    static bool areSimilar(const QSqlDatabase& _firstDBC, const QSqlDatabase& _secondDBC);
    /**
     * @brief shutdown will shutdown garbage collector thread
     */
    static void shutdown();

private:
    Q_DISABLE_COPY(clsDAC)

private:
    QScopedPointer<Private::clsDACPrivate> pPrivate;
    friend class Private::DACImpl;
};


}
}


#endif // TARGOMAN_DBMANAGER_DAC_H
