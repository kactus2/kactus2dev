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

#include "AdHocDelegate.h"

#include <models/port.h>
#include <models/component.h>

#include <designwidget/diagramcomponent.h>
#include <designwidget/blockdiagram.h>
#include <designwidget/designwidget.h>
#include <designwidget/DiagramChangeCommands.h>
#include <designwidget/AdHocEnabled.h>

#include <EndpointDesign/EndpointDesignDiagram.h>
#include <EndpointDesign/EndpointDesignWidget.h>

#include <common/GenericEditProvider.h>

//-----------------------------------------------------------------------------
// Function: AdHocModel::AdHocModel()
//-----------------------------------------------------------------------------
AdHocModel::AdHocModel(QObject *parent) : QAbstractTableModel(parent),
                                          dataSource_(0), table_()
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
void AdHocModel::setDataSource(AdHocEnabled* dataSource)
{
    dataSource_ = dataSource;

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
    if (parent.isValid())
    {
        return 0;
    }

    return table_.size();
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

    return ADHOC_COL_COUNT;
}

//-----------------------------------------------------------------------------
// Function: AdHocModel::data()
//-----------------------------------------------------------------------------
QVariant AdHocModel::data(QModelIndex const& index, int role /*= Qt::DisplayRole*/) const
{
    // Check for invalid index.
    if (!index.isValid() || index.row() < 0 || index.row() >= table_.size())
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case ADHOC_COL_NAME:
            {
                return table_.at(index.row())->getName();
            }

        case ADHOC_COL_DIRECTION:
            {
                return General::direction2Str(table_.at(index.row())->getDirection());
            }

        default:
            {
                return QVariant();
            }
        }
    }
    else if (Qt::CheckStateRole == role)
    {
        if (index.column() == ADHOC_COL_VISIBILITY)
        {
            if (dataSource_->isPortAdHocVisible(table_.at(index.row())->getName()))
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
            switch (section)
            {
            case ADHOC_COL_NAME:
                {
                    return tr("Name");
                }

            case ADHOC_COL_DIRECTION:
                {
                    return tr("Direction");
                }

            case ADHOC_COL_VISIBILITY:
                {
                    return tr("Ad-hoc");
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
// Function: AdHocModel::setData()
//-----------------------------------------------------------------------------
bool AdHocModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
{
    // Check for invalid index.
    if (!index.isValid() || index.row() < 0 || index.row() >= table_.size())
    {
        return false;
    }

    if (role == Qt::CheckStateRole)
    {
        QSharedPointer<QUndoCommand> cmd(new AdHocVisibilityChangeCommand(dataSource_,
                                                                          table_.at(index.row())->getName(),
                                                                          value == Qt::Checked));
        dataSource_->getEditProvider().addCommand(cmd);
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

    if (index.column() == ADHOC_COL_VISIBILITY)
    {
        flags |= Qt::ItemIsUserCheckable | Qt::ItemIsSelectable;
    }

    return flags;
}

//-----------------------------------------------------------------------------
// Function: AdHocModel::updateVisibilities()
//-----------------------------------------------------------------------------
void AdHocModel::updateVisibilities()
{
    emit dataChanged(index(0, ADHOC_COL_VISIBILITY), index(table_.size() - 1, ADHOC_COL_VISIBILITY));
}
