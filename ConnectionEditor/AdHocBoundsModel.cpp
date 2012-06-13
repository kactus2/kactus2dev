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

#include "AdHocBoundsDelegate.h"

#include <designwidget/diagraminterconnection.h>
#include <designwidget/DiagramConnectionEndpoint.h>
#include <designwidget/blockdiagram.h>
#include <designwidget/DiagramChangeCommands.h>

#include <common/GenericEditProvider.h>

//-----------------------------------------------------------------------------
// Function: AdHocBoundsModel::AdHocBoundsModel()
//-----------------------------------------------------------------------------
AdHocBoundsModel::AdHocBoundsModel(QObject *parent) : QAbstractTableModel(parent), connection_(0)
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
void AdHocBoundsModel::setConnection(DiagramInterconnection* connection)
{
    connection_ = connection;
    beginResetModel();
    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: AdHocBoundsModel::rowCount()
//-----------------------------------------------------------------------------
int AdHocBoundsModel::rowCount(QModelIndex const& parent /*= QModelIndex()*/) const
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
int AdHocBoundsModel::columnCount(QModelIndex const& parent /*= QModelIndex()*/) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return ADHOC_BOUNDS_COL_COUNT;
}

//-----------------------------------------------------------------------------
// Function: AdHocBoundsModel::data()
//-----------------------------------------------------------------------------
QVariant AdHocBoundsModel::data(QModelIndex const& index, int role /*= Qt::DisplayRole*/) const
{
    // Check for invalid index.
    if (!index.isValid() || connection_ == 0 || index.row() < 0 || index.row() >= 2)
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case ADHOC_BOUNDS_COL_NAME:
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

        case ADHOC_BOUNDS_COL_LEFT:
            {
                int value = connection_->getAdHocLeftBound(index.row());

                if (value < 0)
                {
                    if (index.row() == 0)
                    {
                        value = connection_->endpoint1()->getPort()->getLeftBound();
                    }
                    else
                    {
                        value = connection_->endpoint2()->getPort()->getLeftBound();
                    }
                }

                return value;
            }

        case ADHOC_BOUNDS_COL_RIGHT:
            {
                int value = connection_->getAdHocRightBound(index.row());

                if (value < 0)
                {
                    if (index.row() == 0)
                    {
                        value = connection_->endpoint1()->getPort()->getRightBound();
                    }
                    else
                    {
                        value = connection_->endpoint2()->getPort()->getRightBound();
                    }
                }

                return value;
            }

        default:
            {
                return QVariant();
            }
        }
    }
    else if (role == UpperPortBoundRole)
    {
        if (index.column() == ADHOC_BOUNDS_COL_LEFT || index.column() == ADHOC_BOUNDS_COL_RIGHT)
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
    else if (role == LowerPortBoundRole)
    {
        if (index.column() == ADHOC_BOUNDS_COL_LEFT || index.column() == ADHOC_BOUNDS_COL_RIGHT)
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
QVariant AdHocBoundsModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            switch (section)
            {
            case ADHOC_BOUNDS_COL_NAME:
                {
                    return tr("Port");
                }

            case ADHOC_BOUNDS_COL_LEFT:
                {
                    return tr("Left Bound");
                }

            case ADHOC_BOUNDS_COL_RIGHT:
                {
                    return tr("Right Bound");
                }

            default:
                {
                    return QVariant();
                }
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
// Function: AdHocBoundsModel::setData()
//-----------------------------------------------------------------------------
bool AdHocBoundsModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
{
    // Check for invalid index.
    if (!index.isValid() || connection_ == 0 || index.row() < 0 || index.row() >= 2)
    {
        return false;
    }
    
    if (role == Qt::EditRole)
    {
        GenericEditProvider& editProvider =
            static_cast<BlockDiagram*>(connection_->scene())->getEditProvider();

        switch (index.column())
        {
        case ADHOC_BOUNDS_COL_LEFT:
            {
                QSharedPointer<QUndoCommand> cmd(new AdHocBoundsChangeCommand(connection_, false, index.row(),
                                                                              connection_->getAdHocLeftBound(index.row()),
                                                                              value.toInt()));
                editProvider.addCommand(cmd);
                return true;
            }

        case ADHOC_BOUNDS_COL_RIGHT:
            {
                QSharedPointer<QUndoCommand> cmd(new AdHocBoundsChangeCommand(connection_, true, index.row(),
                                                                              connection_->getAdHocRightBound(index.row()),
                                                                              value.toInt()));
                editProvider.addCommand(cmd);
                return true;
            }

        default:
            {
                return false;
            }
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

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    return flags;
}
