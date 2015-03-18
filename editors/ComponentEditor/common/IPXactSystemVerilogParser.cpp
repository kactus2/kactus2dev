//-----------------------------------------------------------------------------
// File: IPXactSystemVerilogParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 02.12.2014
//
// Description:
// Parser for SystemVerilog expressions within one IP-XACT component.
//-----------------------------------------------------------------------------

#include "IPXactSystemVerilogParser.h"

#include <editors/ComponentEditor/common/ParameterFinder.h>

#include <QStringList>

//-----------------------------------------------------------------------------
// Function: IPXactSystemVerilogParser::IPXactSystemVerilogParser()
//-----------------------------------------------------------------------------
IPXactSystemVerilogParser::IPXactSystemVerilogParser(QSharedPointer<ParameterFinder> finder)
    : SystemVerilogExpressionParser(), finder_(finder)
{

}

//-----------------------------------------------------------------------------
// Function: IPXactSystemVerilogParser::~IPXactSystemVerilogParser()
//-----------------------------------------------------------------------------
IPXactSystemVerilogParser::~IPXactSystemVerilogParser()
{

}

//-----------------------------------------------------------------------------
// Function: IPXactSystemVerilogParser::parseExpression()
//-----------------------------------------------------------------------------
QString IPXactSystemVerilogParser::parseExpression(QString const& expression) const
{
    if (SystemVerilogExpressionParser::isValidExpression(expression))
    {
        return SystemVerilogExpressionParser::parseExpression(expression);
    }

    QString evaluatedExpression = evaluateReferencesIn(expression, finder_->getAllParameterIds(), 0);
    if (SystemVerilogExpressionParser::isValidExpression(evaluatedExpression))
    {
        return SystemVerilogExpressionParser::parseExpression(evaluatedExpression);
    }
  
    return evaluatedExpression;
}

//-----------------------------------------------------------------------------
// Function: IPXactSystemVerilogParser::isValidExpression()
//-----------------------------------------------------------------------------
bool IPXactSystemVerilogParser::isValidExpression(QString const& expression) const
{
    if (SystemVerilogExpressionParser::isValidExpression(expression))
    {
        return true;
    }

    QString evaluatedExpression = replaceReferencesIn(expression, finder_->getAllParameterIds(), 0);
    return SystemVerilogExpressionParser::isValidExpression(evaluatedExpression);
}

//-----------------------------------------------------------------------------
// Function: IPXactSystemVerilogParser::baseForExpression()
//-----------------------------------------------------------------------------
int IPXactSystemVerilogParser::baseForExpression(QString const& expression) const
{
    if (SystemVerilogExpressionParser::isValidExpression(expression))
    {
        return SystemVerilogExpressionParser::baseForExpression(expression);
    }

    QString evaluatedExpression = replaceReferencesIn(expression, finder_->getAllParameterIds(), 0);

    return SystemVerilogExpressionParser::baseForExpression(evaluatedExpression);
}

//-----------------------------------------------------------------------------
// Function: IPXactSystemVerilogParser::evaluateReferencesIn()
//-----------------------------------------------------------------------------
QString IPXactSystemVerilogParser::evaluateReferencesIn(QString const& expression, QStringList const& availableIds,
    int recursionStep) const
{
    if (shouldTerminateRecursion(recursionStep))
    {
        return "x";
    }

    if (SystemVerilogExpressionParser::isValidExpression(expression))
    {
        return expression;
    }

    QString evaluated = expression;
    foreach(QString referenceId, availableIds)
    {
        if (expression.contains(referenceId))
        {
            QString parameterValue = 
                evaluateReferencesIn(finder_->valueForId(referenceId), availableIds, recursionStep + 1);

            if (SystemVerilogExpressionParser::isValidExpression(parameterValue))
            {
                parameterValue = SystemVerilogExpressionParser::parseExpression(parameterValue);
            }

            evaluated.replace(referenceId, parameterValue);
        }
    }

    return evaluated;
}

//-----------------------------------------------------------------------------
// Function: IPXactSystemVerilogParser::replaceReferencesIn()
//-----------------------------------------------------------------------------
QString IPXactSystemVerilogParser::replaceReferencesIn(QString const& expression, 
    QStringList const& availableIds, int recursionStep) const
{
    if (shouldTerminateRecursion(recursionStep))
    {
        return "x";
    }

    if (SystemVerilogExpressionParser::isValidExpression(expression))
    {
        return expression;
    }

    QString evaluated = expression;
    foreach(QString referenceId, availableIds)
    {
        if (expression.contains(referenceId))
        {
            QString parameterValue = 
                replaceReferencesIn(finder_->valueForId(referenceId), availableIds, recursionStep + 1);

            evaluated.replace(referenceId, parameterValue);
        }
    }

    return evaluated;
}

//-----------------------------------------------------------------------------
// Function: IPXactSystemVerilogParser::shouldTerminateRecursion()
//-----------------------------------------------------------------------------
bool IPXactSystemVerilogParser::shouldTerminateRecursion(int recursionStep) const
{
    return recursionStep == MAX_RECURSION_STEPS;
}
