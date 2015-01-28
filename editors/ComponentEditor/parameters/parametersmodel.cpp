//-----------------------------------------------------------------------------
// File: parametersmodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 4.4.2011
//
// Description:
// Table model that can be used to display parameters to be edited.
//-----------------------------------------------------------------------------

#include "parametersmodel.h"

#include "ParameterColumns.h"

#include <IPXACTmodels/choice.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/Enumeration.h>

#include <QColor>
#include <QUuid>

//-----------------------------------------------------------------------------
// Function: ParametersModel::ParametersModel()
//-----------------------------------------------------------------------------
ParametersModel::ParametersModel(QList<QSharedPointer<Parameter> >& parameters,
    QSharedPointer<QList<QSharedPointer<Choice> > > choices, QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ExpressionFormatter> expressionFormatter, QObject *parent): 
AbstractParameterModel(choices, expressionParser, expressionFormatter, parent), 
parameters_(parameters)
{

}

//-----------------------------------------------------------------------------
// Function: ParametersModel::~ParametersModel()
//-----------------------------------------------------------------------------
ParametersModel::~ParametersModel()
{

}

//-----------------------------------------------------------------------------
// Function: ParametersModel::rowCount()
//-----------------------------------------------------------------------------
int ParametersModel::rowCount(QModelIndex const& parent /*= QModelIndex() */ ) const
{
	if (parent.isValid())
    {
		return 0;
	}

	return parameters_.size();
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
// Function: ParametersModel::onAddItem()
//-----------------------------------------------------------------------------
void ParametersModel::onAddItem(QModelIndex const& index)
{
    int row = parameters_.size();

    // if the index is valid then add the item to the correct position
    if (index.isValid()) {
        row = index.row();
    }

    beginInsertRows(QModelIndex(), row, row);
    QSharedPointer<Parameter> createdParameter(new Parameter());
    parameters_.insert(row, createdParameter);
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

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	parameters_.removeAt(index.row());
	endRemoveRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ParametersModel::getParameterOnRow()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> ParametersModel::getParameterOnRow(int row) const
{
    return parameters_.at(row);
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
// Function: ParameterDelegate::bitwidthColumn()
//-----------------------------------------------------------------------------
int ParametersModel::bitWidthColumn() const
{
    return ParameterColumns::BITWIDTH;
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
// Function: ParametersModel::arraySizeColumn()
//-----------------------------------------------------------------------------
int ParametersModel::arraySizeColumn() const
{
    return ParameterColumns::ARRAY_SIZE;
}

//-----------------------------------------------------------------------------
// Function: ParametersModel::arrayOffsetColumn()
//-----------------------------------------------------------------------------
int ParametersModel::arrayOffsetColumn() const
{
    return ParameterColumns::ARRAY_OFFSET;
}

//-----------------------------------------------------------------------------
// Function: ParametersModel::descriptionColumn()
//-----------------------------------------------------------------------------
int ParametersModel::descriptionColumn() const
{
    return ParameterColumns::DESCRIPTION;
}

//-----------------------------------------------------------------------------
// Function: parametersmodel::valueIdColumn()
//-----------------------------------------------------------------------------
int ParametersModel::valueIdColumn() const
{
    return ParameterColumns::VALUEID;
}

//-----------------------------------------------------------------------------
// Function: parametersmodel::usageCountColumn()
//-----------------------------------------------------------------------------
int ParametersModel::usageCountColumn() const
{
    return ParameterColumns::USAGE_COUNT;
}