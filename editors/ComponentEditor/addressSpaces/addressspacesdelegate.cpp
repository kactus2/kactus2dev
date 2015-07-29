//-----------------------------------------------------------------------------
// File: addressspacesdelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 11.6.2012
//
// Description:
// The delegate that provides editors to edit the address spaces summary.
//-----------------------------------------------------------------------------

#include "addressspacesdelegate.h"

#include "AddressSpaceColumns.h"

//-----------------------------------------------------------------------------
// Function: AddressSpacesDelegate::AddressSpacesDelegate()
//-----------------------------------------------------------------------------
AddressSpacesDelegate::AddressSpacesDelegate(QCompleter* parameterNameCompleter, 
    QSharedPointer<ParameterFinder> parameterFinder, QObject *parent):
ExpressionDelegate(parameterNameCompleter, parameterFinder, parent)
{

}

//-----------------------------------------------------------------------------
// Function: AddressSpacesDelegate::~AddressSpacesDelegate()
//-----------------------------------------------------------------------------
AddressSpacesDelegate::~AddressSpacesDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: AddressSpacesDelegate::columnAcceptsExpression()
//-----------------------------------------------------------------------------
bool AddressSpacesDelegate::columnAcceptsExpression(int column) const
{
    return column == AddressSpaceColumns::RANGE || column == AddressSpaceColumns::WIDTH;        
}

//-----------------------------------------------------------------------------
// Function: AddressSpacesDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int AddressSpacesDelegate::descriptionColumn() const
{
    return AddressSpaceColumns::DESCRIPTION;
}
