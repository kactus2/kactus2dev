//-----------------------------------------------------------------------------
// File: ArrayDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 12.03.2015
//
// Description:
// Delegate that provides widgets for editing arrays.
//-----------------------------------------------------------------------------

#include "ArrayDelegate.h"
#include "ArrayColumns.h"

#include <IPXACTmodels/Component/Choice.h>

#include <QComboBox>

//-----------------------------------------------------------------------------
// Function: ArrayDelegate::ArrayDelegate()
//-----------------------------------------------------------------------------
ArrayDelegate::ArrayDelegate(QCompleter* parameterCompleter, QSharedPointer<ParameterFinder> finder,
    QSharedPointer<Choice> selectedChoice, QObject* parent):
ChoiceCreatorDelegate(parameterCompleter, finder, parent),
selectedChoice_(selectedChoice)
{

}

//-----------------------------------------------------------------------------
// Function: ArrayDelegate::~ArrayDelegate()
//-----------------------------------------------------------------------------
ArrayDelegate::~ArrayDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: ArrayDelegate::columnAcceptsExpression()
//-----------------------------------------------------------------------------
bool ArrayDelegate::columnAcceptsExpression(int column) const
{
    return column == ArrayColumns::VALUE;
}

//-----------------------------------------------------------------------------
// Function: ArrayDelegate::valueColumn()
//-----------------------------------------------------------------------------
int ArrayDelegate::valueColumn() const
{
    return ArrayColumns::VALUE;
}

//-----------------------------------------------------------------------------
// Function: ArrayDelegate::choiceNameOnRow()
//-----------------------------------------------------------------------------
QString ArrayDelegate::choiceNameOnRow(QModelIndex const& /*index*/) const
{
    return selectedChoice_->name();
}

//-----------------------------------------------------------------------------
// Function: ArrayDelegate::findChoice()
//-----------------------------------------------------------------------------
QSharedPointer<Choice> ArrayDelegate::findChoice(QModelIndex const& /*index*/) const
{
    return selectedChoice_;
}

//-----------------------------------------------------------------------------
// Function: ArrayDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int ArrayDelegate::descriptionColumn() const
{
    return -1;
}
