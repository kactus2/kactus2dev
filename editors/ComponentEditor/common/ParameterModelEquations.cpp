//-----------------------------------------------------------------------------
// File: ParameterModelEquations.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 17.12.2014
//
// Description:
// Base class for equation usage in parameter editors.
//-----------------------------------------------------------------------------

#include "ParameterModelEquations.h"

#include "ValueFormatter.h"

#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>

#include <QFont>
#include <QColor>

//-----------------------------------------------------------------------------
// Function: ParameterModelEquations::ParameterModelEquations()
//-----------------------------------------------------------------------------
ParameterModelEquations::ParameterModelEquations():
expressionParser_()
{

}

//-----------------------------------------------------------------------------
// Function: ParameterModelEquations::~ParameterModelEquations()
//-----------------------------------------------------------------------------
ParameterModelEquations::~ParameterModelEquations()
{

}

//-----------------------------------------------------------------------------
// Function: ParameterModelEquations::getExpressionSymbol()
//-----------------------------------------------------------------------------
QString ParameterModelEquations::getExpressionSymbol() const
{
    QString functionSymbol(0x0192);
    return QString(", " + functionSymbol + "(x)");
}

//-----------------------------------------------------------------------------
// Function: ParameterModelEquations::setExpressionParser()
//-----------------------------------------------------------------------------
void ParameterModelEquations::setExpressionParser(QSharedPointer <ExpressionParser> expressionParser)
{
    expressionParser_ = expressionParser;
}

//-----------------------------------------------------------------------------
// Function: ParameterModelEquations::formattedValueFor()
//-----------------------------------------------------------------------------
QString ParameterModelEquations::formattedValueFor(QString const& expression) const
{
    if (expressionParser_->isPlainValue(expression))
    {
        return expression;
    }
    else if (expressionParser_->isValidExpression(expression))
    {
        ValueFormatter formatter;
        return formatter.format(expressionParser_->parseExpression(expression),
            expressionParser_->baseForExpression(expression));
    }
    else
    {
        return "n/a";
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterModelEquations::italicForEvaluatedValue()
//-----------------------------------------------------------------------------
QVariant ParameterModelEquations::italicForEvaluatedValue(QModelIndex const& index) const
{
    QString valueOrExpression = expressionOrValueForIndex(index).toString();

    if (isValidExpressionColumn(index) && !expressionParser_->isPlainValue(valueOrExpression))
    {
        QFont italicFont;
        italicFont.setItalic(true);
        return italicFont;
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterModelEquations::blackForValidOrRefForInavlidIndex()
//-----------------------------------------------------------------------------
QVariant ParameterModelEquations::blackForValidOrRedForInvalidIndex(QModelIndex const& index) const
{
    if (validateColumnForParameter(index))
    {
        return QColor("black");
    }
    else
    {
        return QColor("red");
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterModelEquations::isValuePlainOrExpression()
//-----------------------------------------------------------------------------
bool ParameterModelEquations::isValuePlainOrExpression(QString const& value) const
{
    if (expressionParser_->isPlainValue(value) || expressionParser_->isValidExpression(value))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterModelEquations::parsedExpression()
//-----------------------------------------------------------------------------
QString ParameterModelEquations::parseExpressionToDecimal (QString const& expression) const
{
    return expressionParser_->parseExpression(expression);
}
