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

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <IPXACTmodels/common/TransactionalTypes.h>
#include <IPXACTmodels/common/DirectionTypes.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Port.h>

#include <QIcon>

//-----------------------------------------------------------------------------
// Function: PhysicalPortMappingTableModel::PhysicalPortMappingTableModel()
//-----------------------------------------------------------------------------
PhysicalPortMappingTableModel::PhysicalPortMappingTableModel(QSharedPointer<Component> component,
    QSharedPointer<ExpressionParser> componentParser, QObject *parent):
PortMappingTableModel(parent),
    component_(component),
    ports_(),
    expressionParser_(componentParser)
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

    ports_.clear();

    foreach (QSharedPointer<Port> port, *component_->getPorts())
    {
        if (!ports_.contains(port))
        {
            ports_.append(port);
        }
    }

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
        return ports_.size();
    }
}

//-----------------------------------------------------------------------------
// Function: PhysicalPortMappingTableModel::data()
//-----------------------------------------------------------------------------
QVariant PhysicalPortMappingTableModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= ports_.size())
    {
        return QVariant();
    }

    QSharedPointer<Port> selectedPort = ports_.at(index.row());
    QSharedPointer<Transactional> portTransactional = selectedPort->getTransactional();

    if (role == Qt::DisplayRole)
    {
        if (index.column() == PortMappingColumns::NAME)
        {
            return selectedPort->name();
        }
        else if (index.column() == PortMappingColumns::DIRECTION)
        {
            return DirectionTypes::direction2Str(selectedPort->getDirection());
        }
        else if (index.column() == PortMappingColumns::INITIATIVE && portTransactional)
        {
            QString transactionalInitiative = portTransactional->getInitiative();
            if (transactionalInitiative == TransactionalTypes::INITIATIVE_BOTH)
            {
                transactionalInitiative = TransactionalTypes::INITIATIVE_REQUIRES_PROVIDES;
            }

            return transactionalInitiative;
        }
        else if (index.column() == PortMappingColumns::LEFT_BOUND)
        {
            QString leftExpression = expressionParser_->parseExpression(selectedPort->getLeftBound());
            if (leftExpression.compare(QLatin1String("x")) == 0)
            {
                return QString();
            }

            return leftExpression;
        }
        else if (index.column() == PortMappingColumns::RIGHT_BOUND)
        {
            QString rightExpression = expressionParser_->parseExpression(selectedPort->getRightBound());
            if (rightExpression.compare(QLatin1String("x")) == 0)
            {
                return QString();
            }

            return rightExpression;
        }
        else if (index.column() == PortMappingColumns::SIZE)
        {
            if (portTransactional)
            {
                return expressionParser_->parseExpression(portTransactional->getBusWidth());
            }
            else
            {
                return getPortSize(selectedPort);
            }
        }
    }
    else if (role == Qt::DecorationRole)
    {
        if (index.column() == PortMappingColumns::DIRECTION)
        {
            DirectionTypes::Direction direction = selectedPort->getDirection();
            if(direction == DirectionTypes::IN)
            {
                return QIcon(":icons/common/graphics/input.png");
            }
            else if (direction == DirectionTypes::OUT)
            {
                return QIcon(":icons/common/graphics/output.png");
            }
            else if (direction == DirectionTypes::INOUT)
            {
                return QIcon(":icons/common/graphics/inout.png");
            }
            else if (direction == DirectionTypes::DIRECTION_PHANTOM)
            {
                return QIcon(":icons/common/graphics/phantom.png");
            }
            else
            {
                return QIcon(":icons/common/graphics/cross.png");
            }
        }
        else if (index.column() == PortMappingColumns::INITIATIVE && portTransactional)
        {
            if (portTransactional->getInitiative().compare(
                TransactionalTypes::INITIATIVE_PROVIDES, Qt::CaseInsensitive) == 0)
            {
                return QIcon(":icons/common/graphics/provides.png");
            }
            else if (portTransactional->getInitiative().compare(
                TransactionalTypes::INITIATIVE_REQUIRES, Qt::CaseInsensitive) == 0)
            {
                return QIcon(":icons/common/graphics/requires.png");
            }
            else if (portTransactional->getInitiative().compare(
                TransactionalTypes::INITIATIVE_BOTH, Qt::CaseInsensitive) == 0)
            {
                return QIcon(":icons/common/graphics/requires_provides.png");
            }
            else if (portTransactional->getInitiative().compare(
                TransactionalTypes::INITIATIVE_PHANTOM, Qt::CaseInsensitive) == 0)
            {
                return QIcon(":icons/common/graphics/phantom.png");
            }
        }
    }
    
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: PhysicalPortMappingTableModel::getPortSize()
//-----------------------------------------------------------------------------
QVariant PhysicalPortMappingTableModel::getPortSize(QSharedPointer<Port> selectedPort) const
{
    int portLeftBound = expressionParser_->parseExpression(selectedPort->getLeftBound()).toInt();
    int portRightBound = expressionParser_->parseExpression(selectedPort->getRightBound()).toInt();

    int portSize = abs(portLeftBound - portRightBound) + 1;
    return portSize;
}

//-----------------------------------------------------------------------------
// Function: PhysicalPortMappingTableModel::addPort()
//-----------------------------------------------------------------------------
void PhysicalPortMappingTableModel::addPort(QString const& portName)
{
    foreach (QSharedPointer<Port> port, ports_)
    {
        if (port->name().compare(portName) == 0)
        {
            return;
        }
    }

    QSharedPointer<Port> newPort = component_->getPort(portName);
    if (newPort)
    {
        beginResetModel();

        ports_.append(newPort);

        endResetModel();
    }
}
