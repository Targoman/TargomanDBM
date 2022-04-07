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

#include <QSqlQuery>
#include <QStringList>
#include <QDateTime>
#include <QVariant>
#include <QJsonObject>
#include <QJsonArray>
#include "clsDACDriver_MySQL.h"

#include "../DACImpl.h"

namespace Targoman {
namespace DBManager {
namespace Private{
namespace Drivers {

clsDACDriver_MySQL::clsDACDriver_MySQL()
{
    this->DataTypes.insert("BIT",enuSQLAbstractDataType::Numeric);
    this->DataTypes.insert("TINYINT",enuSQLAbstractDataType::Numeric);
    this->DataTypes.insert("BOOL",enuSQLAbstractDataType::Numeric);
    this->DataTypes.insert("BOOLEAN",enuSQLAbstractDataType::Numeric);
    this->DataTypes.insert("SMALLINT",enuSQLAbstractDataType::Numeric);
    this->DataTypes.insert("MEDIUMINT",enuSQLAbstractDataType::Numeric);
    this->DataTypes.insert("INT",enuSQLAbstractDataType::Numeric);
    this->DataTypes.insert("INTEGER",enuSQLAbstractDataType::Numeric);
    this->DataTypes.insert("BIGINT",enuSQLAbstractDataType::Numeric);
    this->DataTypes.insert("DECIMAL",enuSQLAbstractDataType::Numeric);
    this->DataTypes.insert("DEC",enuSQLAbstractDataType::Numeric);
    this->DataTypes.insert("NUMERIC",enuSQLAbstractDataType::Numeric);
    this->DataTypes.insert("FIXED",enuSQLAbstractDataType::Numeric);
    this->DataTypes.insert("FLOAT",enuSQLAbstractDataType::Numeric);
    this->DataTypes.insert("DOUBLE",enuSQLAbstractDataType::Numeric);

    this->DataTypes.insert("DATE",enuSQLAbstractDataType::String);
    this->DataTypes.insert("TIME",enuSQLAbstractDataType::String);
    this->DataTypes.insert("DATETIME",enuSQLAbstractDataType::String);
    this->DataTypes.insert("TIMESTAMP",enuSQLAbstractDataType::String);
    this->DataTypes.insert("YEAR",enuSQLAbstractDataType::String);

    this->DataTypes.insert("CHAR",enuSQLAbstractDataType::String);
    this->DataTypes.insert("VARCHAR",enuSQLAbstractDataType::String);
    this->DataTypes.insert("TINYBLOB",enuSQLAbstractDataType::String);
    this->DataTypes.insert("TINYTEXT",enuSQLAbstractDataType::String);
    this->DataTypes.insert("BLOB",enuSQLAbstractDataType::String);
    this->DataTypes.insert("TEXT",enuSQLAbstractDataType::String);
    this->DataTypes.insert("MEDIUMBLOB",enuSQLAbstractDataType::String);
    this->DataTypes.insert("LONGBLOB",enuSQLAbstractDataType::String);
    this->DataTypes.insert("LONGTEXT",enuSQLAbstractDataType::String);
    this->DataTypes.insert("ENUM",enuSQLAbstractDataType::String);
    this->DataTypes.insert("SET",enuSQLAbstractDataType::String);

    this->DataTypes.insert("JSON",enuSQLAbstractDataType::JSON);

    this->DataTypes.insert("BINARY",enuSQLAbstractDataType::Binary);
    this->DataTypes.insert("VARBINARY",enuSQLAbstractDataType::Binary);

}

clsDACDriver_MySQL::~clsDACDriver_MySQL()
{ ; }

QStringList clsDACDriver_MySQL::bindSPQuery(const QString& _spName,
                                            const SPParams_t& _spParams,
                                            const QVariantMap &_spArgs,
                                            QStringList& _boundingVars)
{
    QString SetQueryStr;
    QString CallQueryStr = "CALL "+_spName + "(";
    bool ParamAdded2Query = false;
    bool ParamAdded2Set = false;

    foreach(const stuSPParam& SPParam, _spParams) {
        QVariant ParamValue;
        switch(SPParam.Dir) {
        case enuSPParamDir::In:
            if (_spArgs.contains(SPParam.Name) == false)
                throw exTargomanDBMUnableToBindQuery(QString("Obligatory variable %1 for calling %2 not found").arg(SPParam.Name, _spName));
            ParamValue = _spArgs.value(SPParam.Name);
            if (ParamAdded2Query)
                CallQueryStr += ", ";

            if (SPParam.VarType == enuSQLAbstractDataType::Binary)
                CallQueryStr += QString("x\'%1\'").arg(ParamValue.toByteArray().toHex().data());
            else if (SPParam.VarType == enuSQLAbstractDataType::JSON){
                QJsonDocument JsonDoc;
                if(ParamValue.canConvert<QJsonDocument>())
                    JsonDoc = ParamValue.toJsonDocument();
                else if (ParamValue.canConvert<QJsonObject>()){
                    QJsonObject JsonObject = ParamValue.toJsonObject();
                    JsonDoc = JsonObject.isEmpty() ? QJsonDocument() : QJsonDocument::fromVariant(JsonObject.toVariantMap());
                }else if (ParamValue.canConvert<QJsonArray>()){
                    QJsonArray JsonArray = ParamValue.toJsonArray();
                    JsonDoc = JsonArray.isEmpty() ? QJsonDocument() : QJsonDocument::fromVariant(JsonArray.toVariantList());
                }else
                    throw exTargomanDBMUnableToBindQuery(QString("Param %1 has type of JSON but passed argument is not json").arg(SPParam.Name));

                if(JsonDoc.isNull())
                    CallQueryStr += "NULL";
                else
                    CallQueryStr += QString("'%1'").arg(JsonDoc.toJson(QJsonDocument::Compact).replace("\'", "\\\\\\\'").constData());
            }else if (ParamValue.isNull())
                CallQueryStr += "NULL";
            else
                CallQueryStr += QString("'%1'").arg(ParamValue.toString().replace("\'", "\\\\\\\'"));

            ParamAdded2Query = true;
            break;
        case enuSPParamDir::InOut:
            if (_spArgs.contains(SPParam.Name) == false)
                throw exTargomanDBM(QString("Obligatory variable %1 for calling %2 not found").arg(SPParam.Name, _spName));
            ParamValue = _spArgs.value(SPParam.Name);

            if (ParamAdded2Query)
                CallQueryStr += ", ";
            if (ParamAdded2Set)
                SetQueryStr += ", ";
            else
                SetQueryStr = "SET ";

            if (SPParam.VarType == enuSQLAbstractDataType::Binary)
                SetQueryStr += QString("x\'%1\'").arg(ParamValue.toByteArray().toHex().data());
            else if (SPParam.VarType == enuSQLAbstractDataType::JSON){
                QJsonDocument JsonDoc = ParamValue.toJsonDocument();
                if(JsonDoc.isNull())
                    CallQueryStr += "NULL";
                else
                    CallQueryStr += QString("'%1'").arg(JsonDoc.toJson().replace("\'", "\\\\\\\'").constData());
            }else if (ParamValue.isNull())
                SetQueryStr += QString("%1 = NULL").arg(SPParam.Value.toString());
            else
                SetQueryStr += QString("%1 = '%2'").arg(
                            SPParam.Value.toString()).arg(
                            ParamValue.toString().replace("\'", "\\\'"));

            CallQueryStr += SPParam.Value.toString();
            ParamAdded2Query = true;
            ParamAdded2Set = true;
            _boundingVars.append(SPParam.Value.toString());
            break;
        case enuSPParamDir::Out:
            if (ParamAdded2Query)
                CallQueryStr += ", ";

            CallQueryStr += SPParam.Value.toString();
            ParamAdded2Query = true;
            _boundingVars.append(SPParam.Value.toString());
            break;
        }
    }

    if (SetQueryStr.isEmpty())
        return QStringList()<<CallQueryStr + ")";
    else
        return QStringList()<<SetQueryStr<<CallQueryStr + ")";
}

QString clsDACDriver_MySQL::boundSPQuery(const QString& _spName,
                                         const QStringList& _boundingVars)
{
    Q_UNUSED(_spName)

    return "SELECT " + _boundingVars.join(",");
}


SPParams_t clsDACDriver_MySQL::getSPParams(QSqlQuery& _connectedQuery,
                                           const QString _schema,
                                           const QString& _spName)
{
    QString Schema = (_spName.contains('.') ? _spName.split('.').first() : _schema);
    QString Procedure = (_spName.contains('.') ? _spName.split('.').last() : _spName);
    _connectedQuery.clear();
    _connectedQuery.prepare("SELECT PARAMETER_NAME,"
                            "       DATA_TYPE,"
                            "       PARAMETER_MODE"
                            "  FROM INFORMATION_SCHEMA.PARAMETERS"
                            " WHERE SPECIFIC_SCHEMA=?"
                            "   AND SPECIFIC_NAME=?"
                            );
    _connectedQuery.addBindValue(Schema);
    _connectedQuery.addBindValue(Procedure);
    DACImpl::instance().runPreparedQuery(this,
                                     _connectedQuery,
                                     "Retriving SP Parameters Order");
    SPParams_t SPParams;

    while(_connectedQuery.next()){
        QString VarName = _connectedQuery.value(0).toString();
        QString ParamTypeStr = _connectedQuery.value(1).toString();
        QString VarType = _connectedQuery.value(2).toString().toUpper();
        if(VarType == "IN")
            SPParams.append(stuSPParam(enuSPParamDir::In,
                                       VarName,
                                       this->abstractDataType(ParamTypeStr)));
        else if (VarType == "OUT")
            SPParams.append(stuSPParam(enuSPParamDir::Out,
                                       VarName,
                                       this->abstractDataType(ParamTypeStr),
                                       DEFAULT_OUTVAR_PATTERN + VarName));
        else if (VarType == "INOUT")
            SPParams.append(stuSPParam(enuSPParamDir::InOut,
                                       VarName,
                                       this->abstractDataType(ParamTypeStr),
                                       DEFAULT_OUTVAR_PATTERN + VarName));
        else
            throw exTargomanDBMUnableToExecuteQuery("Invalid SP param type: " + VarType);
    }

    if(SPParams.isEmpty()){
        _connectedQuery.clear();
        _connectedQuery.prepare("SELECT 1"
                                "  FROM INFORMATION_SCHEMA.ROUTINES"
                                " WHERE ROUTINE_SCHEMA=?"
                                "   AND ROUTINE_NAME=?"
                                );
        _connectedQuery.addBindValue(Schema);
        _connectedQuery.addBindValue(Procedure);
        DACImpl::instance().runPreparedQuery(this,
                                         _connectedQuery,
                                         "check SP existence without input params");

        if(_connectedQuery.next() == false)
            throw exTargomanDBMUnableToExecuteQuery("Stored Procedure  [" + Schema + '.' + Procedure + " Not found.");
    }
    return SPParams;
}

enuSQLAbstractDataType::Type   clsDACDriver_MySQL::abstractDataType(const QString& _typeStr)
{
    enuSQLAbstractDataType::Type DataType = this->DataTypes.value(_typeStr.toUpper(), enuSQLAbstractDataType::Unknown);

    if (DataType != enuSQLAbstractDataType::Unknown)
        return DataType;
    else
        throw exTargomanDBM(QString("MySQL Data Type: %1 not Supported ").arg(_typeStr));
}

QStringList clsDACDriver_MySQL::whichOnesAreUpdated(const QSqlDatabase& _dbc,
                                                    const QStringList& _tableNames,
                                                    QDateTime& _pLastCheckTime)
{
    Q_UNUSED(_dbc)
    Q_UNUSED(_tableNames)
    Q_UNUSED(_pLastCheckTime)

    // Good suggestion for checking MySQL change:
    //      http://dba.stackexchange.com/questions/9569/fastest-way-to-check-if-innodb-table-has-changed
    //      http://stackoverflow.com/questions/7971761/should-i-migrate-from-mysql-to-percona-server

    // Final Result: in standard MySQL there is no way to detect updates so it's beetter to migrate to Percona-Server

    return _tableNames;
}

}
}
}
}
