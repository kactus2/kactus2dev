//-----------------------------------------------------------------------------
// File: ModeConditionParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 16.10.2023
//
// Description:
// Parser for Mode condition expressions.
//-----------------------------------------------------------------------------

#include "ModeConditionParser.h"
#include "IPXactSystemVerilogParser.h"

#include <IPXACTmodels/utilities/Search.h>

//-----------------------------------------------------------------------------
// Function: ModeConditionParser::ModeConditionParser()
//-----------------------------------------------------------------------------
ModeConditionParser::ModeConditionParser(QSharedPointer<ParameterFinder> finder,
    PortSlice::List portSlices,
    FieldSlice::List fieldSlices,
    Mode::List modes):
    IPXactSystemVerilogParser(finder),
    portSlices_(portSlices),
    fieldSlices_(fieldSlices),
    modes_(modes)
{

}

//-----------------------------------------------------------------------------
// Function: ModeConditionParser::setFieldSlices()
//-----------------------------------------------------------------------------
void ModeConditionParser::setFieldSlices(FieldSlice::List fieldSlices)
{
    fieldSlices_ = fieldSlices;
}

//-----------------------------------------------------------------------------
// Function: ModeConditionParser::setPortSlices()
//-----------------------------------------------------------------------------
void ModeConditionParser::setPortSlices(PortSlice::List portSlices)
{
    portSlices_ = portSlices;
}

//-----------------------------------------------------------------------------
// Function: ModeConditionParser::setModes()
//-----------------------------------------------------------------------------
void ModeConditionParser::setModes(Mode::List modes)
{
    modes_ = modes;
}

//-----------------------------------------------------------------------------
// Function: ModeConditionParser::isSymbol()
//-----------------------------------------------------------------------------
bool ModeConditionParser::isSymbol(QStringView expression) const
{
    return isModeReference(expression) || IPXactSystemVerilogParser::isSymbol(expression);
}

//-----------------------------------------------------------------------------
// Function: ModeConditionParser::findSymbolValue()
//-----------------------------------------------------------------------------
QString ModeConditionParser::findSymbolValue(QStringView symbol) const
{
    if (isModeReference(symbol))
    {
        return symbol.toString();
    }

    return IPXactSystemVerilogParser::findSymbolValue(symbol);
}

//-----------------------------------------------------------------------------
// Function: ModeConditionParser::isUnaryOperator()
//-----------------------------------------------------------------------------
bool ModeConditionParser::isUnaryOperator(QStringView token) const
{
    return token == QLatin1String("$ipxact_port_value") ||
        token == QLatin1String("$ipxact_field_value") || 
        token == QLatin1String("$ipxact_mode_condition") ||
        IPXactSystemVerilogParser::isUnaryOperator(token);
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::solveUnary()
//-----------------------------------------------------------------------------
QString ModeConditionParser::solveUnary(QStringView operation, QString const& term) const
{
    if (operation.compare(QLatin1String("$ipxact_port_value")) == 0)
    {
        return solveCondition(term, portSlices_);
    }
    else if (operation.compare(QLatin1String("$ipxact_field_value")) == 0)
    {
        return solveCondition(term, fieldSlices_);
    }
    else if (operation.compare(QLatin1String("$ipxact_mode_condition")) == 0)
    {
        return solveCondition(term, modes_);
    }

    return SystemVerilogExpressionParser::solveUnary(operation, term);
}

//-----------------------------------------------------------------------------
// Function: ModeConditionParser::isModeReference()
//-----------------------------------------------------------------------------
bool ModeConditionParser::isModeReference(QStringView expression) const
{
    return Search::findByName(expression, *portSlices_) != nullptr ||
        Search::findByName(expression, *fieldSlices_) != nullptr ||
        Search::findByName(expression, *modes_) != nullptr;
}
