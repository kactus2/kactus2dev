//-----------------------------------------------------------------------------
// File: FieldAccessPoliciesModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 8.8.2023
//
// Description:
// Model to manage the field access policies of a field.
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
        
        // Data for displaying mode refs in access policy table.
        else if (index.column() == FieldAccessPolicyColumns::MODE)
        {
            auto const& modeRefsList = fieldInterface_->getModeRefs(fieldName_.toStdString(), index.row());
            QStringList list;
            for (auto const& [reference, priority] : modeRefsList)
            {
                list.append(QString::fromStdString(reference));
            }

            return list.join(", ");
        }

        else if (index.column() == FieldAccessPolicyColumns::MODIFIED_WRITE)
        {
            return QString::fromStdString(fieldInterface_->getModifiedWriteString(fieldName_.toStdString(), index.row()));
        }
    }

    else if (role == Qt::BackgroundRole)
    {
        return KactusColors::REGULAR_FIELD;
    }

    else if (role == Qt::UserRole)
    {
        // Data for mode ref editor.
        if (index.column() == FieldAccessPolicyColumns::MODE)
        {
            auto const& modeRefsList = fieldInterface_->getModeRefs(fieldName_.toStdString(), index.row());

            QList<QPair<QString, int> > modeRefs;
            for (auto const& [reference, priority] : modeRefsList)
            {
                modeRefs.append(QPair<QString, int>({QString::fromStdString(reference), priority }));
            }

            QVariant modeRefsVariant;
            modeRefsVariant.setValue(modeRefs);
            return modeRefsVariant;
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPoliciesModel::setData()
//-----------------------------------------------------------------------------
bool FieldAccessPoliciesModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
{
    if (!index.isValid() || index.row() < 0 || 
        index.row() >= fieldInterface_->getAccessPolicyCount(fieldName_.toStdString()) ||
        !(flags(index) & Qt::ItemIsEditable) || role != Qt::EditRole)
    {
        return false;
    }

    if (index.column() == FieldAccessPolicyColumns::MODE)
    {
        auto modeRefs = value.value<QList<QPair<QString, int> > >();

        std::vector<std::pair<std::string, int> > modeRefsStdType;
        for (auto const& [reference, priority] : modeRefs)
        {
            modeRefsStdType.emplace_back(reference.toStdString(), priority);
        }

        fieldInterface_->setModeRefs(fieldName_.toStdString(), index.row(), modeRefsStdType);
    }

    else if (index.column() == FieldAccessPolicyColumns::ACCESS)
    {
        auto accessTypeStr = value.toString();

        fieldInterface_->setAccess(fieldName_.toStdString(), index.row(), accessTypeStr.toStdString());
    }

    else if (index.column() == FieldAccessPolicyColumns::MODIFIED_WRITE)
    {
        auto modifiedWriteStr = value.toString();

        fieldInterface_->setModifiedWrite(fieldName_.toStdString(), index.row(), modifiedWriteStr.toStdString());
    }

    return true;
}

