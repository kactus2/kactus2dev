//-----------------------------------------------------------------------------
// File: RegisterExpressionGatherer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 29.04.2015
//
// Description:
// Gathers expressions from a register and its fields.
//-----------------------------------------------------------------------------

#include "RegisterExpressionsGatherer.h"
#include "FieldExpressionsGatherer.h"

#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/MemoryArray.h>

//-----------------------------------------------------------------------------
// Function: RegisterExpressionsGatherer::RegisterExpressionsGatherer()
//-----------------------------------------------------------------------------
RegisterExpressionsGatherer::RegisterExpressionsGatherer()
{

}

//-----------------------------------------------------------------------------
// Function: RegisterExpressionsGatherer::~RegisterExpressionsGatherer()
//-----------------------------------------------------------------------------
RegisterExpressionsGatherer::~RegisterExpressionsGatherer()
{

}

//-----------------------------------------------------------------------------
// Function: RegisterExpressionsGatherer::getExpressions()
//-----------------------------------------------------------------------------
QStringList RegisterExpressionsGatherer::getExpressions(QSharedPointer<Register> currentRegister) const
{
    QStringList expressionList;

    expressionList.append(currentRegister->getAddressOffset());
    expressionList.append(currentRegister->getSize());

    if (auto memArray = currentRegister->getMemoryArray())
    {
        for (auto const& dim : *memArray->getDimensions())
        {
            expressionList.append(dim->value_);
        }
    }

    expressionList.append(currentRegister->getIsPresent());

    FieldExpressionsGatherer fieldGatherer;

    foreach (QSharedPointer<Field> field, *currentRegister->getFields())
    {
        expressionList.append(fieldGatherer.getExpressions(field));
    }

    return expressionList;
}
