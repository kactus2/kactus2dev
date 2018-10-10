//-----------------------------------------------------------------------------
// File: RegisterExpressionGatherer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Dan Chianucci
// Date: 11.06.2018
//
// Description:
// Gathers expressions from a register file and its registers.
//-----------------------------------------------------------------------------

#include "RegisterFileExpressionsGatherer.h"
#include "RegisterExpressionsGatherer.h"

#include <IPXACTmodels/Component/RegisterFile.h>
#include <IPXACTmodels/Component/Register.h>

//-----------------------------------------------------------------------------
// Function: RegisterFileExpressionsGatherer::RegisterFileExpressionsGatherer()
//-----------------------------------------------------------------------------
RegisterFileExpressionsGatherer::RegisterFileExpressionsGatherer()
{

}

//-----------------------------------------------------------------------------
// Function: RegisterFileExpressionsGatherer::getExpressions()
//-----------------------------------------------------------------------------
QStringList RegisterFileExpressionsGatherer::getExpressions(QSharedPointer<RegisterFile> currentRegisterFile) const
{
    QStringList expressionList;

    expressionList.append(currentRegisterFile->getAddressOffset());
    expressionList.append(currentRegisterFile->getRange());
    expressionList.append(currentRegisterFile->getDimension());
    expressionList.append(currentRegisterFile->getIsPresent());

    RegisterExpressionsGatherer regGatherer;

    foreach (QSharedPointer<RegisterBase> regBase, *currentRegisterFile->getRegisterData())
    {
        QSharedPointer<Register> regItem = regBase.dynamicCast<Register>();
        QSharedPointer<RegisterFile> regFileItem = regBase.dynamicCast<RegisterFile >();

        if(regItem)
        {
            expressionList.append(regGatherer.getExpressions(regItem));
        }
        else if(regFileItem)
        {
            expressionList.append(getExpressions(regFileItem));
        }
    }

    return expressionList;
}
