//-----------------------------------------------------------------------------
// File: ParametrizableTable.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 17.12.2014
//
// Description:
// Base class for equation usage in parameter editors.
//-----------------------------------------------------------------------------

#include "ParameterizableTable.h"

#include "ValueFormatter.h"

#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>

#include <QFont>
#include <QColor>

//-----------------------------------------------------------------------------
// Function: ParameterizableTable::ParametrizableTable()
//-----------------------------------------------------------------------------
ParameterizableTable::ParameterizableTable(QSharedPointer<ParameterFinder> parameterFinder, QObject *parent):
QAbstractTableModel(parent),
expressionParser_(),
parameterFinder_(parameterFinder)
{

}

//-----------------------------------------------------------------------------
// Function: ParameterizableTable::~ParametrizableTable()
//-----------------------------------------------------------------------------
ParameterizableTable::~ParameterizableTable()
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
    if (expressionParser_->isValidExpression(expression))
    {
        ValueFormatter formatter;
        return formatter.format(expressionParser_->parseExpression(expression),
            expressionParser_->baseForExpression(expression));
    }
    else if (expressionParser_->isPlainValue(expression))
    {
        return expression;
    }
    else
    {
        return "n/a";
    }
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
// Function: ParameterizableTable::isValuePlainOrExpression()
//-----------------------------------------------------------------------------
bool ParameterizableTable::isValuePlainOrExpression(QString const& value) const
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
// Function: ParameterizableTable::parsedExpression()
//-----------------------------------------------------------------------------
QString ParameterizableTable::parseExpressionToDecimal (QString const& expression) const
{
    return expressionParser_->parseExpression(expression);
}

//-----------------------------------------------------------------------------
// Function: ParameterizableTable::removeReferencesToItemOnRow()
//-----------------------------------------------------------------------------
void ParameterizableTable::removeReferencesInItemOnRow(const int& row) const
{
    QStringList allParameterIds = parameterFinder_->getAllParameterIds();
    foreach (QString valueID, allParameterIds)
    {
        int totalReferences = getAllReferencesToIdInItemOnRow(row, valueID);
        for (int i = 0; i < totalReferences; ++i)
        {
            emit decreaseReferences(valueID);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterizableTable::removeReferencesFromSingleExpression()
//-----------------------------------------------------------------------------
void ParameterizableTable::removeReferencesFromSingleExpression(QString const& expression) const
{
    QStringList allParameterIds = parameterFinder_->getAllParameterIds();
    foreach (QString valueID, allParameterIds)
    {
        int referencesToId = expression.count(valueID);
        for (int i = 0; i < referencesToId; ++i)
        {
            emit decreaseReferences(valueID);
        }
    }
}
