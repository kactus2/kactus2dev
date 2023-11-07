//-----------------------------------------------------------------------------
// File: AccessPoliciesModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 28.8.2023
//
// Description:
// Model describing the access policies of an address block, register or register file.
//-----------------------------------------------------------------------------

#include "AccessPoliciesModel.h"

#include <KactusAPI/include/RegisterInterface.h>
#include <KactusAPI/include/AccessPolicyInterface.h>

#include <common/KactusColors.h>

//-----------------------------------------------------------------------------
// Function: AccessPoliciesModel::AccessPoliciesModel()
//-----------------------------------------------------------------------------
AccessPoliciesModel::AccessPoliciesModel(AccessPolicyInterface* accessPolicyInterface, QObject* parent) :
    QAbstractTableModel(parent),
    interface_(accessPolicyInterface)
{

}

//-----------------------------------------------------------------------------
// Function: AccessPoliciesModel::rowCount()
//-----------------------------------------------------------------------------
int AccessPoliciesModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return interface_->getAccessPolicyCount();
}

//-----------------------------------------------------------------------------
// Function: AccessPoliciesModel::columnCount()
//-----------------------------------------------------------------------------
int AccessPoliciesModel::columnCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return 2; // mode references and access
}

//-----------------------------------------------------------------------------
// Function: AccessPoliciesModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags AccessPoliciesModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: AccessPoliciesModel::headerData()
//-----------------------------------------------------------------------------
QVariant AccessPoliciesModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    if (orientation != Qt::Horizontal)
    {
        return QVariant();
    }
    if (role == Qt::DisplayRole)
    {
        if (section == 0)
        {
            return QStringLiteral("Mode(s)");
        }

        else if (section == 1)
        {
            return QStringLiteral("Access");
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: AccessPoliciesModel::data()
//-----------------------------------------------------------------------------
QVariant AccessPoliciesModel::data(const QModelIndex& index, int role /*= Qt::DisplayRole*/) const
{
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        return valueForIndex(index);
    }

    else if (role == Qt::BackgroundRole)
    {
        if (index.flags() == Qt::NoItemFlags)
        {
            return KactusColors::DISABLED_FIELD;
        }

        return KactusColors::REGULAR_FIELD;
    }

    else if (role == Qt::ToolTipRole)
    {
        return valueForIndex(index);
    }

    else if (role == Qt::ForegroundRole)
    {
        if (validateIndex(index))
        {
            return KactusColors::REGULAR_TEXT;
        }
        
        return KactusColors::ERROR;
    }

    else if (role == Qt::UserRole)
    {
        if (index.column() == 0)
        {
            QVariant modeRefsVariant;

            using ModeRefList = QSharedPointer<QList<QSharedPointer<ModeReference> > >;

            // Mode references for current index and all other mode refs.
            QPair<ModeRefList, ModeRefList> modeRefs;

            auto currentModeRefs = interface_->getAccesPolicyModeReferences(index.row());
            auto otherModeRefsInUse = interface_->getModeReferencesInUse(index.row());

            modeRefs.first = currentModeRefs;
            modeRefs.second = otherModeRefsInUse;

            modeRefsVariant.setValue(modeRefs);

            return modeRefsVariant;
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: AccessPoliciesModel::setData()
//-----------------------------------------------------------------------------
bool AccessPoliciesModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
{
    if (!index.isValid() || index.row() < 0 ||
        index.row() >= interface_->getAccessPolicyCount() ||
        !(flags(index) & Qt::ItemIsEditable) || role != Qt::EditRole)
    {
        return false;
    }

    if (role == Qt::EditRole)
    {
        // Set modified mode reference data to access policy.
        if (index.column() == 0)
        {
            QSharedPointer<QList<QSharedPointer<ModeReference> > > updatedModeRefs =
                value.value<QSharedPointer<QList<QSharedPointer<ModeReference> > > >();

            interface_->setAccessPolicyModeReferences(index.row(), updatedModeRefs);
        }
        else if (index.column() == 1)
        {
            interface_->setAccess(value.toString().toStdString(), index.row());
        }
    }

    emit dataChanged(index, index);
    return true;
}

//-----------------------------------------------------------------------------
// Function: AccessPoliciesModel::onAddRow()
//-----------------------------------------------------------------------------
void AccessPoliciesModel::onAddRow(QModelIndex const& index)
{
    int lastRow = interface_->getAccessPolicyCount();

    beginInsertRows(QModelIndex(), lastRow, lastRow);
    interface_->addAccessPolicy();
    endInsertRows();

    emit invalidateFilter();
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AccessPoliciesModel::onRemoveItem()
//-----------------------------------------------------------------------------
void AccessPoliciesModel::onRemoveItem(QModelIndex const& index)
{
    if (!index.isValid() || index.row() < 0 ||
        index.row() >= interface_->getAccessPolicyCount())
    {
        return;
    }

    beginRemoveRows(QModelIndex(), index.row(), index.row());
    interface_->removeAccessPolicy(index.row());
    endRemoveRows();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AccessPoliciesModel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant AccessPoliciesModel::valueForIndex(QModelIndex const& index) const
{
    if (index.column() == 0) // Mode ref display values
    {
        auto modeRefsList = interface_->getModeRefs(index.row());

        QStringList valueList;
        for (auto const& [reference, priority] : modeRefsList)
        {
            valueList.append(QString::fromStdString(reference));
        }

        return valueList.join(", ");
    }

    else if (index.column() == 1) // Access display values
    {
        return QString::fromStdString(interface_->getAccessString(index.row()));
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: AccessPoliciesModel::validateIndex()
//-----------------------------------------------------------------------------
bool AccessPoliciesModel::validateIndex(QModelIndex const& index) const
{
    if (index.column() == 0)
    {
        return interface_->hasValidAccessPolicies();
    }

    return true;
}
