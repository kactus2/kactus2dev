//-----------------------------------------------------------------------------
// File: parametersmodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 4.4.2011
//
// Description:
// Table model that can be used to display parameters to be edited.
//-----------------------------------------------------------------------------

#include "parametersmodel.h"
#include "ParameterColumns.h"

#include <IPXACTmodels/Component/Choice.h>

#include <editors/ComponentEditor/parameters/ParametersInterface.h>

//-----------------------------------------------------------------------------
// Function: ParametersModel::ParametersModel()
//-----------------------------------------------------------------------------
ParametersModel::ParametersModel(QSharedPointer<ParametersInterface> parameterInterface,
    QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ParameterFinder> parameterFinder, QObject *parent) :
AbstractParameterModel(parameterInterface, expressionParser, parameterFinder, parent)
{

}

//-----------------------------------------------------------------------------
// Function: ParametersModel::columnCount()
//-----------------------------------------------------------------------------
int ParametersModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

	return ParameterColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: parametersmodel::resetModel()
//-----------------------------------------------------------------------------
void ParametersModel::resetModelItems()
{
    beginResetModel();
    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: ParametersModel::onAddItem()
//-----------------------------------------------------------------------------
void ParametersModel::onAddItem(QModelIndex const& index)
{
    int row = getInterface()->itemCount();

    // if the index is valid then add the item to the correct position
    if (index.isValid())
    {
        row = index.row();
    }

    beginInsertRows(QModelIndex(), row, row);
    getInterface()->addParameter(row);
    endInsertRows();

    // tell also parent widget that contents have been changed
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ParametersModel::onRemoveItem()
//-----------------------------------------------------------------------------
void ParametersModel::onRemoveItem(QModelIndex const& index ) 
{
	// don't remove anything if index is invalid
 	if (!index.isValid() || index.row() < 0 || index.row() >= rowCount()) 
    {
		return;
	}

    if (canRemoveRow(index.row()))
    {
        string parameterName = getInterface()->getIndexedItemName(index.row());

    	// remove the specified item
	    beginRemoveRows(QModelIndex(), index.row(), index.row());
        getInterface()->removeParameter(parameterName);
	    endRemoveRows();

    	// tell also parent widget that contents have been changed
	    emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: ParametersModel::nameColumn()
//-----------------------------------------------------------------------------
int ParametersModel::nameColumn() const
{
    return ParameterColumns::NAME;
}

//-----------------------------------------------------------------------------
// Function: ParametersModel::displayNameColumn()
//-----------------------------------------------------------------------------
int ParametersModel::displayNameColumn() const
{
    return ParameterColumns::DISPLAY_NAME;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::choiceColumn()
//-----------------------------------------------------------------------------
int ParametersModel::choiceColumn() const
{
    return ParameterColumns::CHOICE;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::formatColumn()
//-----------------------------------------------------------------------------
int ParametersModel::typeColumn() const
{
    return ParameterColumns::TYPE;
}

//-----------------------------------------------------------------------------
// Function: parametersmodel::bitWidthLeftColumn()
//-----------------------------------------------------------------------------
int ParametersModel::bitWidthLeftColumn() const
{
    return ParameterColumns::BITWIDTH_LEFT;
}

//-----------------------------------------------------------------------------
// Function: parametersmodel::bitWidthRightColumn()
//-----------------------------------------------------------------------------
int ParametersModel::bitWidthRightColumn() const
{
    return ParameterColumns::BITWIDTH_RIGHT;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::minimumColumn()
//-----------------------------------------------------------------------------
int ParametersModel::minimumColumn() const
{
    return ParameterColumns::MINIMUM;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::maximumColumn()
//-----------------------------------------------------------------------------
int ParametersModel::maximumColumn() const
{
    return ParameterColumns::MAXIMUM;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::valueColumn()
//-----------------------------------------------------------------------------
int ParametersModel::valueColumn() const
{
    return ParameterColumns::VALUE;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::resolveColumn()
//-----------------------------------------------------------------------------
int ParametersModel::resolveColumn() const
{
    return ParameterColumns::RESOLVE;
}

//-----------------------------------------------------------------------------
// Function: parametersmodel::arrayLeftColumn()
//-----------------------------------------------------------------------------
int ParametersModel::arrayLeftColumn() const
{
    return ParameterColumns::ARRAY_LEFT;
}

//-----------------------------------------------------------------------------
// Function: parametersmodel::arrayRightColumn()
//-----------------------------------------------------------------------------
int ParametersModel::arrayRightColumn() const
{
    return ParameterColumns::ARRAY_RIGHT;
}

//-----------------------------------------------------------------------------
// Function: ParametersModel::descriptionColumn()
//-----------------------------------------------------------------------------
int ParametersModel::descriptionColumn() const
{
    return ParameterColumns::DESCRIPTION;
}

//-----------------------------------------------------------------------------
// Function: parametersmodel::idColumn()
//-----------------------------------------------------------------------------
int ParametersModel::idColumn() const
{
    return ParameterColumns::ID;
}

//-----------------------------------------------------------------------------
// Function: parametersmodel::usageCountColumn()
//-----------------------------------------------------------------------------
int ParametersModel::usageCountColumn() const
{
    return ParameterColumns::USAGE_COUNT;
}
