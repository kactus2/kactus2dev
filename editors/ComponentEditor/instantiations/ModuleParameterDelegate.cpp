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

#include "ModuleParameterColumns.h"

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
	if (index.column() == ModuleParameterColumns::USAGE_TYPE) 
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
    return ModuleParameterColumns::NAME;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterDelegate::choiceColumn()
//-----------------------------------------------------------------------------
int ModuleParameterDelegate::choiceColumn() const
{
    return ModuleParameterColumns::CHOICE;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterDelegate::formatColumn()
//-----------------------------------------------------------------------------
int ModuleParameterDelegate::formatColumn() const
{
    return ModuleParameterColumns::TYPE;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterDelegate::bitWidthLeftColumn()
//-----------------------------------------------------------------------------
int ModuleParameterDelegate::bitWidthLeftColumn() const
{
    return ModuleParameterColumns::BITWIDTH_LEFT;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterDelegate::bitWidthRightColumn()
//-----------------------------------------------------------------------------
int ModuleParameterDelegate::bitWidthRightColumn() const
{
    return ModuleParameterColumns::BITWIDTH_RIGHT;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterDelegate::minimumColumn()
//-----------------------------------------------------------------------------
int ModuleParameterDelegate::minimumColumn() const
{
    return ModuleParameterColumns::MINIMUM;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterDelegate::maximumColumn()
//-----------------------------------------------------------------------------
int ModuleParameterDelegate::maximumColumn() const
{
    return ModuleParameterColumns::MAXIMUM;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterDelegate::valueColumn()
//-----------------------------------------------------------------------------
int ModuleParameterDelegate::valueColumn() const
{
    return ModuleParameterColumns::VALUE;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterDelegate::resolveColumn()
//-----------------------------------------------------------------------------
int ModuleParameterDelegate::resolveColumn() const
{
    return ModuleParameterColumns::RESOLVE;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int ModuleParameterDelegate::descriptionColumn() const
{
    return ModuleParameterColumns::DESCRIPTION;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterDelegate::arrayLeftColumn()
//-----------------------------------------------------------------------------
int ModuleParameterDelegate::arrayLeftColumn() const
{
    return ModuleParameterColumns::ARRAY_LEFT;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterDelegate::arrayRightColumn()
//-----------------------------------------------------------------------------
int ModuleParameterDelegate::arrayRightColumn() const
{
    return ModuleParameterColumns::ARRAY_RIGHT;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterDelegate::usageCountColumn()
//-----------------------------------------------------------------------------
int ModuleParameterDelegate::usageCountColumn() const
{
    return ModuleParameterColumns::USAGE_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterDelegate::idColumn()
//-----------------------------------------------------------------------------
int ModuleParameterDelegate::idColumn() const
{
    return ModuleParameterColumns::ID;
}
