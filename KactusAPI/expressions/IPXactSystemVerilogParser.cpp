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

#include "ParameterFinder.h"

//-----------------------------------------------------------------------------
// Function: IPXactSystemVerilogParser::IPXactSystemVerilogParser()
//-----------------------------------------------------------------------------
IPXactSystemVerilogParser::IPXactSystemVerilogParser(QSharedPointer<ParameterFinder> finder):
SystemVerilogExpressionParser(), finder_(finder)
{

}

//-----------------------------------------------------------------------------
// Function: IPXactSystemVerilogParser::isSymbol()
//-----------------------------------------------------------------------------
bool IPXactSystemVerilogParser::isSymbol(QStringView expression) const
{
    return finder_->hasId(expression);
}

//-----------------------------------------------------------------------------
// Function: IPXactSystemVerilogParser::findSymbolValue()
//-----------------------------------------------------------------------------
QString IPXactSystemVerilogParser::findSymbolValue(QStringView expression) const
{
    // Check for ring references.
    if (symbolStack_.contains(expression))
    {
        symbolStack_.clear();
        return QStringLiteral("x");
    }

    symbolStack_.append(expression);
    QString value = parseExpression(finder_->valueForId(expression));
    if (symbolStack_.isEmpty() == false)
    {
        symbolStack_.removeLast();
    }

    return value;
}

//-----------------------------------------------------------------------------
// Function: IPXactSystemVerilogParser::findSymbolValue()
//-----------------------------------------------------------------------------
int IPXactSystemVerilogParser::getBaseForSymbol(QStringView symbol) const
{
    return baseForExpression(finder_->valueForId(symbol));
}
