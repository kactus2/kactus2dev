//-----------------------------------------------------------------------------
// File: FieldExpressionsGatherer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 29.04.2015
//
// Description:
// Gathers expressions from a field.
//-----------------------------------------------------------------------------

#include "FieldExpressionsGatherer.h"

#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/FieldReset.h>
#include <IPXACTmodels/Component/WriteValueConstraint.h>

//-----------------------------------------------------------------------------
// Function: FieldExpressionGatherer::FieldExpressionGatherer()
//-----------------------------------------------------------------------------
FieldExpressionsGatherer::FieldExpressionsGatherer()
{

}

//-----------------------------------------------------------------------------
// Function: FieldExpressionsGatherer::~FieldExpressionsGatherer()
//-----------------------------------------------------------------------------
FieldExpressionsGatherer::~FieldExpressionsGatherer()
{

}

//-----------------------------------------------------------------------------
// Function: FieldExpressionGatherer::getExpressions()
//-----------------------------------------------------------------------------
QStringList FieldExpressionsGatherer::getExpressions(QSharedPointer<Field> field) const
{
    QStringList expressionList;

    expressionList.append(field->getBitOffset());
    expressionList.append(field->getBitWidth());
    expressionList.append(field->getIsPresent());

    if (field->getResets())
    {
        for (auto fieldReset : *field->getResets())
        {
            expressionList.append(fieldReset->getResetValue());
            expressionList.append(fieldReset->getResetMask());
        }
    }

    if (field->getWriteConstraint())
    {
        expressionList.append(field->getWriteConstraint()->getMinimum());
        expressionList.append(field->getWriteConstraint()->getMaximum());
    }

    return expressionList;
}
