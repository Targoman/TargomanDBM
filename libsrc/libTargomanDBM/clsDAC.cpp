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

#include <QThread>
#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "clsDAC.h"
#include "Private/clsDACPrivate.h"
#include "Private/DACImpl.h"

namespace Targoman {
namespace DBManager {

/* ----------------------------------------------- */
clsDAC::clsDAC(const QString& _domain,
               const QString& _entityName,
               const QString& _target,
               bool _clone) :
    pPrivate(new Private::clsDACPrivate(Private::DACImpl::instance().getDBEngine(_domain, _entityName, _target, _clone)))
{ ; }

clsDAC::~clsDAC()
{ ; }

/* ----------------------------------------------- */
QSqlDatabase clsDAC::getDBEngine(const QString& _domain,
                                 const QString& _entityName,
                                 const QString& _target,
                                 bool _clone,
                                 enuDBEngines::Type* _engineType,
                                 bool _returnBase)
{
    return Private::DACImpl::instance().getDBEngine(_domain, _entityName, _target, _clone, _engineType, _returnBase);
}

/* ----------------------------------------------- */
void clsDAC::addDBEngine(enuDBEngines::Type _engineType,
                         const QString& _domain,
                         const QString& _entityName,
                         const QString& _target)
{
    const QString DriverName = QString("Q%1").arg(enuDBEngines::toStr(_engineType)).toUpper();

    if (!QSqlDatabase::isDriverAvailable(DriverName))
        throw exTargomanDBMEngineNotSupported(QString("DB Engine: %1 Not supported. Try to install it.").arg(DriverName));

    QString DBName = DEFAULT_DB_NAME;
    if (!_domain.isEmpty()) {
        DBName += "_" + _domain;
        if (!_entityName.isEmpty()) {
            DBName += "_" + _entityName;
            if (!_target.isEmpty())
                DBName += "_" + _target;
        }
    }
    QSqlDatabase::addDatabase(DriverName, DBName);
}

/* ----------------------------------------------- */
void clsDAC::setConnectionString(const QString& _conStr,
                                 const QString& _domain,
                                 const QString& _entityName,
                                 const QString& _target)
{
    QSqlDatabase DB = Private::DACImpl::instance().getDBEngine(_domain, _entityName, _target,false,nullptr,true);

    bool Result;

    QStringList ConStrList = _conStr.split(';', QString::SkipEmptyParts);
    if (ConStrList.size() < 2)
        throw exTargomanDBMInvalidConnectionString("Count of parameters in connection string must be at least 2");

    DB.setHostName("");
    DB.setPort(0);
    DB.setUserName("");
    DB.setPassword("");
    DB.setDatabaseName("");

    foreach(QString Param, ConStrList) {
        if (Param.trimmed().isEmpty())
            break;
        QStringList ParamPair = Param.split('=');
        if (ParamPair.size() == 2) {
            if (ParamPair[0].trimmed().toUpper() == "HOST")
                DB.setHostName(ParamPair[1].trimmed());
            else if (ParamPair[0].trimmed().toUpper() == "PORT")
                DB.setPort(ParamPair[1].toInt(&Result));
            else if (ParamPair[0].trimmed().toUpper() == "USER")
                DB.setUserName(ParamPair[1].trimmed());
            else if (ParamPair[0].trimmed().toUpper() == "PASSWORD")
                DB.setPassword(ParamPair[1].trimmed());
            else if (ParamPair[0].trimmed().toUpper() == "SCHEMA")
                DB.setDatabaseName(ParamPair[1].trimmed());
            else
                throw exTargomanDBMInvalidConnectionString(QString("Invalid field <%1> in connection string").arg(
                                                               ParamPair[0].trimmed()));
        }
        else
            throw exTargomanDBMInvalidConnectionString("Connection String parameters must be divided by = and ;");
    }

    if (!DB.isOpen())
        DB.open();

    if (!DB.isOpen())
        throw exTargomanDBMInvalidConnectionString(QString("Unable to open connection for <%1/%2/%3> using <%4:%5@%6:%7>").arg(
                                                       _domain,_entityName, _target).arg(
                                                       DB.userName(), QString(DB.password().size(), '*'), DB.hostName()).arg(
                                                       DB.port()));
}

/* ----------------------------------------------- */
clsDACResult clsDAC::callSP(const QString& _agentID,
                            const QString& _spName,
                            const QVariantMap& _spArgs,
                            const QString& _purpose,
                            quint64* _executionTime)
{
    if (Private::DACImpl::instance().securityProvider()->isSPCallAllowed(_agentID, _spName, _spArgs))
        return  Private::DACImpl::instance().callSP(*this, _spName,_spArgs, _purpose, _executionTime);
    else
        throw exTargomanDBMNotEnoughPrivileges(
                QString("Not Enough privileges to call '%1' by %2").arg(_spName, _agentID));
}

clsDACResult clsDAC::callSPCacheable(quint32 _maxCacheTime, const QString &_agentID, const QString &_spName, const QVariantMap &_spArgs, const QString &_purpose, quint64 *_executionTime)
{
    if (Private::DACImpl::instance().securityProvider()->isSPCallAllowed(_agentID, _spName, _spArgs))
        return  Private::DACImpl::instance().callSPCacheable(_maxCacheTime, *this, _spName,_spArgs, _purpose, _executionTime);
    else
        throw exTargomanDBMNotEnoughPrivileges(
                QString("Not Enough privileges to call '%1' by %2").arg(_spName, _agentID));
}

/* ----------------------------------------------- */
clsDACResult clsDAC::execQuery(const QString &_agentID,
                               const QString &_queryStr,
                               const QVariantList& _params,
                               const QString &_purpose,
                               quint64* _executionTime)
{
    if (Private::DACImpl::instance().securityProvider()->isQueryAllowed(_agentID, _queryStr, _params))
        return  Private::DACImpl::instance().runQuery(*this, _queryStr, _params, _purpose, _executionTime);
    else
        throw exTargomanDBMNotEnoughPrivileges(
                QString("Not Enough privileges to execute '%1'[%2] by %3").arg(_queryStr, QJsonDocument::fromVariant(_params).toJson().constData(), _agentID));
}

/* ----------------------------------------------- */
clsDACResult clsDAC::execQuery(const QString &_agentID,
                               const QString &_queryStr,
                               const QVariantMap& _params,
                               const QString &_purpose,
                               quint64* _executionTime)
{
    if (Private::DACImpl::instance().securityProvider()->isQueryAllowed(_agentID, _queryStr, _params))
        return  Private::DACImpl::instance().runQuery(*this, _queryStr, _params, _purpose, _executionTime);
    else
        throw exTargomanDBMNotEnoughPrivileges(
                QString("Not Enough privileges to execute '%1'[%2] by %3").arg(_queryStr, QJsonDocument::fromVariant(_params).toJson().constData(), _agentID));
}

/* ----------------------------------------------- */
clsDACResult clsDAC::execQueryCacheable(quint32 _maxCacheTime, const QString &_agentID, const QString &_queryStr, const QVariantList &_params, const QString &_purpose, quint64 *_executionTime)
{
    if (Private::DACImpl::instance().securityProvider()->isQueryAllowed(_agentID, _queryStr, _params))
        return  Private::DACImpl::instance().runQueryCacheable(_maxCacheTime, *this, _queryStr, _params, _purpose, _executionTime);
    else
        throw exTargomanDBMNotEnoughPrivileges(
                QString("Not Enough privileges to execute '%1'[%2] by %3").arg(_queryStr, QJsonDocument::fromVariant(_params).toJson().constData(), _agentID));
}

/* ----------------------------------------------- */
clsDACResult clsDAC::execQueryCacheable(quint32 _maxCacheTime, const QString &_agentID, const QString &_queryStr, const QVariantMap &_params, const QString &_purpose, quint64 *_executionTime)
{
    if (Private::DACImpl::instance().securityProvider()->isQueryAllowed(_agentID, _queryStr, _params))
        return  Private::DACImpl::instance().runQueryCacheable(_maxCacheTime, *this, _queryStr, _params, _purpose, _executionTime);
    else
        throw exTargomanDBMNotEnoughPrivileges(
                QString("Not Enough privileges to execute '%1'[%2] by %3").arg(_queryStr, QJsonDocument::fromVariant(_params).toJson().constData(), _agentID));
}

/* ----------------------------------------------- */
QStringList clsDAC::whichTablesAreUpdated(const QStringList& _tableNames,
                                          QDateTime& _pLastCheckTime)
{
    return Private::DACImpl::instance().whichOnesAreUpdated(this->pPrivate->DB, _tableNames, _pLastCheckTime);
}

/* ----------------------------------------------- */
bool clsDAC::areSimilar(const QSqlDatabase& _firstDBC, const QSqlDatabase& _secondDBC)
{
    return (_firstDBC.databaseName() == _secondDBC.databaseName() &&
            _firstDBC.driverName() == _secondDBC.driverName() &&
            _firstDBC.hostName() == _secondDBC.hostName() &&
            _firstDBC.port() == _secondDBC.port());
}

void clsDAC::shutdown()
{
    Private::DACImpl::instance().shutdown();
}

/**********************************************************************************************************************/
Private::clsDACPrivate::clsDACPrivate(const QSqlDatabase& _db) :
    DB(_db),
    Driver(Private::DACImpl::instance().driver(_db.driverName()))
{ ; }
Private::clsDACPrivate::~clsDACPrivate()
{;}
/**********************************************************************************************************************/
Private::clsDACResultPrivate::~clsDACResultPrivate()
{ ; }
/**********************************************************************************************************************/
clsDACResult::clsDACResult() : d(new Private::clsDACResultPrivate(QSqlDatabase()))
{ ; }

clsDACResult::clsDACResult(const QSqlDatabase &_dbc) : d(new Private::clsDACResultPrivate(_dbc))
{ ; }

clsDACResult::clsDACResult(const clsDACResult &_other) : d(_other.d)
{ ; }

clsDACResult::~clsDACResult()
{;}

QJsonDocument clsDACResult::toJson(bool _justSingle, const QMap<QString, std::function<QVariant(const QVariant& _value)>> _converters)
{
    QJsonDocument  Json;
    QJsonArray     RecordsArray;
    while(this->d->Query.next()) {
        QJsonObject        recordObject;
        for(int i = 0; i < this->d->Query.record().count(); ++i){
            QVariant Value;
            if(this->d->Query.isNull(i) == false)
                Value = this->d->Query.value(i);
            QString ValueStr = Value.toString();

            if(ValueStr.size() && ValueStr.at(0) == '\0'){
                ValueStr = "false";
                Value = QVariant(false);
            }else if(ValueStr.size() && ValueStr.at(0) == '\1'){
                ValueStr = "true";
                Value = QVariant(true);
            }

            QJsonParseError Error;
            if(ValueStr.size() > 5 &&
               *ValueStr.toLatin1().begin() == '{' &&
               *(ValueStr.toLatin1().end()-1) == '}'){
                QJsonDocument Doc = QJsonDocument::fromJson(ValueStr.toUtf8(), &Error);
                if(Error.error == QJsonParseError::NoError){
                    //TODO IMPORTANT
                    /*if(_converters.contains(this->d->Query.record().fieldName(i))) {
                        auto a = Doc.object();
                        auto b = this->d->Query.record().fieldName(i);
                        auto c = _converters.value(b)(a);
                        auto d = QJsonValue::fromVariant(a);
                        recordObject.insert(this->d->Query.record().fieldName(i),
                                            QJsonValue::fromVariant(_converters.value(this->d->Query.record().fieldName(i))(Doc.object()))
                                            );
                    }else*/
                        recordObject.insert(this->d->Query.record().fieldName(i),
                                            Doc.object()
                                            );
                    continue;
                }
            }
            if (ValueStr.size() > 3 &&
                *ValueStr.toLatin1().begin() == '[' &&
                *(ValueStr.toLatin1().end()-1)== ']'){
                QJsonDocument Doc = QJsonDocument::fromJson(ValueStr.toUtf8(), &Error);
                if(Error.error == QJsonParseError::NoError){
                    if(_converters.contains(this->d->Query.record().fieldName(i)))
                        recordObject.insert(this->d->Query.record().fieldName(i),
                                            QJsonValue::fromVariant(_converters.value(this->d->Query.record().fieldName(i))(Doc.array()))
                                            );
                    else
                        recordObject.insert(this->d->Query.record().fieldName(i),
                                            Doc.array()
                                            );
                    continue;
                }

            }
            if(Value.isValid() && _converters.contains(this->d->Query.record().fieldName(i)))
                recordObject.insert( this->d->Query.record().fieldName(i),
                                     QJsonValue::fromVariant(_converters.value(this->d->Query.record().fieldName(i))(Value)) );
            else
                recordObject.insert( this->d->Query.record().fieldName(i),
                                     QJsonValue::fromVariant(Value) );
        }

        RecordsArray.push_back(recordObject);
    }

    if(this->d->WasSP){
        Json.setObject({
                           {DBM_SPRESULT_ROWS, RecordsArray},
                           {DBM_SPRESULT_DIRECT, QJsonDocument::fromVariant(this->d->SPDirectOutputs).object()}
                       });
    }else if(_justSingle){
        if(RecordsArray.count() > 0)
            Json.setObject(RecordsArray.at(0).toObject());
        else
            Json.setObject(QJsonObject());
    }else
        Json.setArray(RecordsArray);

    return Json;
}

int clsDACResult::at()
{
    return this->d->Query.at();
}

bool clsDACResult::first()
{
    return this->d->Query.first();
}

bool clsDACResult::isNull(int _field)
{
    return this->d->Query.isNull(_field);
}

bool clsDACResult::isSelect()
{
    return this->d->Query.isSelect();
}

bool clsDACResult::isValid()
{
    return this->d->Query.isValid();
}

bool clsDACResult::last()
{
    return this->d->Query.last();
}

QSqlError    clsDACResult::lastError () const
{
    return this->d->Query.lastError();
}

QVariant    clsDACResult::lastInsertId () const
{
    return this->d->Query.lastInsertId();
}

QString    clsDACResult::lastQuery () const
{
    return this->d->Query.lastQuery();
}

bool    clsDACResult::next ()
{
    return this->d->Query.next();
}

bool    clsDACResult::nextResult ()
{
    return this->d->Query.nextResult();
}

int    clsDACResult::numRowsAffected () const
{
    return this->d->Query.numRowsAffected();
}

QSql::NumericalPrecisionPolicy    clsDACResult::numericalPrecisionPolicy () const
{
    return this->d->Query.numericalPrecisionPolicy();
}

bool    clsDACResult::previous ()
{
    return this->d->Query.previous();
}

QSqlRecord    clsDACResult::record () const
{
    return this->d->Query.record();
}

bool    clsDACResult::seek ( int _index, bool _relative)
{
    return this->d->Query.seek(_index, _relative);
}

int    clsDACResult::size () const
{
    return this->d->Query.size();
}

QVariant    clsDACResult::value ( int _index ) const
{
    return this->d->Query.value(_index);
}

QVariant    clsDACResult::value ( const QString& _colName ) const
{
    int Index = this->d->Query.record().indexOf(_colName);
    Q_ASSERT_X(Index >= 0,
               "Retrieving Column Index",
               qPrintable("Invalid Column Name: " + _colName));

    return this->d->Query.value(Index);
}


int clsDACResult::colIndex(const QString& _colName)
{
    int Index = this->d->Query.record().indexOf(_colName);
    Q_ASSERT_X(Index >= 0,
               "Retrieving Column Index",
               qPrintable("Invalid Column Name: " + _colName));
    return Index;
}

QVariantMap clsDACResult::spDirectOutputs(const QMap<QString, std::function<QVariant(const QVariant&)>> _converters)
{
    if(this->d->WasSP == false)
        throw exTargomanDBM("Last query was no a call to Stored procedure");
    if(_converters.size())
        for (auto OutputIter  = this->d->SPDirectOutputs.begin();
             OutputIter != this->d->SPDirectOutputs.end();
             ++OutputIter
             )
            if(_converters.contains(OutputIter.key()))
                OutputIter.value() =_converters.value(OutputIter.key())(OutputIter.value());

    return this->d->SPDirectOutputs;
}

}
}
