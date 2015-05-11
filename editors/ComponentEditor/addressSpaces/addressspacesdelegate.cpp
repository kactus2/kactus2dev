/* 
 *  	Created on: 11.6.2012
 *      Author: Antti Kamppi
 * 		filename: addressspacesdelegate.cpp
 *		Project: Kactus 2
 */

#include "addressspacesdelegate.h"

#include "AddressSpaceColumns.h"

#include <QLineEdit>
#include <QSpinBox>

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
