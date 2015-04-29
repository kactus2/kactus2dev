//-----------------------------------------------------------------------------
// File: AddressBlockExpressionGatherer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 29.04.2015
//
// Description:
// Gathers expressions from an address block and its registers.
//-----------------------------------------------------------------------------

#include "AddressBlockExpressionsGatherer.h"
#include "RegisterExpressionsGatherer.h"

#include <IPXACTmodels/register.h>
#include <IPXACTmodels/registermodel.h>

//-----------------------------------------------------------------------------
// Function: AddressBlockExpressionsGatherer::AddressBlockExpressionGatherer()
//-----------------------------------------------------------------------------
AddressBlockExpressionGatherer::AddressBlockExpressionGatherer()
{

}

//-----------------------------------------------------------------------------
// Function: AddressBlockExpressionsGatherer::~AddressBlockExpressionGatherer()
//-----------------------------------------------------------------------------
AddressBlockExpressionGatherer::~AddressBlockExpressionGatherer()
{

}

//-----------------------------------------------------------------------------
// Function: AddressBlockExpressionsGatherer::getExpressions()
//-----------------------------------------------------------------------------
QStringList AddressBlockExpressionGatherer::getExpressions(QSharedPointer<AddressBlock> currentAddressBlock) const
{
    QStringList expressionList;

    expressionList.append(currentAddressBlock->getBaseAddress());
    expressionList.append(currentAddressBlock->getRange());
    expressionList.append(currentAddressBlock->getWidthExpression());

    RegisterExpressionsGatherer* registerGatherer = new RegisterExpressionsGatherer();

    foreach (QSharedPointer<RegisterModel> registerModel, currentAddressBlock->getRegisterData())
    {
        QSharedPointer<Register> targetRegister = registerModel.dynamicCast<Register>();

        if (targetRegister)
        {
            expressionList.append(registerGatherer->getExpressions(targetRegister));
        }
    }

    return expressionList;
}
