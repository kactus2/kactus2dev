//-----------------------------------------------------------------------------
// File: ModelParameterDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 31.3.2011
//
// Description:
// Delegate that provides widgets for editing model parameters.
//-----------------------------------------------------------------------------

#include "ModelParameterDelegate.h"

#include "ModelParameterColumns.h"

#include <IPXACTmodels/choice.h>
#include <IPXACTmodels/Enumeration.h>

#include <QComboBox>
#include <QLineEdit>

//-----------------------------------------------------------------------------
// Function: ModelParameterDelegate::ModelParameterDelegate()
//-----------------------------------------------------------------------------
ModelParameterDelegate::ModelParameterDelegate(QSharedPointer<QList<QSharedPointer<Choice> > > choices, 
    QCompleter* parameterCompleter, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter, QObject* parent):
ParameterDelegate(choices, parameterCompleter, parameterFinder, expressionFormatter, parent)
{

}

//-----------------------------------------------------------------------------
// Function: ModelParameterDelegate::~ModelParameterDelegate()
//-----------------------------------------------------------------------------
ModelParameterDelegate::~ModelParameterDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: ModelParameterDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* ModelParameterDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
    QModelIndex const& index ) const
{
	if (index.column() == ModelParameterColumns::USAGE_TYPE) 
    {
		QComboBox* combo = new QComboBox(parent);
		combo->addItem(QString("typed"));
		combo->addItem(QString("nontyped"));
		return combo;
	}
    else
    {
        return ParameterDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ModelParameterDelegate::nameColumn()
//-----------------------------------------------------------------------------
int ModelParameterDelegate::nameColumn() const
{
    return ModelParameterColumns::NAME;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterDelegate::choiceColumn()
//-----------------------------------------------------------------------------
int ModelParameterDelegate::choiceColumn() const
{
    return ModelParameterColumns::CHOICE;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterDelegate::formatColumn()
//-----------------------------------------------------------------------------
int ModelParameterDelegate::formatColumn() const
{
    return ModelParameterColumns::TYPE;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterDelegate::bitWidthLeftColumn()
//-----------------------------------------------------------------------------
int ModelParameterDelegate::bitWidthLeftColumn() const
{
    return ModelParameterColumns::BITWIDTH_LEFT;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterDelegate::bitWidthRightColumn()
//-----------------------------------------------------------------------------
int ModelParameterDelegate::bitWidthRightColumn() const
{
    return ModelParameterColumns::BITWIDTH_RIGHT;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterDelegate::minimumColumn()
//-----------------------------------------------------------------------------
int ModelParameterDelegate::minimumColumn() const
{
    return ModelParameterColumns::MINIMUM;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterDelegate::maximumColumn()
//-----------------------------------------------------------------------------
int ModelParameterDelegate::maximumColumn() const
{
    return ModelParameterColumns::MAXIMUM;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterDelegate::valueColumn()
//-----------------------------------------------------------------------------
int ModelParameterDelegate::valueColumn() const
{
    return ModelParameterColumns::VALUE;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterDelegate::resolveColumn()
//-----------------------------------------------------------------------------
int ModelParameterDelegate::resolveColumn() const
{
    return ModelParameterColumns::RESOLVE;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int ModelParameterDelegate::descriptionColumn() const
{
    return ModelParameterColumns::DESCRIPTION;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterDelegate::arrayLeftColumn()
//-----------------------------------------------------------------------------
int ModelParameterDelegate::arrayLeftColumn() const
{
    return ModelParameterColumns::ARRAY_LEFT;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterDelegate::arrayRightColumn()
//-----------------------------------------------------------------------------
int ModelParameterDelegate::arrayRightColumn() const
{
    return ModelParameterColumns::ARRAY_RIGHT;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterDelegate::usageCountColumn()
//-----------------------------------------------------------------------------
int ModelParameterDelegate::usageCountColumn() const
{
    return ModelParameterColumns::USAGE_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterDelegate::idColumn()
//-----------------------------------------------------------------------------
int ModelParameterDelegate::idColumn() const
{
    return ModelParameterColumns::ID;
}
