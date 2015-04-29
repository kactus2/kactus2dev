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

    expressionList.append(field->getBitOffsetExpression());
    expressionList.append(field->getBitWidthExpression());
    expressionList.append(field->getIsPresentExpression());

    return expressionList;
}
