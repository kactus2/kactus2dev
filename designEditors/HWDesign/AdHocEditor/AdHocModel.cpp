//-----------------------------------------------------------------------------
// File: AdHocModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 20.2.2012
//
// Description:
// Table model for visualizing ad-hoc visibility for dataSource ports.
//-----------------------------------------------------------------------------

#include "AdHocModel.h"
#include "AdHocColumns.h"

#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Port.h>

#include <designEditors/HWDesign/HWComponentItem.h>
#include <designEditors/HWDesign/AdHocEnabled.h>
#include <designEditors/HWDesign/HWConnectionEndpoint.h>

#include <designEditors/HWDesign/undoCommands/AdHocVisibilityChangeCommand.h>

#include <common/IEditProvider.h>

//-----------------------------------------------------------------------------
// Function: AdHocModel::AdHocModel()
//-----------------------------------------------------------------------------
AdHocModel::AdHocModel(QObject *parent):
QAbstractTableModel(parent),
dataSource_(0),
table_(new QList<QSharedPointer<Port> > ())
{

}

//-----------------------------------------------------------------------------
// Function: AdHocModel::~AdHocModel()
//-----------------------------------------------------------------------------
AdHocModel::~AdHocModel()
{

}

//-----------------------------------------------------------------------------
// Function: AdHocModel::setDataSource()
//-----------------------------------------------------------------------------
void AdHocModel::setDataSource(AdHocEnabled* dataSource, QSharedPointer<IEditProvider> editProvider)
{
    dataSource_ = dataSource;
    editProvider_ = editProvider;

    beginResetModel();

    if (dataSource_ != 0)
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
// Function: AdHocModel::rowCount()
//-----------------------------------------------------------------------------
int AdHocModel::rowCount(QModelIndex const& parent /*= QModelIndex()*/) const
{
    if (parent.isValid() || !table_)
    {
        return 0;
    }

    return table_->size();
}

//-----------------------------------------------------------------------------
// Function: AdHocModel::columnCount()
//-----------------------------------------------------------------------------
int AdHocModel::columnCount(QModelIndex const& parent /*= QModelIndex()*/) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return AdHocColumns::ADHOC_COL_COUNT;
}

//-----------------------------------------------------------------------------
// Function: AdHocModel::data()
//-----------------------------------------------------------------------------
QVariant AdHocModel::data(QModelIndex const& index, int role /*= Qt::DisplayRole*/) const
{
    // Check for invalid index.
    if (!index.isValid() || index.row() < 0 || index.row() >= table_->size())
    {
        return QVariant();
    }

    QSharedPointer<Port> adhocPort = table_->at(index.row());

    if (role == Qt::DisplayRole)
    {
        if (index.column() == AdHocColumns::ADHOC_COL_NAME)
        {
            return table_->at(index.row())->name();
        }
        else if (index.column() == AdHocColumns::ADHOC_COL_DIRECTION)
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
        if (index.column() == AdHocColumns::ADHOC_COL_VISIBILITY)
        {
            if (dataSource_->isPortAdHocVisible(adhocPort->name()))
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
        if (adHocPortIsRemovable(adhocPort))
        {
            return QColor(Qt::black);
        }
        else
        {
            return QColor(Qt::gray);
        }
    }

    else if (role == Qt::ToolTipRole)
    {
        if (!adHocPortIsRemovable(adhocPort))
        {
            QString connectedText = QObject::tr("Connected ad hoc port cannot be disabled.");
            return connectedText;
        }
        else
        {
            return QString();
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: AdHocModel::headerData()
//-----------------------------------------------------------------------------
QVariant AdHocModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            if (section == AdHocColumns::ADHOC_COL_NAME)
            {
                return tr("Name");
            }
            else if (section == AdHocColumns::ADHOC_COL_DIRECTION)
            {
                return tr("Direction");
            }
            else if (section == AdHocColumns::ADHOC_COL_VISIBILITY)
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
// Function: AdHocModel::setData()
//-----------------------------------------------------------------------------
bool AdHocModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
{
    // Check for invalid index.
    if (!index.isValid() || index.row() < 0 || index.row() >= table_->size())
    {
        return false;
    }

    if (role == Qt::CheckStateRole)
    {
        QSharedPointer<QUndoCommand> cmd(
            new AdHocVisibilityChangeCommand(dataSource_, table_->at(index.row())->name(), value == Qt::Checked));
        editProvider_->addCommand(cmd);
        cmd->redo();

        emit dataChanged(index, index);
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AdHocModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags AdHocModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    Qt::ItemFlags flags = Qt::ItemIsEnabled;

    QSharedPointer<Port> indexedPort = table_->at(index.row());
    if (index.column() == AdHocColumns::ADHOC_COL_VISIBILITY && adHocPortIsRemovable(indexedPort))
    {
        flags |= Qt::ItemIsUserCheckable | Qt::ItemIsSelectable;
    }

    return flags;
}

//-----------------------------------------------------------------------------
// Function: AdHocModel::isAdHocPortRemovable()
//-----------------------------------------------------------------------------
bool AdHocModel::adHocPortIsRemovable(QSharedPointer<Port> port) const
{
    HWConnectionEndpoint* endpoint = dataSource_->getDiagramAdHocPort(port->name());
    if (endpoint)
    {
        if (!endpoint->getConnections().isEmpty() || 
            (endpoint->getOffPageConnector() && !endpoint->getOffPageConnector()->getConnections().isEmpty()))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AdHocModel::updateVisibilities()
//-----------------------------------------------------------------------------
void AdHocModel::updateVisibilities()
{
    emit dataChanged(index(0, AdHocColumns::ADHOC_COL_VISIBILITY), index(table_->size() - 1,
        AdHocColumns::ADHOC_COL_VISIBILITY));
}
