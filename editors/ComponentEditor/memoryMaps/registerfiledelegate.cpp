//-----------------------------------------------------------------------------
// File: registerfiledelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Dan Chianucci
// Date: 19.06.2018
//
// Description:
// The delegate that provides editors to add/remove/edit the details of register file
//-----------------------------------------------------------------------------

#include "registerfiledelegate.h"

#include "RegisterFileColumns.h"

#include <common/widgets/booleanComboBox/booleancombobox.h>
#include <common/widgets/accessComboBox/accesscombobox.h>

//-----------------------------------------------------------------------------
// Function: RegisterFileDelegate::RegisterFileDelegate()
//-----------------------------------------------------------------------------
RegisterFileDelegate::RegisterFileDelegate(QAbstractItemModel* completionModel,
    QSharedPointer<ParameterFinder> parameterFinder,
    QObject *parent) :
    ExpressionDelegate(completionModel, parameterFinder, parent)
{

}


//-----------------------------------------------------------------------------
// Function: RegisterFileDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int RegisterFileDelegate::descriptionColumn() const
{
    return RegisterFileColumns::DESCRIPTION;
}

//-----------------------------------------------------------------------------
// Function: RegisterFileDelegate::columnAcceptsExpression()
//-----------------------------------------------------------------------------
bool RegisterFileDelegate::columnAcceptsExpression(int column) const
{
    return column == RegisterFileColumns::DIMENSION ||
        column == RegisterFileColumns::RANGE ||
        column == RegisterFileColumns::ADDRESS_OFFSET ||
        column == RegisterFileColumns::IS_PRESENT;
}
