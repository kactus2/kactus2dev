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
    QObject* parent):
ParameterDelegate(choices, parent)
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
    return ModelParameterColumns::FORMAT;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterDelegate::bitwidthColumn()
//-----------------------------------------------------------------------------
int ModelParameterDelegate::bitwidthColumn() const
{
    return ModelParameterColumns::BITSTRINGLENGTH;
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
