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

#include <QStringList>
#include <QCoreApplication>
#include <QtConcurrent/QtConcurrent>
#include <QThread>
#include <unistd.h>
#include <QSqlError>

#include "DACImpl.h"
#include "Private/clsDACPrivate.h"

#include "Drivers/clsDACDriver_MySQL.h"
#include "Drivers/clsDACDriver_DB2.h"
#include "Drivers/clsDACDriver_IBase.h"
#include "Drivers/clsDACDriver_ODBC.h"
#include "Drivers/clsDACDriver_Oracle.h"
#include "Drivers/clsDACDriver_PSQL.h"
#include "Drivers/clsDACDriver_SQLite.h"

#include "libTargomanCommon/Logger.h"

namespace Targoman {
namespace DBManager {
namespace Private{

/*####################################*/
#define clsDACPrivate_OpenDB(DBC) \
    if (!DBC.isValid()) \
    throw exTargomanDBMUnableToExecuteQuery("Invalid Database Connection"); \
    if (!DBC.isOpen()) { \
    DBC.close(); \
    usleep(100000); \
    DBC.open(); \
    usleep(100000); \
    if (!DBC.isOpen()) \
    throw exTargomanDBMUnableToExecuteQuery("Unable to connect to DB:" + DBC.lastError().text()); \
} \
    QMutexLocker ConPoolLocker(&this->mxConnectionList); \
    this->DBCAccessCache.insert(DBC.connectionName(), QDateTime::currentDateTime()); \
    ConPoolLocker.unlock();
/*####################################3*/

class clsNullDACSecurity: public intfDACSecurity
{
public:
    inline clsNullDACSecurity(){}
    inline virtual ~clsNullDACSecurity(){}
    inline bool isSPCallAllowed(const QString&/*_operatorID*/,
                                const QString& /*_sp*/,
                                const QVariantMap& /*_params*/)
    {return true;}

    inline bool isQueryAllowed(const QString& /*_operatorID*/, const QString& /*_query*/, const QVariantList& /*_params*/)
    {return true;}
    inline bool isQueryAllowed(const QString& /*_operatorID*/, const QString& /*_query*/, const QVariantMap& /*_params*/)
    {return true;}
};

DACImpl::DACImpl() :
    SecurityProvider(new clsNullDACSecurity)
{
    this->SecurityProvider = new clsNullDACSecurity;
    this->SQLDrivers.insert(QString(enuDBEngines::toStr(enuDBEngines::MySQL)).toUpper(), new Drivers::clsDACDriver_MySQL);
    this->SQLDrivers.insert(QString(enuDBEngines::toStr(enuDBEngines::DB2)).toUpper(), new Drivers::clsDACDriver_DB2);
    this->SQLDrivers.insert(QString(enuDBEngines::toStr(enuDBEngines::IBase)).toUpper(), new Drivers::clsDACDriver_IBase);
    this->SQLDrivers.insert(QString(enuDBEngines::toStr(enuDBEngines::ODBC)).toUpper(), new Drivers::clsDACDriver_ODBC);
    this->SQLDrivers.insert(QString(enuDBEngines::toStr(enuDBEngines::Oracle)).toUpper(), new Drivers::clsDACDriver_Oracle);
    this->SQLDrivers.insert(QString(enuDBEngines::toStr(enuDBEngines::PSQL)).toUpper(), new Drivers::clsDACDriver_PSQL);
    this->SQLDrivers.insert(QString(enuDBEngines::toStr(enuDBEngines::SQLite)).toUpper(), new Drivers::clsDACDriver_SQLite);

    this->ShuttingDown = false;

    //Every minute I'll check all DB connection and close those that has been not used for more than 3 minutes
    this->DBCChecker = QtConcurrent::run(this, &DACImpl::purgeConnections);
    connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this,  [this](){
        TargomanWarn(5,"Shutting down DAC connection manager");
        try{
            this->ShuttingDown = true;
            this->DBCChecker.waitForFinished();
        }catch(QUnhandledException &e) {
            Q_UNUSED(e)
        }
    });
}

QSqlDatabase DACImpl::getDBEngine(const QString &_domain, const QString &_entityName, const QString &_target, bool _clone, enuDBEngines::Type *_engineType, bool _returnBase)
{
    QSqlDatabase DB;

    auto retrieveConnectionByString = [&DB, _clone, _returnBase](const QString& _checkStr, bool _cloneCondition = false, const QString& _clonedName = ""){
        foreach(QString Connection, QSqlDatabase::connectionNames())
            if (Connection == _checkStr) {
                if (_clone){
                    if(_checkStr.split('_').count() == 4)
                        throw exTargomanDBM("Cloning target specified DB connections is not allowed");
                    else if(_cloneCondition)
                        DB = QSqlDatabase::cloneDatabase(QSqlDatabase::database(Connection, false),
                                                         Connection + "_" + _clonedName);
                    break;
                }

                if (_returnBase) {
                    DB =  QSqlDatabase::database(Connection, false);
                    break;
                }
                QString ThreadSafeConnection = _checkStr;
                if (QSqlDatabase::contains(
                            ThreadSafeConnection.replace(DEFAULT_DB_NAME,
                                                         DEFAULT_DB_NAME + QString("^%1^").arg((quint64)QThread::currentThreadId()))))
                    DB = QSqlDatabase::database(ThreadSafeConnection, false);
                else
                    DB = QSqlDatabase::cloneDatabase(QSqlDatabase::database(Connection, false), ThreadSafeConnection);

                break;
            }
    };

    retrieveConnectionByString(QString("%1_%2_%3_%4").arg(DEFAULT_DB_NAME).arg(_domain).arg(_entityName).arg(_target));

    if (DB.isValid() == false) retrieveConnectionByString(QString("%1_%2_%3").arg(DEFAULT_DB_NAME).arg(_domain).arg(_entityName), !_target.isEmpty(), _target);
    if (DB.isValid() == false) retrieveConnectionByString(QString("%1_%2").arg(DEFAULT_DB_NAME).arg(_domain), !_entityName.isEmpty() && !_target.isEmpty(), _entityName + "_" + _target);
    if (DB.isValid() == false) retrieveConnectionByString(QString("%1").arg(DEFAULT_DB_NAME), !_domain.isEmpty()&& !_entityName.isEmpty() && !_target.isEmpty(), _domain + "_" + _entityName + "_" + _target);
    if (DB.isValid()) {
        if (_engineType)
            *_engineType = enuDBEngines::toEnum(DB.driverName().toStdString().c_str());
        return DB;
    }
    throw exTargomanDBMEngineNotSupported(QString("No engine registerd for %1 : %2 : %3").arg(
                                              _domain).arg(
                                              _entityName).arg(
                                              _target));
}

qint64 DACImpl::runQueryBase(intfDACDriver* _driver,
                             QSqlQuery &_sqlQuery,
                             const QString& _purpose,
                             quint64* _executionTime)
{
    if (_purpose == "[NO_LOG]"){
        TargomanDebug(5, QString("[%2: Query: %1").arg(_sqlQuery.lastQuery(),_purpose).toUtf8().constData());
    }else{
        TargomanLogDebug(5, QString("[%2]: Query: %1").arg(_sqlQuery.lastQuery(),_purpose));
    }

    bool Result;

    if(_executionTime){
        QTime Timer;
        Timer.start();
        Result = _sqlQuery.exec();
        *_executionTime = Timer.elapsed();
    }else
        Result = _sqlQuery.exec();

    if(!Result){
        if(_driver->wasConnectionLost(_sqlQuery.lastError()))
            throw exTargomanDBMConnectionLost(
                    QString(
                        "Connection lost when executing query: [%1] %2").arg(
                        _sqlQuery.lastError().number()).arg(
                        _sqlQuery.lastError().text()));
        else
            throw exTargomanDBMUnableToExecuteQuery(
                    QString(
                        "Unable to execute query: [%1] %2").arg(
                        _sqlQuery.lastError().number()).arg(
                        _sqlQuery.lastError().text()));
    }

    if (_sqlQuery.isSelect())
        return _sqlQuery.size();
    else
        return _sqlQuery.numRowsAffected();
}

qint64 DACImpl::runQueryBase(intfDACDriver* _driver,
                             clsDACResult& _resultStorage,
                             const QString &_queryStr,
                             const QVariantList &_params,
                             const QString& _purpose,
                             quint64* _executionTime)
{
    if(!_resultStorage.d->Query.prepare (_queryStr))
        throw exTargomanDBMUnableToPrepareQuery(_queryStr);

    if (_params.size())
        foreach(auto Param, _params)
            _resultStorage.d->Query.addBindValue(Param);

    _resultStorage.d->AffectedRows = this->runQueryBase (_driver, _resultStorage.d->Query, _purpose, _executionTime);
    return _resultStorage.d->AffectedRows;
}

qint64 DACImpl::runQueryBase(intfDACDriver *_driver,
                             clsDACResult& _resultStorage,
                             const QString &_queryStr,
                             const QVariantMap &_params,
                             const QString &_purpose,
                             quint64 *_executionTime)
{
    if(!_resultStorage.d->Query.prepare (_queryStr))
        throw exTargomanDBMUnableToPrepareQuery(_queryStr);
    if (_params.size())
        for(auto ParamIter = _params.begin(); ParamIter != _params.end(); ++ParamIter)
            _resultStorage.d->Query.bindValue(ParamIter.key(), ParamIter.value());

    _resultStorage.d->AffectedRows = this->runQueryBase (_driver, _resultStorage.d->Query, _purpose, _executionTime);
    _resultStorage.d->AffectedRows;
}

clsDACResult DACImpl::runQuery(clsDAC &_dac,
                               const QString &_queryStr,
                               const QVariantList &_params,
                               const QString& _purpose,
                               quint64* _executionTime)
{
    QSqlDatabase DBC = this->getThreadDBC(_dac.pPrivate->DB);
    QMutexLocker QLocker(this->getCurrConnectionLock(DBC.connectionName()));

    quint8 Retries=0;
    QTime Timer;
    if(_executionTime)
        Timer.start();

    clsDACResult Result(DBC);

    while (true) {
        try{
            clsDACPrivate_OpenDB(DBC);

            this->runQueryBase(_dac.pPrivate->Driver, Result, _queryStr, _params, _purpose);
            if(_executionTime)
                *_executionTime = Timer.elapsed();
            return Result;
        }catch(exTargomanDBMConnectionLost){
            if (Retries++ > 2)
                throw;
            if (_purpose == "[NO_LOG]"){
                TargomanDebug(5, QString("%2: Retried QueryString: %1").arg(_queryStr,_purpose).toUtf8().constData())
            }else{
                TargomanLogDebug(5, QString("%2: Retried QueryString: %1").arg(_queryStr,_purpose))
            }
            DBC.close();
            usleep(50000);
        }
    }
}

clsDACResult DACImpl::runQuery(clsDAC &_dac,
                               const QString &_queryStr,
                               const QVariantMap &_params,
                               const QString& _purpose,
                               quint64* _executionTime)
{
    QTime Timer;
    if(_executionTime)
        Timer.start();

    QSqlDatabase DBC = this->getThreadDBC(_dac.pPrivate->DB);
    QMutexLocker QLocker(this->getCurrConnectionLock(DBC.connectionName()));
    quint8 Retries=0;
    clsDACResult Result(DBC);

    while (true) {
        try{
            clsDACPrivate_OpenDB(DBC);

            this->runQueryBase(_dac.pPrivate->Driver, Result, _queryStr, _params, _purpose);
            if(_executionTime)
                *_executionTime = Timer.elapsed();
            return Result;
        }catch(exTargomanDBMConnectionLost){
            if (Retries++ > 2)
                throw;
            if (_purpose == "[NO_LOG]"){
                TargomanDebug(5, QString("%2: Retried QueryString: %1").arg(_queryStr,_purpose).toUtf8().constData())
            }else{
                TargomanLogDebug(5, QString("%2: Retried QueryString: %1").arg(_queryStr,_purpose))
            }
            DBC.close();
            usleep(50000);
        }
    }
}

void DACImpl::callSP(clsDAC &_dac,
                     const QString& _spName,
                     const QVariantList &_spArgs,
                     const QString& _purpose,
                     quint64* _executionTime)
{
    QTime Timer;
    if(_executionTime)
        Timer.start();

    QSqlDatabase DBC = this->getThreadDBC(_dac.pPrivate->DB);
    QMutexLocker QLocker(this->getCurrConnectionLock(DBC.connectionName()));
    quint64 FirstExecutionTime = 0;
    quint8 Retries=0;
    intfDACDriver* Driver = _dac.pPrivate->Driver;
    clsDACResult Result(DBC);

    while (true) {
        try{
            clsDACPrivate_OpenDB(DBC);

            SPParams_t SPParams = Driver->getSPParams(Result.d->Query,
                                                      DBC.databaseName(),
                                                      _spName);

            QStringList BoundingVars;
            QStringList QueryStrings = Driver->bindSPQuery(_spName, SPParams, _spArgs, BoundingVars);

            foreach (const QString& QueryStr, QueryStrings) {
                this->runQueryBase(_dac.pPrivate->Driver, Result, _queryStr, _params, _purpose, _executionTime);
                this->runQuery(_sqlQuery, QueryStr, _purpose, _executionTime);
                if (_executionTime)
                    FirstExecutionTime += *_executionTime;
            }

            if (BoundingVars.size()) {
                QSqlQuery Query(_dac);

                QString QueryStr = Driver->boundSPQuery(_spName, BoundingVars);
                this->runQuery(&Query, QueryStr, _purpose, _executionTime);

                if (_executionTime)
                    *_executionTime += FirstExecutionTime;

                if (_spOutputs) {
                    _spOutputs->clear();
                    if(Query.first()) {
                        for(int i=0; i< BoundingVars.size(); i++) {
                            if (_purpose == "[NO_LOG]")
                                TargomanDebug(6,QString("\t%1 : <%2>").arg(QString(BoundingVars.at(i)).remove(
                                                                               0, DEFAULT_OUTVAR_PATTERN.size()), Query.value(i).toString()).toUtf8().constData())
                                        else
                                        TargomanLogDebug(6,QString("\t%1 : <%2>").arg(QString(BoundingVars.at(i)).remove(
                                                                                          0, DEFAULT_OUTVAR_PATTERN.size()), Query.value(i).toString()))

                                        _spOutputs->insert(
                                            QString(BoundingVars.at(i)).remove(
                                                0, DEFAULT_OUTVAR_PATTERN.size()), Query.value(i));
                        }
                    }
                }
            }
            break;
        }catch(exTargomanDBMConnectionLost){
            if (Retries++ > 2)
                throw;
            if (_purpose == "[NO_LOG]")
                TargomanDebug(5, QString("%2: Retried CallSP: %1").arg(_spName,_purpose).toUtf8().constData())
                        else
                        TargomanLogDebug(5, QString("%2: Retried CallSP: %1").arg(_spName,_purpose))
                        DBC.close();
            usleep(50000);
        }
    }
}

void DACImpl::setSecurityProvider(intfDACSecurity* _securityProvider)
{
    if(dynamic_cast<intfDACSecurity*>(_securityProvider) == NULL)
        throw exTargomanDBM("Invalid Security Provider pointer");

    if (dynamic_cast<clsNullDACSecurity*>(this->SecurityProvider) != NULL) {
        delete static_cast<clsNullDACSecurity*>(this->SecurityProvider);
        this->SecurityProvider = NULL;
    }

    //Just clsNullTCPServerSecurity Provider can be changed runtime
    if (this->SecurityProvider)
        throw exTargomanDBM("Runtime change of Network security provider is not allowed");

    this->SecurityProvider = _securityProvider;
}

intfDACSecurity* DACImpl::securityProvider()
{
    return this->SecurityProvider;
}

intfDACDriver *DACImpl::driver(const QString _driverName)
{
    intfDACDriver* Driver =  this->SQLDrivers.value(_driverName.toUpper());
    if(!Driver)
        Driver = this->SQLDrivers.value(_driverName.mid(1).toUpper());
    if(!Driver)
        throw exTargomanDBMEngineNotRegistered();
    return Driver;
}

SPParams_t DACImpl::getSPParams(intfDACDriver* _driver,
                                QSqlQuery *_connectedQuery,
                                const QString& _schema,
                                const QString& _spName)
{
    if (this->SPCache.contains(_spName) == false)
        this->SPCache.insert(_spName, _driver->getSPParams(_connectedQuery, _schema, _spName));

    return this->SPCache.value(_spName);
}

QStringList DACImpl::whichOnesAreUpdated(const QSqlDatabase &_dbc,
                                         const QStringList& _tableNames,
                                         QDateTime& _pLastCheckTime)
{
    intfDACDriver* Driver = this->driver(_dbc.driverName());
    return Driver->whichOnesAreUpdated(_dbc, _tableNames, _pLastCheckTime);
}

void DACImpl::purgeConnections()
{
    while(!this->ShuttingDown)
    {
        sleep(10); //Check every 10 Seconds
        QStringList ToBeRemoved;
        for(QHash<QString,QDateTime>::iterator CacheIter= this->DBCAccessCache.begin();
            CacheIter != this->DBCAccessCache.end();
            CacheIter++) {
            if (QDateTime::currentDateTime().secsTo(CacheIter.value()) < -60) //Close connections which was not active for more than 60 secs
            {
                ToBeRemoved.append(CacheIter.key());
                QMutexLocker QLocker(this->getCurrConnectionLock(CacheIter.key()));
                QSqlDatabase::database(CacheIter.key()).close();

                TargomanLogDebug(5, "Connection to DB " + CacheIter.key() + " closed by Timeout");
            }
        }

        QMutexLocker ConPoolLocker(&this->mxConnectionList);
        foreach (const QString& ConnName, ToBeRemoved) {
            this->DBCAccessCache.remove(ConnName);
            delete this->RunningQueryLocks.take(ConnName);
        }
    }
}

QSqlDatabase DACImpl::getThreadDBC(const QSqlDatabase &_dbc)
{
    QString ThreadSafeConnName = _dbc.connectionName().replace(
                DEFAULT_DB_NAME,
                QString("%1^%2^").arg(DEFAULT_DB_NAME).arg((quint64)QThread::currentThreadId()));

    if (_dbc.connectionName().contains(QString("^%1^").arg((quint64)QThread::currentThreadId())))
        return _dbc;
    else {
        QSqlDatabase DBC = QSqlDatabase::database(ThreadSafeConnName);
        if (DBC.isValid())
            return DBC;
        else
            return QSqlDatabase::cloneDatabase(_dbc,ThreadSafeConnName);
    }
}

QMutex *DACImpl::getCurrConnectionLock(const QString &_conName)
{
    QMutexLocker ConPoolLocker(&this->mxConnectionList);
    QMutex* mxCurrConnectionLock = this->RunningQueryLocks.value(_conName, NULL);
    if (mxCurrConnectionLock == NULL) {
        mxCurrConnectionLock = new QMutex;
        this->RunningQueryLocks.insert(_conName, mxCurrConnectionLock);
    }
    return mxCurrConnectionLock;
}

}
}
}
