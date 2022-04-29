//-----------------------------------------------------------------------------
// File: PhysicalPortMappingTableModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 19.05.2016
//
// Description:
// Model to display the physical ports of a component in port mapping.
//-----------------------------------------------------------------------------

#include "PhysicalPortMappingTableModel.h"
#include "PortMappingColumns.h"

#include <KactusAPI/include/ExpressionParser.h>
#include <KactusAPI/include/PortsInterface.h>

#include <IPXACTmodels/common/TransactionalTypes.h>
#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Port.h>

#include <QIcon>

//-----------------------------------------------------------------------------
// Function: PhysicalPortMappingTableModel::PhysicalPortMappingTableModel()
//-----------------------------------------------------------------------------
PhysicalPortMappingTableModel::PhysicalPortMappingTableModel(PortsInterface* portInterface, QObject *parent):
PortMappingTableModel(parent),
portInterface_(portInterface)
{
    refresh();
}

//-----------------------------------------------------------------------------
// Function: PhysicalPortMappingTableModel::~PhysicalPortMappingTableModel()
//-----------------------------------------------------------------------------
PhysicalPortMappingTableModel::~PhysicalPortMappingTableModel()
{

}

//-----------------------------------------------------------------------------
// Function: PhysicalPortMappingTableModel::refresh()
//-----------------------------------------------------------------------------
void PhysicalPortMappingTableModel::refresh()
{
    beginResetModel();
    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: PhysicalPortMappingTableModel::rowCount()
//-----------------------------------------------------------------------------
int PhysicalPortMappingTableModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    else
    {
        return portInterface_->itemCount();
    }
}

//-----------------------------------------------------------------------------
// Function: PhysicalPortMappingTableModel::data()
//-----------------------------------------------------------------------------
QVariant PhysicalPortMappingTableModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= portInterface_->itemCount())
    {
        return QVariant();
    }

    std::string portName = portInterface_->getIndexedItemName(index.row());

    if (role == Qt::DisplayRole)
    {
        if (index.column() == PortMappingColumns::NAME)
        {
            return QString::fromStdString(portName);
        }
        else if (index.column() == PortMappingColumns::DIRECTION)
        {
            QString portDirection = QString::fromStdString(portInterface_->getDirection(portName));
            return portDirection;
        }
        else if (index.column() == PortMappingColumns::INITIATIVE && portInterface_->portIsTransactional(portName))
        {
            QString transactionalInitiative = QString::fromStdString(portInterface_->getInitiative(portName));
            if (transactionalInitiative == TransactionalTypes::INITIATIVE_BOTH)
            {
                transactionalInitiative = TransactionalTypes::INITIATIVE_REQUIRES_PROVIDES;
            }

            return transactionalInitiative;
        }
        else if (index.column() == PortMappingColumns::LEFT_BOUND)
        {
            QString leftExpression = QString::fromStdString(portInterface_->getLeftBoundValue(portName));
            if (leftExpression.compare(QLatin1String("x")) == 0)
            {
                return QString();
            }

            return leftExpression;
        }
        else if (index.column() == PortMappingColumns::RIGHT_BOUND)
        {
            QString rightExpression = QString::fromStdString(portInterface_->getRightBoundValue(portName));
            if (rightExpression.compare(QLatin1String("x")) == 0)
            {
                return QString();
            }

            return rightExpression;
        }
        else if (index.column() == PortMappingColumns::SIZE)
        {
            if (portInterface_->portIsTransactional(portName))
            {
                return QString::fromStdString(portInterface_->getBusWidthValue(portName));
            }
            else
            {
                return QString::fromStdString(portInterface_->getWidth(portName));
            }
        }
    }
    else if (role == Qt::DecorationRole && (index.column() == PortMappingColumns::DIRECTION ||
        index.column() == PortMappingColumns::INITIATIVE))
    {
        return QIcon(QString::fromStdString(portInterface_->getIconPathForPort(portName)));
    }
    
    return QVariant();
}
