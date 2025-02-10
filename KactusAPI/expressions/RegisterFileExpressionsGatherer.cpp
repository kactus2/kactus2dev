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
#include <IPXACTmodels/Component/MemoryArray.h>

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
    
    if (auto memArray = currentRegisterFile->getMemoryArray())
    {
        for (auto const& dim : *memArray->getDimensions())
        {
            expressionList.append(dim->value_);
        }
    }

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
