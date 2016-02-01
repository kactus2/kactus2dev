//-----------------------------------------------------------------------------
// File: ModuleParameterDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 31.3.2011
//
// Description:
// Delegate that provides widgets for editing model parameters.
//-----------------------------------------------------------------------------

#include "ModuleParameterDelegate.h"

#include <editors/ComponentEditor/modelParameters/ModelParameterColumns.h>

#include <QComboBox>

//-----------------------------------------------------------------------------
// Function: ModuleParameterDelegate::ModuleParameterDelegate()
//-----------------------------------------------------------------------------
ModuleParameterDelegate::ModuleParameterDelegate(QSharedPointer<QList<QSharedPointer<Choice> > > choices, 
    QCompleter* parameterCompleter, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter, QObject* parent):
ParameterDelegate(choices, parameterCompleter, parameterFinder, expressionFormatter, parent)
{

}

//-----------------------------------------------------------------------------
// Function: ModuleParameterDelegate::~ModuleParameterDelegate()
//-----------------------------------------------------------------------------
ModuleParameterDelegate::~ModuleParameterDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: ModuleParameterDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* ModuleParameterDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
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
// Function: ModuleParameterDelegate::nameColumn()
//-----------------------------------------------------------------------------
int ModuleParameterDelegate::nameColumn() const
{
    return ModelParameterColumns::NAME;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterDelegate::choiceColumn()
//-----------------------------------------------------------------------------
int ModuleParameterDelegate::choiceColumn() const
{
    return ModelParameterColumns::CHOICE;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterDelegate::formatColumn()
//-----------------------------------------------------------------------------
int ModuleParameterDelegate::formatColumn() const
{
    return ModelParameterColumns::TYPE;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterDelegate::bitWidthLeftColumn()
//-----------------------------------------------------------------------------
int ModuleParameterDelegate::bitWidthLeftColumn() const
{
    return ModelParameterColumns::BITWIDTH_LEFT;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterDelegate::bitWidthRightColumn()
//-----------------------------------------------------------------------------
int ModuleParameterDelegate::bitWidthRightColumn() const
{
    return ModelParameterColumns::BITWIDTH_RIGHT;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterDelegate::minimumColumn()
//-----------------------------------------------------------------------------
int ModuleParameterDelegate::minimumColumn() const
{
    return ModelParameterColumns::MINIMUM;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterDelegate::maximumColumn()
//-----------------------------------------------------------------------------
int ModuleParameterDelegate::maximumColumn() const
{
    return ModelParameterColumns::MAXIMUM;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterDelegate::valueColumn()
//-----------------------------------------------------------------------------
int ModuleParameterDelegate::valueColumn() const
{
    return ModelParameterColumns::VALUE;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterDelegate::resolveColumn()
//-----------------------------------------------------------------------------
int ModuleParameterDelegate::resolveColumn() const
{
    return ModelParameterColumns::RESOLVE;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int ModuleParameterDelegate::descriptionColumn() const
{
    return ModelParameterColumns::DESCRIPTION;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterDelegate::arrayLeftColumn()
//-----------------------------------------------------------------------------
int ModuleParameterDelegate::arrayLeftColumn() const
{
    return ModelParameterColumns::ARRAY_LEFT;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterDelegate::arrayRightColumn()
//-----------------------------------------------------------------------------
int ModuleParameterDelegate::arrayRightColumn() const
{
    return ModelParameterColumns::ARRAY_RIGHT;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterDelegate::usageCountColumn()
//-----------------------------------------------------------------------------
int ModuleParameterDelegate::usageCountColumn() const
{
    return ModelParameterColumns::USAGE_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterDelegate::idColumn()
//-----------------------------------------------------------------------------
int ModuleParameterDelegate::idColumn() const
{
    return ModelParameterColumns::ID;
}
