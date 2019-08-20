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

#include <QSqlQuery>
#include <QStringList>
#include <QDateTime>
#include <QVariant>
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
    this->DataTypes.insert("JSON",enuSQLAbstractDataType::String);
    this->DataTypes.insert("TINYBLOB",enuSQLAbstractDataType::String);
    this->DataTypes.insert("TINYTEXT",enuSQLAbstractDataType::String);
    this->DataTypes.insert("BLOB",enuSQLAbstractDataType::String);
    this->DataTypes.insert("TEXT",enuSQLAbstractDataType::String);
    this->DataTypes.insert("MEDIUMBLOB",enuSQLAbstractDataType::String);
    this->DataTypes.insert("LONGBLOB",enuSQLAbstractDataType::String);
    this->DataTypes.insert("LONGTEXT",enuSQLAbstractDataType::String);
    this->DataTypes.insert("ENUM",enuSQLAbstractDataType::String);
    this->DataTypes.insert("SET",enuSQLAbstractDataType::String);

    this->DataTypes.insert("BINARY",enuSQLAbstractDataType::Binary);
    this->DataTypes.insert("VARBINARY",enuSQLAbstractDataType::Binary);

}

QStringList clsDACDriver_MySQL::bindSPQuery(const QString& _spName,
                                            const SPParams_t& _spParams,
                                            const QVariantList &_spArgs,
                                            QStringList& _boundingVars)
{
    /*   QString SetQueryStr;
    QString CallQueryStr = "CALL "+_spName + "(";
    bool ParamAdded2Query = false;
    bool ParamAdded2Set = false;

    foreach(const stuSPParam& SPParam, _spParams) {
        QVariant ParamValue;
        switch(SPParam.Dir) {
        case enuSPParamDir::In:
            if (_spArgs.contains(SPParam.Name) == false)
                throw exTargomanDBM(QString("Obligatory variable %1 for calling %2 not found").arg(SPParam.Name, _spName));
            ParamValue = _spArgs.value(SPParam.Name);
            if (ParamAdded2Query)
                CallQueryStr += ", ";

            if (SPParam.VarType == enuSQLAbstractDataType::Binary)
                CallQueryStr += QString("x\'%1\'").arg(ParamValue.toByteArray().toHex().data());
            else if (ParamValue.isNull())
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
            else if (ParamValue.isNull())
                SetQueryStr += QString("%1 = NULL").arg(SPParam.Value.toString());
            else
                SetQueryStr += QString("%1 = '%2'").arg(
                            SPParam.Value.toString()).arg(
                            ParamValue.toString().replace("\'", "\\\'"));

            CallQueryStr += SPParam.Value.toString();
            ParamAdded2Query = true;
            ParamAdded2Set = true;
            _boundingVars->append(SPParam.Value.toString());
            break;
        case enuSPParamDir::Out:
            if (ParamAdded2Query)
                CallQueryStr += ", ";

            CallQueryStr += SPParam.Value.toString();
            ParamAdded2Query = true;
            _boundingVars->append(SPParam.Value.toString());
            break;
        }
    }

    if (SetQueryStr.isEmpty())
        return QStringList()<<CallQueryStr + ")";
    else
        return QStringList()<<SetQueryStr<<CallQueryStr + ")";
*/
}

QString clsDACDriver_MySQL::boundSPQuery(const QString& _spName,
                                         const QStringList& _boundingVars)
{
    Q_UNUSED(_spName)

    return "SELECT " + _boundingVars.join(",");
}

const QString DEFAULT_OUTVAR_PATTERN = "@";

SPParams_t clsDACDriver_MySQL::getSPParams(QSqlQuery& _connectedQuery,
                                           const QString _schema,
                                           const QString& _spName)
{
    _connectedQuery.clear();
    _connectedQuery.prepare("SELECT param_list FROM mysql.proc p where db='" + _schema + "' AND name='" + _spName + "'");
    DACImpl::instance().runQueryBase(this,
                                     _connectedQuery,
                                     "Retriving SP Parameters Order");

    SPParams_t SPParams;

    if (_connectedQuery.next()) {
        QStringList ParamsList = _connectedQuery.value(0).toString().trimmed().split(',', QString::SkipEmptyParts);
        if (ParamsList.first().size()) {
            foreach (const QString& Parameter, ParamsList) {
                QStringList ParamterSections = Parameter.split(QRegExp("\\s+"), QString::SkipEmptyParts);
                QString VarName =  QString(ParamterSections.at(1)).remove("`");
                QString ParamTypeStr = ParamterSections.at(2).split(QRegExp("\\(|,")).at(0);

                if (ParamterSections.at(0).toUpper() == "OUT")
                    SPParams.append(stuSPParam(enuSPParamDir::Out,
                                               VarName,
                                               this->abstractDataType(ParamTypeStr),
                                               DEFAULT_OUTVAR_PATTERN + VarName));
                else if (ParamterSections.at(0).toUpper() == "IN")
                    SPParams.append(stuSPParam(enuSPParamDir::In,
                                               VarName,
                                               this->abstractDataType(ParamTypeStr)));
                else if (ParamterSections.at(0).toUpper() == "INOUT")
                    SPParams.append(stuSPParam(enuSPParamDir::InOut,
                                               VarName,
                                               this->abstractDataType(ParamTypeStr),
                                               DEFAULT_OUTVAR_PATTERN + VarName));
                else
                    throw exTargomanDBMUnableToExecuteQuery("Invalid SP params count: " + _connectedQuery.value(0).toString());
            }
        }
        _connectedQuery.clear();
        return SPParams;
    }
    else
        throw exTargomanDBMUnableToExecuteQuery(QString("Stored procedure: %1 not found in DB").arg(_spName));
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
