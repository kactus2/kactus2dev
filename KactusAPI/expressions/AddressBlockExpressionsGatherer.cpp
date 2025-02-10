//-----------------------------------------------------------------------------
// File: AddressBlockExpressionGatherer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 29.04.2015
//
// Description:
// Gathers expressions from an address block and its registers.
//-----------------------------------------------------------------------------

#include "AddressBlockExpressionsGatherer.h"
#include "RegisterExpressionsGatherer.h"
#include "RegisterFileExpressionsGatherer.h"
#include <IPXACTmodels/Component/RegisterBase.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/RegisterFile.h>
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
    expressionList.append(currentAddressBlock->getWidth());
    expressionList.append(currentAddressBlock->getIsPresent());

    RegisterExpressionsGatherer registerGatherer;
    RegisterFileExpressionsGatherer registerFileGatherer;

    foreach (QSharedPointer<RegisterBase> registerModel, *currentAddressBlock->getRegisterData())
    {
        QSharedPointer<Register> targetRegister = registerModel.dynamicCast<Register>();
        QSharedPointer<RegisterFile> targetRegisterFile = registerModel.dynamicCast<RegisterFile>();
        if (targetRegister)
        {
          expressionList.append(registerGatherer.getExpressions(targetRegister));
        }
        else if(targetRegisterFile){
          expressionList.append(registerFileGatherer.getExpressions(targetRegisterFile));
        }
    }

    return expressionList;
}
