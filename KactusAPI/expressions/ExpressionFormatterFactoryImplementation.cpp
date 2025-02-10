//-----------------------------------------------------------------------------
// File: ExpressionFormatterFactoryImplementation.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 16.02.2015
//
// Description:
// Class for expression formatter factory.
//-----------------------------------------------------------------------------

#include "ExpressionFormatterFactoryImplementation.h"

#include "ComponentParameterFinder.h"
#include "ListParameterFinder.h"
#include "MultipleParameterFinder.h"

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

//-----------------------------------------------------------------------------
// Function: ExpressionFormatterFactoryImplementation::createDesignInstanceFormatter()
//-----------------------------------------------------------------------------
ExpressionFormatter* ExpressionFormatterFactoryImplementation::createDesignInstanceFormatter(
    QSharedPointer<Component> component, QSharedPointer<Design> design)
{
    QSharedPointer<ListParameterFinder> designFinder(new ListParameterFinder());
    designFinder->setParameterList(design->getParameters());

    QSharedPointer<ComponentParameterFinder> componentFinder(new ComponentParameterFinder(component));

    QSharedPointer<MultipleParameterFinder> designInstanceFinder(new MultipleParameterFinder());
    designInstanceFinder->addFinder(designFinder);
    designInstanceFinder->addFinder(componentFinder);

    ExpressionFormatter* formatter = new ExpressionFormatter(designInstanceFinder);
    return formatter;
}
