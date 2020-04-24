//-----------------------------------------------------------------------------
// File: ParameterizableInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 13.03.2020
//
// Description:
// Interface for parameterizable items.
//-----------------------------------------------------------------------------

#include "ParameterizableInterface.h"

#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <IPXACTmodels/common/validators/ValueFormatter.h>

//-----------------------------------------------------------------------------
// Function: ParameterizableInterface::ParameterizableInterface()
//-----------------------------------------------------------------------------
ParameterizableInterface::ParameterizableInterface() :
expressionParser_(),
formatter_(),
valueFormatter_(new ValueFormatter())
{

}

//-----------------------------------------------------------------------------
// Function: ParameterizableInterface::setExpressionParser()
//-----------------------------------------------------------------------------
void ParameterizableInterface::setExpressionParser(QSharedPointer<ExpressionParser> parser)
{
    expressionParser_ = parser;
}

//-----------------------------------------------------------------------------
// Function: ParameterizableInterface::setExprressionFormatter()
//-----------------------------------------------------------------------------
void ParameterizableInterface::setExpressionFormatter(QSharedPointer<ExpressionFormatter> formatter)
{
    formatter_ = formatter;
}

//-----------------------------------------------------------------------------
// Function: ParameterizableInterface::formattedValueFor()
//-----------------------------------------------------------------------------
QString ParameterizableInterface::formattedValueFor(QString const& expression) const
{
    return formatter_->formatReferringExpression(expression);
}

//-----------------------------------------------------------------------------
// Function: ParameterizableInterface::parseExpressionToDecimal()
//-----------------------------------------------------------------------------
QString ParameterizableInterface::parseExpressionToDecimal(QString const& expression) const
{
    return expressionParser_->parseExpression(expression);
}

//-----------------------------------------------------------------------------
// Function: ParameterizableInterface::parseExpressionToBaseNumber()
//-----------------------------------------------------------------------------
QString ParameterizableInterface::parseExpressionToBaseNumber(QString const& expression, int const& baseNumber) const
{
    if (baseNumber == 0)
    {
        return ExpressionFormatter::format(expression, expressionParser_);
    }
    else
    {
        return valueFormatter_->format(parseExpressionToDecimal(expression), baseNumber);
    }
}
