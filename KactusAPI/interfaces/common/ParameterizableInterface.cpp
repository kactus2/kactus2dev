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

#include <ParameterizableInterface.h>

#include <KactusAPI/include/ExpressionFormatter.h>
#include <KactusAPI/include/ExpressionParser.h>

#include <IPXACTmodels/common/validators/ValueFormatter.h>

//-----------------------------------------------------------------------------
// Function: ParameterizableInterface::ParameterizableInterface()
//-----------------------------------------------------------------------------
ParameterizableInterface::ParameterizableInterface(QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ExpressionFormatter> expressionFormatter):
expressionParser_(expressionParser),
formatter_(expressionFormatter),
valueFormatter_(new ValueFormatter())
{

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
QString ParameterizableInterface::parseExpressionToDecimal(QString const& expression, bool* expressionIsValid /*= nullptr*/) const
{
    return expressionParser_->parseExpression(expression, expressionIsValid);
}

//-----------------------------------------------------------------------------
// Function: ParameterizableInterface::parseExpressionToBaseNumber()
//-----------------------------------------------------------------------------
QString ParameterizableInterface::parseExpressionToBaseNumber(QString const& expression, unsigned int const& baseNumber, bool* expressionIsValid /*= nullptr*/) const
{
    if (baseNumber == 0)
    {
        return ExpressionFormatter::format(expression, expressionParser_, expressionIsValid);
    }
    else
    {
        return valueFormatter_->format(parseExpressionToDecimal(expression, expressionIsValid), baseNumber);
    }
}
