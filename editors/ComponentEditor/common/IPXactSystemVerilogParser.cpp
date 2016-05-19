//-----------------------------------------------------------------------------
// File: IPXactSystemVerilogParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 02.12.2014
//
// Description:
// Parser for SystemVerilog expressions within one IP-XACT component.
//-----------------------------------------------------------------------------

#include "IPXactSystemVerilogParser.h"

#include <editors/ComponentEditor/common/ParameterFinder.h>

#include <QRegularExpression>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: IPXactSystemVerilogParser::IPXactSystemVerilogParser()
//-----------------------------------------------------------------------------
IPXactSystemVerilogParser::IPXactSystemVerilogParser(QSharedPointer<ParameterFinder> finder):
SystemVerilogExpressionParser(), 
    finder_(finder)
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

    QString evaluatedExpression = evaluateReferencesIn(expression);
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

    return SystemVerilogExpressionParser::isValidExpression(evaluateReferencesIn(expression));
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

    return SystemVerilogExpressionParser::baseForExpression(evaluateReferencesIn(expression));
}

//-----------------------------------------------------------------------------
// Function: IPXactSystemVerilogParser::evaluateReferencesIn()
//-----------------------------------------------------------------------------
QString IPXactSystemVerilogParser::evaluateReferencesIn(QString const& expression) const
{
    QString validatingExpression('(' + finder_->getAllParameterIds().join('|') + "){1}");
    if (validatingExpression.length() == 5)
    {
        return expression;
    }

    QRegularExpression parameterIdExpression(validatingExpression);
    
    QString evaluatedExpression = expression;

    int steps = MAX_EVALUATION_STEPS;
    int pos = evaluatedExpression.indexOf(parameterIdExpression);
    while(pos != -1 && steps > 0)
    {
        QRegularExpressionMatch match = parameterIdExpression.match(evaluatedExpression);

        QString referenceId = match.captured(1);
        QString parameterValue = finder_->valueForId(referenceId);

        if (!isPlainValue(parameterValue) && evaluatedExpression != referenceId)
        {
            parameterValue.prepend('(');
            parameterValue.append(')');
        }

        evaluatedExpression.replace(referenceId, parameterValue);
        
        pos = evaluatedExpression.indexOf(parameterIdExpression);
        steps--;
    }

    if (steps == 0)
    {
        return "x";
    }

    return evaluatedExpression;
}
