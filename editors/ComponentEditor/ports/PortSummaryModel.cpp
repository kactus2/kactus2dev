//-----------------------------------------------------------------------------
// File: PortSummaryModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 06.05.2019
//
// Description:
// Table model for wire ports.
//-----------------------------------------------------------------------------

#include "PortSummaryModel.h"

#include "PortSummaryColumns.h"

#include <KactusAPI/include/PortsInterface.h>

#include <IPXACTmodels/common/DirectionTypes.h>

#include <QIcon>

//-----------------------------------------------------------------------------
// Function: PortSummaryModel::PortSummaryModel()
//-----------------------------------------------------------------------------
PortSummaryModel::PortSummaryModel(QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<PortsInterface> portsInterface, QSharedPointer<PortAbstractionInterface> signalInterface,
    QObject *parent):
PortsModel(parameterFinder, portsInterface, signalInterface, parent)
{

}

//-----------------------------------------------------------------------------
// Function: PortSummaryModel::columnCount()
//-----------------------------------------------------------------------------
int PortSummaryModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
    }

 	return PortSummaryColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: PortSummaryModel::headerData()
//-----------------------------------------------------------------------------
QVariant PortSummaryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && section == PortSummaryColumns::TYPE && role == Qt::DisplayRole)
    {
        return tr("Type");
    }

    return PortsModel::headerData(section, orientation, role);
}

//-----------------------------------------------------------------------------
// Function: PortSummaryModel::data()
//-----------------------------------------------------------------------------
QVariant PortSummaryModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= getInterface()->itemCount())
    {
        return QVariant();
    }

    return PortsModel::data(index, role);
}

//-----------------------------------------------------------------------------
// Function: PortSummaryModel::setData()
//-----------------------------------------------------------------------------
bool PortSummaryModel::setData(QModelIndex const& index, QVariant const& value, int role)
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

    }
    // unsupported role
    else
    {
        return false;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortSummaryModel::onAddWire()
//-----------------------------------------------------------------------------
void PortSummaryModel::onAddWire()
{
    auto const row = rowCount();

    beginInsertRows(QModelIndex(), row, row);
    getInterface()->addWirePort();
    endInsertRows();

    emit invalidateOtherFilter();
    emit portCountChanged();

    // tell also parent widget that contents have been changed
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: PortSummaryModel::onAddTransactional()
//-----------------------------------------------------------------------------
void PortSummaryModel::onAddTransactional()
{
    auto const row = rowCount();

    beginInsertRows(QModelIndex(), row, row);
    getInterface()->addTransactionalPort();
    endInsertRows();

    emit invalidateOtherFilter();
    emit portCountChanged();

    // tell also parent widget that contents have been changed
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: PortSummaryModel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant PortSummaryModel::valueForIndex(QModelIndex const& index) const
{
    if (index.column() == PortSummaryColumns::TYPE)
    {
        const auto ports = getInterface();
        std::string portName = ports->getIndexedItemName(index.row());

        if (ports->portIsWire(portName))
        {
            return tr("wire");
        }
        else if (ports->portIsTransactional(portName))
        {
            return tr("transactional");
        }
        else
        {
            return tr("unknown");
        }
       
    }
    
    return PortsModel::valueForIndex(index);
}

//-----------------------------------------------------------------------------
// Function: PortSummaryModel::isValidExpressionColumn()
//-----------------------------------------------------------------------------
bool PortSummaryModel::isValidExpressionColumn(QModelIndex const& /*index*/) const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: PortSummaryModel::isPortTypeColumn()
//-----------------------------------------------------------------------------
bool PortSummaryModel::isPortTypeColumn(QModelIndex const& index) const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: PortSummaryModel::formattedExpressionForIndex()
//-----------------------------------------------------------------------------
QVariant PortSummaryModel::formattedExpressionForIndex(QModelIndex const& index) const
{
  
        return PortsModel::formattedExpressionForIndex(index);
    
}

//-----------------------------------------------------------------------------
// Function: PortSummaryModel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant PortSummaryModel::expressionOrValueForIndex(QModelIndex const& index) const
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
// Function: PortSummaryModel::rowNumberColumn()
//-----------------------------------------------------------------------------
int PortSummaryModel::rowNumberColumn() const
{
    return PortSummaryColumns::ROW_NUMBER;
}

//-----------------------------------------------------------------------------
// Function: PortSummaryModel::nameColumn()
//-----------------------------------------------------------------------------
int PortSummaryModel::nameColumn() const
{
    return PortSummaryColumns::NAME;
}

//-----------------------------------------------------------------------------
// Function: PortSummaryModel::typeColumn()
//-----------------------------------------------------------------------------
int PortSummaryModel::typeColumn() const
{
    return PortSummaryColumns::TYPE_NAME;
}

//-----------------------------------------------------------------------------
// Function: PortSummaryModel::arrayLeftColumn()
//-----------------------------------------------------------------------------
int PortSummaryModel::arrayLeftColumn() const
{
    return PortSummaryColumns::ARRAY_LEFT;
}

//-----------------------------------------------------------------------------
// Function: PortSummaryModel::arrayRightColum()
//-----------------------------------------------------------------------------
int PortSummaryModel::arrayRightColum() const
{
    return PortSummaryColumns::ARRAY_RIGHT;
}

//-----------------------------------------------------------------------------
// Function: PortSummaryModel::tagColumn()
//-----------------------------------------------------------------------------
int PortSummaryModel::tagColumn() const
{
    return PortSummaryColumns::TAG_GROUP;
}

//-----------------------------------------------------------------------------
// Function: PortSummaryModel::adHocColumn()
//-----------------------------------------------------------------------------
int PortSummaryModel::adHocColumn() const
{
    return PortSummaryColumns::ADHOC_VISIBILITY;
}

//-----------------------------------------------------------------------------
// Function: PortSummaryModel::descriptionColumn()
//-----------------------------------------------------------------------------
int PortSummaryModel::descriptionColumn() const
{
    return PortSummaryColumns::DESCRIPTION;
}

//-----------------------------------------------------------------------------
// Function: PortSummaryModel::indexedItemIsDisabled()
//-----------------------------------------------------------------------------
bool PortSummaryModel::indexedItemIsDisabled(QModelIndex const& index) const
{
    return index.column() == PortSummaryColumns::ROW_NUMBER || 
        index.column() == PortSummaryColumns::TYPE;
 }

//-----------------------------------------------------------------------------
// Function: PortSummaryModel::indexedItemIsMandatory()
//-----------------------------------------------------------------------------
bool PortSummaryModel::indexedItemIsMandatory(QModelIndex const& index) const
{
    return index.column() == PortSummaryColumns::NAME;
}

//-----------------------------------------------------------------------------
// Function: PortSummaryModel::indexedItemIsLocked()
//-----------------------------------------------------------------------------
bool PortSummaryModel::indexedItemIsLocked(QModelIndex const& index) const
{
    return index.column() == PortSummaryColumns::ROW_NUMBER || 
        index.column() == PortSummaryColumns::TYPE;
}

//-----------------------------------------------------------------------------
// Function: PortSummaryModel::getLockedPortIndexes()
//-----------------------------------------------------------------------------
QModelIndexList PortSummaryModel::getLockedPortIndexes(QModelIndex const& /*portIndex*/) const
{
    return QModelIndexList();
}

//-----------------------------------------------------------------------------
// Function: PortSummaryModel::addNewPort()
//-----------------------------------------------------------------------------
void PortSummaryModel::addNewPort()
{

}

