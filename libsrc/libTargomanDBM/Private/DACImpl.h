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
 */

#ifndef TARGOMAN_DBMANAGER_PRIVATE_DACIMPL_H
#define TARGOMAN_DBMANAGER_PRIVATE_DACIMPL_H

#include <QScopedPointer>
#include <QSharedData>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMutex>
#include <QFuture>
#include <QReadWriteLock>

#include "intfDACSecurity.hpp"
#include "intfDACDriver.hpp"
#include "Definitions.h"
#include "clsDAC.h"
#include "libTargomanCommon/tmplExpirableCache.hpp"

namespace Targoman {

namespace AAA{
namespace Private{
class clsAuthorization;
}
}

namespace DBManager {
namespace Private{

class DACImpl : public QObject
{
    Q_OBJECT
    friend class Targoman::AAA::Private::clsAuthorization;

public:
    static  DACImpl& instance() {static DACImpl* Instance = NULL; return *(Q_LIKELY(Instance) ? Instance : Instance = new DACImpl); }
    ~DACImpl();
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
    QSqlDatabase getDBEngine(const QString& _domain = "",
                             const QString& _entityName = "",
                             const QString& _target = "",
                             bool _clone = false,
                             enuDBEngines::Type* _engineType = nullptr,
                             bool _returnBase = false);

    void addDBEngine(enuDBEngines::Type _engineType,
                     const QString& _domain,
                     const QString& _entityName,
                     const QString& _target
                     );

    qint64 runPreparedQuery(intfDACDriver *_driver, QSqlQuery& _sqlQuery, const QString &_purpose, quint64 *_executionTime = nullptr);

    clsDACResult runQuery(clsDAC& _dac,
                          const QString &_queryStr,
                          const QVariantList &_params = QVariantList(),
                          const QString& _purpose = "",
                          quint64* _executionTime = nullptr);

    clsDACResult runQuery(clsDAC& _dac,
                          const QString &_queryStr,
                          const QVariantMap &_params = QVariantMap(),
                          const QString& _purpose = "",
                          quint64* _executionTime = nullptr);

    clsDACResult runQueryCacheable(quint32 _ttl,
                                   clsDAC& _dac,
                                   const QString &_queryStr,
                                   const QVariantList &_params = QVariantList(),
                                   const QString& _purpose = "",
                                   quint64* _executionTime = nullptr);

    clsDACResult runQueryCacheable(quint32 _ttl,
                                   clsDAC& _dac,
                                   const QString &_queryStr,
                                   const QVariantMap &_params = QVariantMap(),
                                   const QString& _purpose = "",
                                   quint64* _executionTime = nullptr);

    clsDACResult callSP(clsDAC& _dac,
                        const QString& _spName,
                        const QVariantMap &_spArgs,
                        const QString& _purpose = "",
                        quint64* _executionTime = nullptr);

    clsDACResult callSPCacheable(quint32 _ttl,
                                 clsDAC& _dac,
                                 const QString& _spName,
                                 const QVariantMap &_spArgs,
                                 const QString& _purpose = "",
                                 quint64* _executionTime = nullptr);

    QStringList whichOnesAreUpdated(const QSqlDatabase& _dbc,
                                    const QStringList& _tableNames,
                                    QDateTime& _pLastCheckTime);

    intfDACSecurity* securityProvider();
    intfDACDriver* driver(const QString _driverName);

private:
    qint64 runQueryMiddleware(intfDACDriver* _driver,
                              clsDACResult &_resultStorage,
                              const QString& _queryStr,
                              const QVariantList& _params = QVariantList(),
                              const QString& _purpose  = "",
                              quint64* _executionTime  = nullptr);

    qint64 runQueryMiddleware(intfDACDriver* _driver,
                              clsDACResult &_resultStorage,
                              const QString& _queryStr,
                              const QVariantMap& _params = QVariantMap(),
                              const QString& _purpose  = "",
                              quint64* _executionTime  = nullptr);

    qint64 runDirectQuery(intfDACDriver *_driver, QSqlQuery& _sqlQuery, const QString& _queryString, const QString &_purpose, quint64 *_executionTime = nullptr);

    void setSecurityProvider(intfDACSecurity* _securityProvider);

    SPParams_t getSPParams(intfDACDriver* _driver,
                           QSqlQuery &_connectedQuery,
                           const QString& _schema,
                           const QString& _spName);

    void purgeConnections();
    QSqlDatabase getThreadDBC(const QSqlDatabase& _dbc);

    QMutex* getCurrConnectionLock(const QString& _conName);

    void throwFormatted(const QSqlError &_error);

public slots:
    void shutdown();

private:
    Q_DISABLE_COPY(DACImpl)
    DACImpl();

    intfDACSecurity*                SecurityProvider;
    QHash<QString, intfDACDriver*>  SQLDrivers;

    QHash<QString, SPParams_t>      SPCache;
    QReadWriteLock                  SPCacheLock;

    QHash<QString, QDateTime>       DBCAccessCache;
    QString                         ActorUUID;
    QHash<QString, QMutex*>         RunningQueryLocks;
    QMutex                          mxConnectionList;
    QFuture<void>                   DBCChecker;
    Common::tmplExpirableCache<QHash, QString, clsDACResult> Cache;
    bool                            ShuttingDown;
    QMutex                          mxRegistry;
    QHash<QString, QSqlDatabase>    Registry;
};

}
}
}
#endif // CLSDACIMPL_H
