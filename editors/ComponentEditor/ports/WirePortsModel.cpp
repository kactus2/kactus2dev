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

#include <KactusAPI/include/PortsInterface.h>
#include <editors/ComponentEditor/ports/WirePortColumns.h>

#include <IPXACTmodels/common/DirectionTypes.h>

#include <QIcon>

using namespace std;

//-----------------------------------------------------------------------------
// Function: WirePortsModel::WirePortsModel()
//-----------------------------------------------------------------------------
WirePortsModel::WirePortsModel(QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<PortsInterface> portsInterface, QSharedPointer<PortAbstractionInterface> signalInterface,
    QSortFilterProxyModel* filter, QObject *parent):
PortsModel(parameterFinder, portsInterface, signalInterface, filter, parent)
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
// Function: WirePortsModel::data()
//-----------------------------------------------------------------------------
QVariant WirePortsModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= getInterface()->itemCount())
    {
        return QVariant();
    }

    if (role == Qt::DecorationRole && index.column() == WirePortColumns::DIRECTION)
    {
        return QIcon(getIconPath(index.row()));
    }
    
    return PortsModel::data(index, role);
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

    std::string portName = getInterface()->getIndexedItemName(index.row());

	if (role == Qt::EditRole)
    {
        if (isLocked(index))
        {
            return false;
        }

        else if (index.column() == WirePortColumns::DIRECTION)
        {
            getInterface()->setDirection(portName, value.toString().toStdString());
        }
        else if (index.column() == WirePortColumns::WIDTH)
        {
            bool success = getInterface()->setWidth(portName, value.toString().toStdString());
            if (success == false)
            {
                return false;
            }
        }
        else if (index.column() == WirePortColumns::LEFT_BOUND)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(
                    QString::fromStdString(getInterface()->getLeftBoundExpression(portName)));
            }

            getInterface()->setLeftBound(portName, value.toString().toStdString());

        }
        else if (index.column() == WirePortColumns::RIGHT_BOUND)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(
                    QString::fromStdString(getInterface()->getRightBoundExpression(portName)));
            }

            getInterface()->setRightBound(portName, value.toString().toStdString());
        }
        else if (index.column() == WirePortColumns::DEFAULT_VALUE)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(
                    QString::fromStdString(getInterface()->getDefaultValueExpression(portName)));
            }

            getInterface()->setDefaultValue(portName, value.toString().toStdString());
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
    string portName = getInterface()->getIndexedItemName(index.row());
    if (index.column() == WirePortColumns::DIRECTION)
    {
        return QString::fromStdString(getInterface()->getDirection(portName));
    }
    else if (index.column() == WirePortColumns::WIDTH)
    {
        return QString::fromStdString(getInterface()->getWidth(portName));
    }
    else if (index.column() == WirePortColumns::LEFT_BOUND)
    {
        return QString::fromStdString(getInterface()->getLeftBoundValue(portName));
    }
    else if (index.column() == WirePortColumns::RIGHT_BOUND)
    {
        return QString::fromStdString(getInterface()->getRightBoundValue(portName));
    }
    else if (index.column() == WirePortColumns::DEFAULT_VALUE)
    {
        return QString::fromStdString(getInterface()->getDefaultValue(portName));
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
// Function: WirePortsModel::formattedExpressionForIndex()
//-----------------------------------------------------------------------------
QVariant WirePortsModel::formattedExpressionForIndex(QModelIndex const& index) const
{
    string portName = getInterface()->getIndexedItemName(index.row());

    if (index.column() == WirePortColumns::LEFT_BOUND)
    {
        return QString::fromStdString(getInterface()->getLeftBoundFormattedExpression(portName));
    }
    else if (index.column() == WirePortColumns::RIGHT_BOUND)
    {
        return QString::fromStdString(getInterface()->getRightBoundFormattedExpression(portName));
    }
    else if (index.column() == WirePortColumns::DEFAULT_VALUE)
    {
        return QString::fromStdString(getInterface()->getDefaultValueFormattedExpression(portName));
    }
    else
    {
        return PortsModel::formattedExpressionForIndex(index);
    }
}

//-----------------------------------------------------------------------------
// Function: WirePortsModel::expressionForIndex()
//-----------------------------------------------------------------------------
QVariant WirePortsModel::expressionForIndex(QModelIndex const& index) const
{
    std::string portName = getInterface()->getIndexedItemName(index.row());

    if (index.column() == WirePortColumns::LEFT_BOUND)
    {
        return QString::fromStdString(getInterface()->getLeftBoundExpression(portName));
    }
    else if (index.column() == WirePortColumns::RIGHT_BOUND)
    {
        return QString::fromStdString(getInterface()->getRightBoundExpression(portName));
    }
    else if (index.column() == WirePortColumns::DEFAULT_VALUE)
    {
        return QString::fromStdString(getInterface()->getDefaultValueExpression(portName));
    }
    else
    {
        return PortsModel::expressionForIndex(index);
    }
}

//-----------------------------------------------------------------------------
// Function: WirePortsModel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant WirePortsModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    if (isValidExpressionColumn(index))
    {
        return expressionForIndex(index);
    }
    else
    {
        return valueForIndex(index);
    }
}

//-----------------------------------------------------------------------------
// Function: WirePortsModel::validateIndex()
//-----------------------------------------------------------------------------
bool WirePortsModel::validateIndex(QModelIndex const& index) const
{
    std::string portName = getInterface()->getIndexedItemName(index.row());

    if (index.column() == WirePortColumns::LEFT_BOUND)
    {
        return getInterface()->portHasValidLeftBound(portName);
    }
    else if (index.column() == WirePortColumns::RIGHT_BOUND)
	{
        return getInterface()->portHasValidRightBound(portName);
    }
    else if (index.column() == WirePortColumns::DEFAULT_VALUE)
    {
        return getInterface()->portHasValidDefaultValue(portName);
    }
    else
    {
        return PortsModel::validateIndex(index);
    }
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
bool WirePortsModel::indexedItemIsDisabled(QModelIndex const& index) const
{
    std::string portName = getInterface()->getIndexedItemName(index.row());

    return (index.column() == WirePortColumns::ROW_NUMBER ||
        (index.column() == WirePortColumns::DEFAULT_VALUE &&
        (QString::fromStdString(getInterface()->getDirection(portName)) != DirectionTypes::IN &&
        QString::fromStdString(getInterface()->getDirection(portName)) != DirectionTypes::INOUT)) ||
        (index.column() == WirePortColumns::WIDTH && getInterface()->hasExpressionInLeftOrRightBound(portName)));
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
    return index.column() == WirePortColumns::WIDTH &&
        getInterface()->hasExpressionInLeftOrRightBound(getInterface()->getIndexedItemName(index.row()));
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
// Function: WirePortsModel::addNewPort()
//-----------------------------------------------------------------------------
void WirePortsModel::addNewPort()
{
    getInterface()->addWirePort();
}
