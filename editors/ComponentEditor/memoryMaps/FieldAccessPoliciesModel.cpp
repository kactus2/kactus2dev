//-----------------------------------------------------------------------------
// File: FieldAccessPoliciesModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: 
// Date: 8.8.2023
//
// Description:
// 
//-----------------------------------------------------------------------------

#include "FieldAccessPoliciesModel.h"
#include "FieldAccessPolicyColumns.h"

#include <common/KactusColors.h>

//-----------------------------------------------------------------------------
// Function: FieldAccessPoliciesModel::FieldAccessPoliciesModel()
//-----------------------------------------------------------------------------
FieldAccessPoliciesModel::FieldAccessPoliciesModel(QString const& fieldName, FieldInterface* fieldInterface, QObject* parent):
    QAbstractTableModel(parent),
    fieldInterface_(fieldInterface),
    fieldName_(fieldName) 
{

}

//-----------------------------------------------------------------------------
// Function: FieldAccessPoliciesModel::rowCount()
//-----------------------------------------------------------------------------
int FieldAccessPoliciesModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return fieldInterface_->getAccessPolicyCount(fieldName_.toStdString());
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPoliciesModel::columnCount()
//-----------------------------------------------------------------------------
int FieldAccessPoliciesModel::columnCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return FieldAccessPolicyColumns::COUNT;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPoliciesModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags FieldAccessPoliciesModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPoliciesModel::headerData()
//-----------------------------------------------------------------------------
QVariant FieldAccessPoliciesModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    if (orientation != Qt::Horizontal)
    {
        return QVariant();
    }
    if (Qt::DisplayRole == role)
    {
        if (section == FieldAccessPolicyColumns::MODE)
        {
            return QStringLiteral("Mode(s)");
        }
        else if (section == FieldAccessPolicyColumns::TYPE_DEFINITION)
        {
            return QStringLiteral("Type definition");
        }
        else if (section == FieldAccessPolicyColumns::ACCESS)
        {
            return QStringLiteral("Access");
        }
        else if (section == FieldAccessPolicyColumns::MODIFIED_WRITE)
        {
            return QStringLiteral("Modified write\nvalue");
        }
        else if (section == FieldAccessPolicyColumns::WRITE_VALUE_CONSTRAINT)
        {
            return QStringLiteral("Write value\nconstraint");
        }
        else if (section == FieldAccessPolicyColumns::READ_ACTION)
        {
            return QStringLiteral("Read action");
        }
        else if (section == FieldAccessPolicyColumns::READ_RESPONSE)
        {
            return QStringLiteral("Read response");
        }
        else if (section == FieldAccessPolicyColumns::TESTABLE)
        {
            return QStringLiteral("Testable");
        }
        else if (section == FieldAccessPolicyColumns::RESERVED)
        {
            return QStringLiteral("Reserved");
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPoliciesModel::data()
//-----------------------------------------------------------------------------
QVariant FieldAccessPoliciesModel::data(const QModelIndex& index, int role /*= Qt::DisplayRole*/) const
{
    if (role == Qt::DisplayRole)
    {
        if (index.column() == FieldAccessPolicyColumns::ACCESS)
        {
            return QString::fromStdString(fieldInterface_->getAccessString(fieldName_.toStdString(), index.row()));
        }
        
        else if (index.column() == FieldAccessPolicyColumns::MODE)
        {
            auto modeRefsList = fieldInterface_->getModeRefs(fieldName_.toStdString(), index.row());
            QStringList list;
            for (auto elem : modeRefsList)
            {
                list.append(QString::fromStdString(elem));
            }

            return list.join(", ");
        }
    }

    else if (role == Qt::BackgroundRole)
    {
        return KactusColors::REGULAR_FIELD;
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPoliciesModel::setData()
//-----------------------------------------------------------------------------
bool FieldAccessPoliciesModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
{
    return true;
}

