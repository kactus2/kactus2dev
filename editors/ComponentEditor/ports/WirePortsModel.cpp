//-----------------------------------------------------------------------------
// File: WirePortsModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 06.05.2019
//
// Description:
// Table model for wire ports.
//-----------------------------------------------------------------------------

#include "WirePortsModel.h"

#include <editors/ComponentEditor/ports/WirePortColumns.h>

#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/validators/PortValidator.h>

//-----------------------------------------------------------------------------
// Function: WirePortsModel::WirePortsModel()
//-----------------------------------------------------------------------------
WirePortsModel::WirePortsModel(QSharedPointer<Model> model, QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<PortValidator> portValidator, QSortFilterProxyModel* filter, QObject *parent):
PortsModel(model, expressionParser, parameterFinder, expressionFormatter, portValidator, filter, parent)
{

}

//-----------------------------------------------------------------------------
// Function: WirePortsModel::columnCount()
//-----------------------------------------------------------------------------
int WirePortsModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
    }

 	return WirePortColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: WirePortsModel::headerData()
//-----------------------------------------------------------------------------
QVariant WirePortsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        if (section == WirePortColumns::DIRECTION)
        {
            return tr("Direction");
        }
        else if (section == WirePortColumns::WIDTH)
        {
            return tr("Width");
        }
        else if (section == WirePortColumns::LEFT_BOUND)
        {
            QString leftBoundHeader = tr("Left (higher)\nbound") + getExpressionSymbol();
            return leftBoundHeader;
        }
        else if (section == WirePortColumns::RIGHT_BOUND)
        {
            QString rightBoundHeader = tr("Right (lower)\nbound") + getExpressionSymbol();
            return rightBoundHeader;
        }
        else if (section == WirePortColumns::DEFAULT_VALUE)
        {
            QString defaultHeader = tr("Default\nvalue") + getExpressionSymbol();
            return defaultHeader;
        }
    }

    return PortsModel::headerData(section, orientation, role);
}

//-----------------------------------------------------------------------------
// Function: WirePortsModel::setData()
//-----------------------------------------------------------------------------
bool WirePortsModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
    if (PortsModel::setData(index, value, role))
    {
        return true;
    }
    else if (!index.isValid() || index.row() < 0 || index.row() >= rowCount())
    {
		return false;
    }

    QSharedPointer<Port> port = portOnRow(index.row());

	if (role == Qt::EditRole)
    {
        if (isLocked(index))
        {
            return false;
        }

        else if (index.column() == WirePortColumns::DIRECTION)
        {
            DirectionTypes::Direction direction = DirectionTypes::str2Direction(value.toString(),
                DirectionTypes::DIRECTION_INVALID);

            port->setDirection(direction);
        }
        else if (index.column() == WirePortColumns::WIDTH)
        {
            if (hasExpressionInLeftOrRightBound(port))
            {
                return false;
            }

            int size = value.toInt();
            port->setPortSize(size);

            QModelIndex widhtIndex = QAbstractTableModel::index(index.row(), WirePortColumns::WIDTH);
            emit dataChanged(widhtIndex, widhtIndex);

            setTypeNameAndDefinitionOnRow(port, index.row());

            emit dataChanged(index, index);
            return true;
        }
        else if (index.column() == WirePortColumns::LEFT_BOUND)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(port->getLeftBound());
            }

            port->setLeftBound(value.toString());

            setTypeNameAndDefinitionOnRow(port, index.row());

            emit dataChanged(index, index);
            return true;
        }
        else if (index.column() == WirePortColumns::RIGHT_BOUND)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(port->getRightBound());
            }

            port->setRightBound(value.toString());

            setTypeNameAndDefinitionOnRow(port, index.row());

            emit dataChanged(index, index);
            return true;
        }
        else if (index.column() == WirePortColumns::DEFAULT_VALUE)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(port->getDefaultValue());
            }

            port->setDefaultValue(value.toString());
        }
        else
        {
            return false;
        }

        emit dataChanged(index, index);
        return true;
    }
    // unsupported role
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: WirePortsModel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant WirePortsModel::valueForIndex(QModelIndex const& index) const
{
    QSharedPointer<Port> port = portOnRow(index.row());
    QSharedPointer<Wire> portWire = port->getWire();

    if (portWire)
    {
        if (index.column() == WirePortColumns::DIRECTION)
        {
            return DirectionTypes::direction2Str(port->getDirection());
        }
        else if (index.column() == WirePortColumns::WIDTH)
        {
            int calculatedLeftBound = parseExpressionToDecimal(port->getLeftBound()).toInt();
            int calculatedRightBound = parseExpressionToDecimal(port->getRightBound()).toInt();

            int portWidth = abs(calculatedLeftBound - calculatedRightBound) + 1;
            return portWidth;
        }
        else if (index.column() == WirePortColumns::LEFT_BOUND)
        {
            QString leftBound = port->getLeftBound();

            if (leftBound == "n/a")
            {
                port->setLeftBound(0);
            }

            return leftBound;
        }
        else if (index.column() == WirePortColumns::RIGHT_BOUND)
        {
            QString rightBound = port->getRightBound();

            if (rightBound == "n/a")
            {
                port->setRightBound(0);
            }

            return rightBound;
        }
        else if (index.column() == WirePortColumns::DEFAULT_VALUE)
        {
            return port->getDefaultValue();
        }
    }
    
    return PortsModel::valueForIndex(index);
}

//-----------------------------------------------------------------------------
// Function: WirePortsModel::isValidExpressionColumn()
//-----------------------------------------------------------------------------
bool WirePortsModel::isValidExpressionColumn(QModelIndex const& index) const
{
    return index.column() == WirePortColumns::LEFT_BOUND || index.column() == WirePortColumns::RIGHT_BOUND ||
        index.column() == WirePortColumns::DEFAULT_VALUE || index.column() == WirePortColumns::ARRAY_LEFT ||
        index.column() == WirePortColumns::ARRAY_RIGHT;
}

//-----------------------------------------------------------------------------
// Function: WirePortsModel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant WirePortsModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    QSharedPointer<Port> port = portOnRow(index.row());

    if (index.column() == WirePortColumns::LEFT_BOUND)
    {
        return port->getLeftBound();
    }
    else if (index.column() == WirePortColumns::RIGHT_BOUND)
    {
        return port->getRightBound();
    }
    else if (index.column() == WirePortColumns::DEFAULT_VALUE)
    {
        return port->getDefaultValue();
    }
    else
    {
        return PortsModel::expressionOrValueForIndex(index);
    }
}

//-----------------------------------------------------------------------------
// Function: WirePortsModel::validateIndex()
//-----------------------------------------------------------------------------
bool WirePortsModel::validateIndex(QModelIndex const& index) const
{
    QSharedPointer<Port> port = portOnRow(index.row());

    if (index.column() == WirePortColumns::LEFT_BOUND)
    {
		if (port->getLeftBound().isEmpty() && port->getRightBound().isEmpty())
		{
			return true;
		}

        return getValidator()->portBoundIsValid(port->getLeftBound());
    }
    else if (index.column() == WirePortColumns::RIGHT_BOUND)
	{
		if (port->getLeftBound().isEmpty() && port->getRightBound().isEmpty())
		{
			return true;
		}

        return getValidator()->portBoundIsValid(port->getRightBound());
    }
    else if (index.column() == WirePortColumns::DEFAULT_VALUE)
    {
        return getValidator()->hasValidDefaultValue(port);
    }
    else
    {
        return PortsModel::validateIndex(index);
    }
}

//-----------------------------------------------------------------------------
// Function: WirePortsModel::getAllReferencesToIdInItemOnRow()
//-----------------------------------------------------------------------------
int WirePortsModel::getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const
{
    QSharedPointer<Port> port = portOnRow(row);

    int referencesInLeftBound = port->getLeftBound().count(valueID);
    int referencesInRightbount = port->getRightBound().count(valueID);
    int referencesInDefaultValue = port->getDefaultValue().count(valueID);

    int  totalReferences = referencesInLeftBound + referencesInRightbount + referencesInDefaultValue +
        PortsModel::getAllReferencesToIdInItemOnRow(row, valueID);
    return totalReferences;
}

//-----------------------------------------------------------------------------
// Function: WirePortsModel::updatePortType()
//-----------------------------------------------------------------------------
void WirePortsModel::setTypeNameAndDefinitionOnRow(QSharedPointer<Port> port, int row)
{
    QModelIndex index = QAbstractTableModel::index(row, typeColumn());

    int calculatedLeftBound = parseExpressionToDecimal(port->getLeftBound()).toInt();
    int calculatedRightBound = parseExpressionToDecimal(port->getRightBound()).toInt();

    int portWidth = abs(calculatedLeftBound - calculatedRightBound) + 1;
    // if port is vectored and previous type was std_logic
    if (portWidth > 1 && port->getTypeName() == QString("std_logic")) 
    {
        // change the type to vectored
        port->setTypeName("std_logic_vector");
        port->setTypeDefinition("std_logic_vector", "IEEE.std_logic_1164.all");
    }
    // if port is not vectored but previous type was std_logic_vector
    else if (portWidth < 2 && port->getTypeName() == QString("std_logic_vector")) 
    {
        port->setTypeName("std_logic");
        port->setTypeDefinition("std_logic", "IEEE.std_logic_1164.all");
    }

    emit dataChanged(index, index);
}

//-----------------------------------------------------------------------------
// Function: WirePortsModel::hasExpressionInLeftOrRightBound()
//-----------------------------------------------------------------------------
bool WirePortsModel::hasExpressionInLeftOrRightBound(QSharedPointer<Port> port) const
{
    QString left = port->getLeftBound();
    QString right = port->getRightBound();

	if (left.isEmpty() && right.isEmpty())
	{
		return false;
	}

    bool leftNumber = false;
    bool rightNumber = false;

    left.toInt(&leftNumber);
    right.toInt(&rightNumber);

    return leftNumber || rightNumber;
}

//-----------------------------------------------------------------------------
// Function: WirePortsModel::rowNumberColumn()
//-----------------------------------------------------------------------------
int WirePortsModel::rowNumberColumn() const
{
    return WirePortColumns::ROW_NUMBER;
}

//-----------------------------------------------------------------------------
// Function: WirePortsModel::nameColumn()
//-----------------------------------------------------------------------------
int WirePortsModel::nameColumn() const
{
    return WirePortColumns::NAME;
}

//-----------------------------------------------------------------------------
// Function: WirePortsModel::typeColumn()
//-----------------------------------------------------------------------------
int WirePortsModel::typeColumn() const
{
    return WirePortColumns::TYPE_NAME;
}

//-----------------------------------------------------------------------------
// Function: WirePortsModel::arrayLeftColumn()
//-----------------------------------------------------------------------------
int WirePortsModel::arrayLeftColumn() const
{
    return WirePortColumns::ARRAY_LEFT;
}

//-----------------------------------------------------------------------------
// Function: WirePortsModel::arrayRightColum()
//-----------------------------------------------------------------------------
int WirePortsModel::arrayRightColum() const
{
    return WirePortColumns::ARRAY_RIGHT;
}

//-----------------------------------------------------------------------------
// Function: WirePortsModel::tagColumn()
//-----------------------------------------------------------------------------
int WirePortsModel::tagColumn() const
{
    return WirePortColumns::TAG_GROUP;
}

//-----------------------------------------------------------------------------
// Function: WirePortsModel::adHocColumn()
//-----------------------------------------------------------------------------
int WirePortsModel::adHocColumn() const
{
    return WirePortColumns::ADHOC_VISIBILITY;
}

//-----------------------------------------------------------------------------
// Function: WirePortsModel::descriptionColumn()
//-----------------------------------------------------------------------------
int WirePortsModel::descriptionColumn() const
{
    return WirePortColumns::DESCRIPTION;
}

//-----------------------------------------------------------------------------
// Function: WirePortsModel::indexedItemIsDisabled()
//-----------------------------------------------------------------------------
bool WirePortsModel::indexedItemIsDisabled(QModelIndex const& index, QSharedPointer<Port> indexedPort) const
{
    return (index.column() == WirePortColumns::ROW_NUMBER ||
        (index.column() == WirePortColumns::DEFAULT_VALUE && (indexedPort->getDirection() != DirectionTypes::IN &&
            indexedPort->getDirection() != DirectionTypes::INOUT)) ||
        (index.column() == WirePortColumns::WIDTH && hasExpressionInLeftOrRightBound(portOnRow(index.row()))));
}

//-----------------------------------------------------------------------------
// Function: WirePortsModel::indexedItemIsMandatory()
//-----------------------------------------------------------------------------
bool WirePortsModel::indexedItemIsMandatory(QModelIndex const& index) const
{
    return index.column() == WirePortColumns::NAME || index.column() == WirePortColumns::DIRECTION ||
        index.column() == WirePortColumns::WIDTH;
}

//-----------------------------------------------------------------------------
// Function: WirePortsModel::indexedItemIsLocked()
//-----------------------------------------------------------------------------
bool WirePortsModel::indexedItemIsLocked(QModelIndex const& index) const
{
    return index.column() == WirePortColumns::WIDTH && hasExpressionInLeftOrRightBound(portOnRow(index.row()));
}

//-----------------------------------------------------------------------------
// Function: WirePortsModel::getLockedPortIndexes()
//-----------------------------------------------------------------------------
QModelIndexList WirePortsModel::getLockedPortIndexes(QModelIndex const& portIndex) const
{
    QModelIndexList lockedIndexes;

    QModelIndex nameIndex = portIndex.sibling(portIndex.row(), WirePortColumns::NAME);
    QModelIndex directionIndex = portIndex.sibling(portIndex.row(), WirePortColumns::DIRECTION);
    QModelIndex typeIndex = portIndex.sibling(portIndex.row(), WirePortColumns::TYPE_NAME);

    lockedIndexes.append(nameIndex);
    lockedIndexes.append(directionIndex);
    lockedIndexes.append(typeIndex);

    return lockedIndexes;
}

//-----------------------------------------------------------------------------
// Function: WirePortsModel::finalizePort()
//-----------------------------------------------------------------------------
void WirePortsModel::finalizePort(QSharedPointer<Port> port)
{
    QSharedPointer<Wire> newWire(new Wire());
    newWire->setDirection(DirectionTypes::IN);
    port->setWire(newWire);
}
