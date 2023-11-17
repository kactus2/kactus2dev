//-----------------------------------------------------------------------------
// File: PowerDomainLinkModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 19.10.2023
//
// Description:
// Model for editing power domain links in a component instance.
//-----------------------------------------------------------------------------

#include "PowerDomainLinkModel.h"
#include "PowerDomainLinkColumns.h"

#include <IPXACTmodels/common/Choice.h>
#include <IPXACTmodels/common/validators/ChoiceValidator.h>

#include <common/KactusColors.h>

//-----------------------------------------------------------------------------
// Function: PowerDomainLinkModel::PowerDomainLinkModel()
//-----------------------------------------------------------------------------
PowerDomainLinkModel::PowerDomainLinkModel(QObject* parent) :
QAbstractTableModel(parent)
{
 
}

//-----------------------------------------------------------------------------
// Function: PowerDomainLinkModel::rowCount()
//-----------------------------------------------------------------------------
int PowerDomainLinkModel::rowCount(const QModelIndex&) const
{
    if (powerDomains_.isNull())
    {
    return 0;
    }

    return powerDomains_->count();
}

//-----------------------------------------------------------------------------
// Function: PowerDomainLinkModel::columnCount()
//-----------------------------------------------------------------------------
int PowerDomainLinkModel::columnCount(const QModelIndex& /*parent*/) const
{
    return PowerDomainLinkColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: PowerDomainLinkModel::data()
//-----------------------------------------------------------------------------
QVariant PowerDomainLinkModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= powerDomains_->count())
    {
        return QVariant();
    }

    auto const& domain = powerDomains_->at(index.row());

    if (role == Qt::DisplayRole)
    {
        if (index.column() == PowerDomainLinkColumns::EXTERNAL_LINK)
        {
            return domain->externalReference_;
        }
        else if (index.column() == PowerDomainLinkColumns::INTERNAL_LINK)
        {
            return domain->internalReferences_.join(QLatin1Char(','));
        }
    }
    else if (role == Qt::EditRole && index.column() == PowerDomainLinkColumns::INTERNAL_LINK)
    {
        return domain->internalReferences_;
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: PowerDomainLinkModel::headerData()
//-----------------------------------------------------------------------------
QVariant PowerDomainLinkModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole || section > columnCount(QModelIndex()))
    {
        return QVariant();
    }

    if (section == PowerDomainLinkColumns::EXTERNAL_LINK)
    {
        return tr("External domain");
    }
    else if (section == PowerDomainLinkColumns::INTERNAL_LINK)
    {
        return tr("Internal domain(s)");
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: PowerDomainLinkModel::setData()
//-----------------------------------------------------------------------------
bool PowerDomainLinkModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid() || index.row() < 0 )
    {
        return false;
    }

    auto const& domain = powerDomains_->at(index.row());

    if (role == Qt::EditRole)
    {
        if (index.column() == PowerDomainLinkColumns::EXTERNAL_LINK)
        {
            domain->externalReference_ = value.toString();
        }
        else if (index.column() == PowerDomainLinkColumns::INTERNAL_LINK)
        {
            domain->internalReferences_ = value.toStringList();
        }

        emit dataChanged(index, index);
        emit contentChanged();
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PowerDomainLinkModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags PowerDomainLinkModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }
    
    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

//-----------------------------------------------------------------------------
// Function: PowerDomainLinkModel::setPowerDomains()
//-----------------------------------------------------------------------------
void PowerDomainLinkModel::setPowerDomains(ComponentInstance::PowerDomainLink::List powerDomains) noexcept
{
    beginResetModel();
    powerDomains_ = powerDomains;
    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: PowerDomainLinkModel::addItem()
//-----------------------------------------------------------------------------
void PowerDomainLinkModel::addItem(const QModelIndex& index)
{
    int row = powerDomains_->count();

    if (index.isValid())
    {
        row = index.row();
    }
    
    beginInsertRows(QModelIndex(), row, row);
    
    QSharedPointer<ComponentInstance::PowerDomainLink> newLink(new ComponentInstance::PowerDomainLink());

    powerDomains_->insert(row, newLink);
    
    endInsertRows();
    
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: PowerDomainLinkModel::removeItem()
//-----------------------------------------------------------------------------
void PowerDomainLinkModel::removeItem(const QModelIndex& index)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= powerDomains_->size())
    {
        return;
    }
    
    beginRemoveRows(QModelIndex(), index.row(), index.row());
    
    powerDomains_->removeAt(index.row());
    
    endRemoveRows();
    
    emit contentChanged();

}
