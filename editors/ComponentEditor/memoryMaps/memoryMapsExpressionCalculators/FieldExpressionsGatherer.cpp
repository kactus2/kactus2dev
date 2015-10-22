//-----------------------------------------------------------------------------
// File: FieldExpressionsGatherer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 29.04.2015
//
// Description:
// Gathers expressions from a field.
//-----------------------------------------------------------------------------

#include "FieldExpressionsGatherer.h"

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
    expressionList.append(field->getResetValue());
    expressionList.append(field->getResetMask());

    if (field->getWriteConstraint())
    {
        expressionList.append(field->getWriteConstraint()->getMinimum());
        expressionList.append(field->getWriteConstraint()->getMaximum());
    }

    return expressionList;
}
