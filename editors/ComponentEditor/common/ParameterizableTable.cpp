//-----------------------------------------------------------------------------
// File: ParametrizableTable.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 17.12.2014
//
// Description:
// Base class for equation usage in parameter editors.
//-----------------------------------------------------------------------------

#include "ParameterizableTable.h"

#include <common/KactusColors.h>

#include <KactusAPI/include/ExpressionFormatter.h>
#include <KactusAPI/include/IPXactSystemVerilogParser.h>

#include <QFont>

//-----------------------------------------------------------------------------
// Function: ParameterizableTable::ParametrizableTable()
//-----------------------------------------------------------------------------
ParameterizableTable::ParameterizableTable(QSharedPointer<ParameterFinder> parameterFinder):
expressionParser_(),
parameterFinder_(parameterFinder)
{

}

//-----------------------------------------------------------------------------
// Function: ParameterizableTable::getExpressionSymbol()
//-----------------------------------------------------------------------------
QString ParameterizableTable::getExpressionSymbol() const
{
    QString functionSymbol(0x0192);
    return QString(", " + functionSymbol + "(x)");
}

//-----------------------------------------------------------------------------
// Function: ParameterizableTable::setExpressionParser()
//-----------------------------------------------------------------------------
void ParameterizableTable::setExpressionParser(QSharedPointer <ExpressionParser> expressionParser)
{
    expressionParser_ = expressionParser;
}

//-----------------------------------------------------------------------------
// Function: ParameterizableTable::formattedValueFor()
//-----------------------------------------------------------------------------
QString ParameterizableTable::formattedValueFor(QString const& expression) const
{
    return ExpressionFormatter::format(expression, expressionParser_);
}

//-----------------------------------------------------------------------------
// Function: ParameterizableTable::italicForEvaluatedValue()
//-----------------------------------------------------------------------------
QVariant ParameterizableTable::italicForEvaluatedValue(QModelIndex const& index) const
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
// Function: ParameterizableTable::blackForValidOrRefForInavlidIndex()
//-----------------------------------------------------------------------------
QVariant ParameterizableTable::blackForValidOrRedForInvalidIndex(QModelIndex const& index) const
{
    if (validateIndex(index))
    {
        return KactusColors::REGULAR_TEXT;
    }
    else
    {
        return KactusColors::ERROR;
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterizableTable::isValidExpression()
//-----------------------------------------------------------------------------
bool ParameterizableTable::isValidExpression(QString const& value) const
{
    bool validExpression = false;
    expressionParser_->parseExpression(value, &validExpression);
    return validExpression;
}

//-----------------------------------------------------------------------------
// Function: ParameterizableTable::parsedExpression()
//-----------------------------------------------------------------------------
QString ParameterizableTable::parseExpressionToDecimal (QString const& expression) const
{
    return expressionParser_->parseExpression(expression);
}

//-----------------------------------------------------------------------------
// Function: ParameterizableTable::getParameterFinder()
//-----------------------------------------------------------------------------
QSharedPointer<ParameterFinder> ParameterizableTable::getParameterFinder() const
{
    return parameterFinder_;
}
