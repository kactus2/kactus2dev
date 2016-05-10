//-----------------------------------------------------------------------------
// File: AdHocVisibilityModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 20.2.2012
//
// Description:
// Table model for visualizing ad-hoc visibility for dataSource ports.
//-----------------------------------------------------------------------------

#include "AdHocVisibilityModel.h"
#include "AdHocVisibilityColumns.h"

#include <common/IEditProvider.h>

#include <designEditors/HWDesign/HWComponentItem.h>
#include <designEditors/HWDesign/AdHocEnabled.h>
#include <designEditors/HWDesign/HWConnectionEndpoint.h>
#include <designEditors/HWDesign/AdHocVisibilityEditor/AdHocVisibilityPolicy.h>
#include <designEditors/HWDesign/undoCommands/AdHocVisibilityChangeCommand.h>

#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Port.h>

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityModel::AdHocVisibilityModel()
//-----------------------------------------------------------------------------
AdHocVisibilityModel::AdHocVisibilityModel(QSharedPointer<AdHocVisibilityPolicy> visibilityPolicy,
    QObject *parent):
QAbstractTableModel(parent),
visibilityPolicy_(visibilityPolicy),
table_(new QList<QSharedPointer<Port> > ())
{

}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityModel::~AdHocVisibilityModel()
//-----------------------------------------------------------------------------
AdHocVisibilityModel::~AdHocVisibilityModel()
{

}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityModel::setDataSource()
//-----------------------------------------------------------------------------
void AdHocVisibilityModel::setDataSource(AdHocEnabled* dataSource, QSharedPointer<IEditProvider> editProvider)
{
    editProvider_ = editProvider;

    beginResetModel();

    if (dataSource != 0)
    {
        table_ = dataSource->getPorts();
    }
    else
    {
        table_.clear();
    }

    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityModel::rowCount()
//-----------------------------------------------------------------------------
int AdHocVisibilityModel::rowCount(QModelIndex const& parent /*= QModelIndex()*/) const
{
    if (parent.isValid() || !table_)
    {
        return 0;
    }

    return table_->size();
}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityModel::columnCount()
//-----------------------------------------------------------------------------
int AdHocVisibilityModel::columnCount(QModelIndex const& parent /*= QModelIndex()*/) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return AdHocVisibilityColumns::ADHOC_COL_COUNT;
}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityModel::data()
//-----------------------------------------------------------------------------
QVariant AdHocVisibilityModel::data(QModelIndex const& index, int role /*= Qt::DisplayRole*/) const
{
    // Check for invalid index.
    if (!index.isValid() || index.row() < 0 || index.row() >= table_->size())
    {
        return QVariant();
    }

    QSharedPointer<Port> adhocPort = table_->at(index.row());

    if (role == Qt::DisplayRole)
    {
        if (index.column() == AdHocVisibilityColumns::ADHOC_COL_NAME)
        {
            return table_->at(index.row())->name();
        }
        else if (index.column() == AdHocVisibilityColumns::ADHOC_COL_DIRECTION)
        {
            return DirectionTypes::direction2Str(adhocPort->getDirection());
        }
        else
        {
            return QVariant();
        }
    }
    else if (Qt::CheckStateRole == role)
    {
        if (index.column() == AdHocVisibilityColumns::ADHOC_COL_VISIBILITY)
        {
            if (visibilityPolicy_->isPortAdhocVisible(adhocPort->name()))
            {
                return Qt::Checked;
            }
            else
            {
                return Qt::Unchecked;
            }
        }
        else
        {
            return QVariant();
        }
    }

    else if (role == Qt::ForegroundRole)
    {
        if (!visibilityPolicy_->canChangeVisibility(adhocPort))
        {
            return QColor(Qt::gray);
        }
        else
        {
            return QColor(Qt::black);
        }
    }

    else if (role == Qt::ToolTipRole)
    {
        if (visibilityPolicy_->portHasConnections(adhocPort))
        {
            QString connectedText = QObject::tr("Connected ad hoc port cannot be disabled.");
            return connectedText;
        }
        else if (visibilityPolicy_->portHasTieOffConnection(adhocPort))
        {
            QString tieOffText = QObject::tr("Port with a tie off value cannot be disabled.");
            return tieOffText;
        }
        else if (adhocPort->isAdHocVisible())
        {
            QString componentPortVisibleText = QObject::tr("Component determines port %1 to be visible.").
                arg(adhocPort->name());
            return componentPortVisibleText;
        }
        else
        {
            return QString();
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityModel::headerData()
//-----------------------------------------------------------------------------
QVariant AdHocVisibilityModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            if (section == AdHocVisibilityColumns::ADHOC_COL_NAME)
            {
                return tr("Name");
            }
            else if (section == AdHocVisibilityColumns::ADHOC_COL_DIRECTION)
            {
                return tr("Direction");
            }
            else if (section == AdHocVisibilityColumns::ADHOC_COL_VISIBILITY)
            {
                return tr("Ad-hoc");
            }
            else
            {
                return QVariant();
            }
        } 
        // Vertical headers get running numbers.
        else
        {
            return section + 1;
        }
    }
    
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityModel::setData()
//-----------------------------------------------------------------------------
bool AdHocVisibilityModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
{
    // Check for invalid index.
    if (!index.isValid() || index.row() < 0 || index.row() >= table_->size())
    {
        return false;
    }

    if (role == Qt::CheckStateRole)
    {
        QSharedPointer<QUndoCommand> cmd(new AdHocVisibilityChangeCommand(
            visibilityPolicy_->getDataSource(), table_->at(index.row())->name(), value == Qt::Checked));
        editProvider_->addCommand(cmd);
        cmd->redo();

        emit dataChanged(index, index);
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags AdHocVisibilityModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    Qt::ItemFlags flags = Qt::ItemIsEnabled;

    QSharedPointer<Port> indexedPort = table_->at(index.row());
    if (index.column() == AdHocVisibilityColumns::ADHOC_COL_VISIBILITY &&
        visibilityPolicy_->canChangeVisibility(indexedPort))
    {
        flags |= Qt::ItemIsUserCheckable | Qt::ItemIsSelectable;
    }

    return flags;
}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityModel::updateVisibilities()
//-----------------------------------------------------------------------------
void AdHocVisibilityModel::updateVisibilities()
{
    emit dataChanged(index(0, AdHocVisibilityColumns::ADHOC_COL_VISIBILITY), index(table_->size() - 1,
        AdHocVisibilityColumns::ADHOC_COL_VISIBILITY));
}
