//-----------------------------------------------------------------------------
// File: ModeConditionParserInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 27.09.2024
//
// Description:
// Interface for creating and handling expression parsers for mode conditions. The base interface is independent of 
// KactusAPI and should be used in IPXACTmodels validators.
//-----------------------------------------------------------------------------

#include "ModeConditionParserInterface.h"

#include <KactusAPI/include/ModeConditionParser.h>

//-----------------------------------------------------------------------------
// Function: ModeConditionParserInterface::ModeConditionParserInterface()
//-----------------------------------------------------------------------------
ModeConditionParserInterface::ModeConditionParserInterface(QSharedPointer<ParameterFinder> parameterFinder):
    parameterFinder_(parameterFinder)
{

}

//-----------------------------------------------------------------------------
// Function: ModeConditionParserInterface::createParser()
//-----------------------------------------------------------------------------
QSharedPointer<ExpressionParser> ModeConditionParserInterface::createParser()
{
    return QSharedPointer<ModeConditionParser>(new ModeConditionParser(parameterFinder_, nullptr, nullptr, nullptr));
}

//-----------------------------------------------------------------------------
// Function: ModeConditionParserInterface::setFieldSlices()
//-----------------------------------------------------------------------------
void ModeConditionParserInterface::setFieldSlices(QSharedPointer<ExpressionParser> expressionParser, FieldSlice::List fieldSlices)
{
    expressionParser.staticCast<ModeConditionParser>()->setFieldSlices(fieldSlices);
}

//-----------------------------------------------------------------------------
// Function: ModeConditionParserInterface::setPortSlices()
//-----------------------------------------------------------------------------
void ModeConditionParserInterface::setPortSlices(QSharedPointer<ExpressionParser> expressionParser, PortSlice::List portSlices)
{
    expressionParser.staticCast<ModeConditionParser>()->setPortSlices(portSlices);
}

//-----------------------------------------------------------------------------
// Function: ModeConditionParserInterface::setModes()
//-----------------------------------------------------------------------------
void ModeConditionParserInterface::setModes(QSharedPointer<ExpressionParser> expressionParser, Mode::List modes)
{
    expressionParser.staticCast<ModeConditionParser>()->setModes(modes);
}
