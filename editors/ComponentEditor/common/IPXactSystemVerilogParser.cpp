//-----------------------------------------------------------------------------
// File: IPXactSystemVerilogParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 02.12.2014
//
// Description:
// Parser for SystemVerilog expressions with parameter references.
//-----------------------------------------------------------------------------

#include "IPXactSystemVerilogParser.h"

#include <editors/ComponentEditor/common/ParameterFinder.h>

#include <QRegularExpression>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: IPXactSystemVerilogParser::IPXactSystemVerilogParser()
//-----------------------------------------------------------------------------
IPXactSystemVerilogParser::IPXactSystemVerilogParser(QSharedPointer<ParameterFinder> finder):
SystemVerilogExpressionParser(), finder_(finder)
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

    return SystemVerilogExpressionParser::parseExpression(parseReferencesIn(expression));
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

    return SystemVerilogExpressionParser::isValidExpression(replaceReferencesWithValues(expression));
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

    return SystemVerilogExpressionParser::baseForExpression(replaceReferencesWithValues(expression));
}

//-----------------------------------------------------------------------------
// Function: IPXactSystemVerilogParser::parseReferencesIn()
//-----------------------------------------------------------------------------
QString IPXactSystemVerilogParser::parseReferencesIn(QString const& expression, unsigned int recursionStep) const
{
    if (recursionStep == MAX_EVALUATION_STEPS)
    {
        return QStringLiteral("x");
    }

    QString evaluatedExpression;
    foreach (QString const& term, toStringList(expression))
    {
        if (finder_->hasId(term))
        {
            QString value = parseReferencesIn(finder_->valueForId(term), recursionStep + 1);
            evaluatedExpression.append(SystemVerilogExpressionParser::parseExpression(value));
        }
        else
        {
            evaluatedExpression.append(term);
        }
    }

    return evaluatedExpression;
}

//-----------------------------------------------------------------------------
// Function: IPXactSystemVerilogParser::replaceReferencesWithValues()
//-----------------------------------------------------------------------------
QString IPXactSystemVerilogParser::replaceReferencesWithValues(QString const& expression, unsigned int recursionStep) const
{
    if (recursionStep == MAX_EVALUATION_STEPS)
    {
        return QStringLiteral("x");
    }

    QString evaluatedExpression;
    foreach (QString const& term, toStringList(expression))
    {
        if (finder_->hasId(term))
        {
            evaluatedExpression.append(replaceReferencesWithValues(finder_->valueForId(term), recursionStep + 1));
        }
        else
        {
            evaluatedExpression.append(term);
        }
    }

    return evaluatedExpression;
}
