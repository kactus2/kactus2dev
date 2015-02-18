//-----------------------------------------------------------------------------
// File: ExpressionFormatterFactoryImplementation.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 16.02.2015
//
// Description:
// Class for expression formatter factory.
//-----------------------------------------------------------------------------

#include "ExpressionFormatterFactoryImplementation.h"

#include <editors/ComponentEditor/common/ComponentParameterFinder.h>

//-----------------------------------------------------------------------------
// Function: ExpressionFormatterFactoryImplementation::ExpressionFormatterFactoryImplementation()
//-----------------------------------------------------------------------------
ExpressionFormatterFactoryImplementation::ExpressionFormatterFactoryImplementation()
{

}

//-----------------------------------------------------------------------------
// Function: ExpressionFormatterFactoryImplementation::~ExpressionFormatterFactoryImplementation()
//-----------------------------------------------------------------------------
ExpressionFormatterFactoryImplementation::~ExpressionFormatterFactoryImplementation()
{

}

//-----------------------------------------------------------------------------
// Function: ExpressionFormatterFactoryImplementation::makeExpressionFormatter()
//-----------------------------------------------------------------------------
ExpressionFormatter* ExpressionFormatterFactoryImplementation::makeExpressionFormatter(
    QSharedPointer<Component> component)
{
    QSharedPointer<ParameterFinder> parameterFinder (new ComponentParameterFinder(component));

    ExpressionFormatter* expressionFormatter = new ExpressionFormatter(parameterFinder);

    return expressionFormatter;
}
