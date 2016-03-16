//-----------------------------------------------------------------------------
// File: AdHocBoundsModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 20.2.2012
//
// Description:
// Table model for visualizing ad-hoc visibility for connection ports.
//-----------------------------------------------------------------------------

#include "AdHocBoundsModel.h"

#include "AdHocBoundColumns.h"

#include <designEditors/common/DesignDiagram.h>

#include <designEditors/HWDesign/AdHocConnectionItem.h>
#include <designEditors/HWDesign/HWConnectionEndpoint.h>
#include <designEditors/HWDesign/HWChangeCommands.h>

#include <common/IEditProvider.h>

#include <IPXACTmodels/Component/Port.h>

//-----------------------------------------------------------------------------
// Function: AdHocBoundsModel::AdHocBoundsModel()
//-----------------------------------------------------------------------------
AdHocBoundsModel::AdHocBoundsModel(QObject *parent) : QAbstractTableModel(parent),
    editProvider_(),
    connection_(0)
{

}

//-----------------------------------------------------------------------------
// Function: AdHocBoundsModel::~AdHocBoundsModel()
//-----------------------------------------------------------------------------
AdHocBoundsModel::~AdHocBoundsModel()
{
}

//-----------------------------------------------------------------------------
// Function: AdHocBoundsModel::setConnection()
//-----------------------------------------------------------------------------
void AdHocBoundsModel::setConnection(AdHocConnectionItem* connection, QSharedPointer<IEditProvider> editProvider)
{
    editProvider_ = editProvider;
    connection_ = connection;
    
    beginResetModel();
    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: AdHocBoundsModel::rowCount()
//-----------------------------------------------------------------------------
int AdHocBoundsModel::rowCount(QModelIndex const& parent) const
{
    if (parent.isValid() || connection_ == 0)
    {
        return 0;
    }

    return 2;
}

//-----------------------------------------------------------------------------
// Function: AdHocBoundsModel::columnCount()
//-----------------------------------------------------------------------------
int AdHocBoundsModel::columnCount(QModelIndex const& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return AdHocBoundColumns::ADHOC_BOUNDS_COL_COUNT;
}

//-----------------------------------------------------------------------------
// Function: AdHocBoundsModel::data()
//-----------------------------------------------------------------------------
QVariant AdHocBoundsModel::data(QModelIndex const& index, int role) const
{
    // Check for invalid index.
    if (!index.isValid() || connection_ == 0 || index.row() < 0 || index.row() >= 2)
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        if (index.column() == AdHocBoundColumns::ADHOC_BOUNDS_COL_NAME)
        {
            if (index.row() == 0)
            {
                return connection_->endpoint1()->name();
            }
            else
            {
                return connection_->endpoint2()->name();
            }
        }

        else if (index.column() == AdHocBoundColumns::ADHOC_BOUNDS_COL_LEFT)
        {
            QString value = connection_->getAdHocLeftBound(index.row());

            if (value.isEmpty())
            {
                if (index.row() == 0)
                {
                    return connection_->endpoint1()->getPort()->getLeftBound();
                }
                else
                {
                    return connection_->endpoint2()->getPort()->getLeftBound();
                }
            }

            return value;
        }

        else if (index.column() == AdHocBoundColumns::ADHOC_BOUNDS_COL_RIGHT)
        {
            QString value = connection_->getAdHocRightBound(index.row());

            if (value.isEmpty())
            {
                if (index.row() == 0)
                {
                    return connection_->endpoint1()->getPort()->getRightBound();
                }
                else
                {
                    return connection_->endpoint2()->getPort()->getRightBound();
                }
            }

            return value;
        }

        else
        {
            return QVariant();
        }
    }
    else if (role == AdHocBoundColumns::UpperPortBoundRole)
    {
        if (index.column() == AdHocBoundColumns::ADHOC_BOUNDS_COL_LEFT || 
            index.column() == AdHocBoundColumns::ADHOC_BOUNDS_COL_RIGHT)
        {
            if (index.row() == 0)
            {
                return connection_->endpoint1()->getPort()->getLeftBound();
            }
            else
            {
                return connection_->endpoint2()->getPort()->getLeftBound();
            }
        }
    }
    else if (role == AdHocBoundColumns::LowerPortBoundRole)
    {
        if (index.column() == AdHocBoundColumns::ADHOC_BOUNDS_COL_LEFT || 
            index.column() == AdHocBoundColumns::ADHOC_BOUNDS_COL_RIGHT)
        {
            if (index.row() == 0)
            {
                return connection_->endpoint1()->getPort()->getRightBound();
            }
            else
            {
                return connection_->endpoint2()->getPort()->getRightBound();
            }
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: AdHocBoundsModel::headerData()
//-----------------------------------------------------------------------------
QVariant AdHocBoundsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
    {
        return QVariant();
    }

    if (orientation == Qt::Horizontal)
    {
        if (section == AdHocBoundColumns::ADHOC_BOUNDS_COL_NAME)
        {
            return tr("Port");
        }
        else if (section == AdHocBoundColumns::ADHOC_BOUNDS_COL_LEFT)
        {
            return tr("Left Bound");
        }
        else if (section == AdHocBoundColumns::ADHOC_BOUNDS_COL_RIGHT)
        {
            return tr("Right Bound");
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

//-----------------------------------------------------------------------------
// Function: AdHocBoundsModel::setData()
//-----------------------------------------------------------------------------
bool AdHocBoundsModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    // Check for invalid index.
    if (!index.isValid() || connection_ == 0 || index.row() < 0 || index.row() >= 2)
    {
        return false;
    }

    if (role == Qt::EditRole)
    {
        if (index.column() == AdHocBoundColumns::ADHOC_BOUNDS_COL_LEFT)
        {
            QSharedPointer<QUndoCommand> cmd(new AdHocBoundsChangeCommand(connection_, false, index.row(),
                connection_->getAdHocLeftBound(index.row()), value.toString()));
            editProvider_->addCommand(cmd);
            cmd->redo();
            return true;
        }

        else if (index.column() == AdHocBoundColumns::ADHOC_BOUNDS_COL_RIGHT)
        {
            QSharedPointer<QUndoCommand> cmd(new AdHocBoundsChangeCommand(connection_, true, index.row(),
                connection_->getAdHocRightBound(index.row()), value.toString()));
            editProvider_->addCommand(cmd);
            cmd->redo();
            return true;
        }

        else
        {
            return false;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AdHocBoundsModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags AdHocBoundsModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}
