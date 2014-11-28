//-----------------------------------------------------------------------------
// File: SystemVerilogExpressionParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.11.2014
//
// Description:
// Parser for SystemVerilog expressions.
//-----------------------------------------------------------------------------

#include "SystemVerilogExpressionParser.h"

#include <QRegExp>
#include <QStringList>

namespace
{
    const QRegExp OPERATORS("[+-/*//]");
    const QRegExp LITERALS("[+-]?([1-9]?[0-9]*'[sS]?[hHoObBdD])?[0-9a-fA-F_]+");

    const QRegExp NEXT_OPERAND("(" + OPERATORS.pattern() + ")\\s*(" + LITERALS.pattern() + ")");
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::SystemVerilogExpressionParser()
//-----------------------------------------------------------------------------
SystemVerilogExpressionParser::SystemVerilogExpressionParser()
{

}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::~SystemVerilogExpressionParser()
//-----------------------------------------------------------------------------
SystemVerilogExpressionParser::~SystemVerilogExpressionParser()
{

}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::parseExpression()
//-----------------------------------------------------------------------------
QString SystemVerilogExpressionParser::parseExpression(QString const& expression) const
{
    if (expression.isEmpty())
    {
        return "0";
    }
    
    if (isNotValidExpression(expression))
    {
        return "x";
    }

    QStringList equation = toStringList(expression);
    if (equation.size() == 1)
    {
        return QString::number(parseConstant(equation.first()));
    }
    else
    {
        equation = solveMultiplyAndDivide(equation);
        equation = solveAdditionAndSubtraction(equation);
        return equation.first();
    }
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::parseConstant()
//-----------------------------------------------------------------------------
int SystemVerilogExpressionParser::parseConstant(QString const& constantNumber) const
{
    QRegExp size("([1-9][0-9_]*)?(?=')");
    QRegExp sign("[sS]?");
    QRegExp baseFormat("'[sS]?([dDbBoOhH])");

    baseFormat.indexIn(constantNumber);

    QString result = constantNumber;
    result.remove(size);
    result.remove(sign);
    result.remove(baseFormat);
    result.remove('_');

    return result.toInt(0, baseForFormat(baseFormat.cap(1)));
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::baseForFormat()
//-----------------------------------------------------------------------------
int SystemVerilogExpressionParser::baseForFormat(QString const& baseFormat) const
{
    if (baseFormat == "h" || baseFormat == "H")
    {
        return 16;
    }
    else if (baseFormat == "d" || baseFormat == "D")
    {
        return 10;
    }
    else if (baseFormat == "o" || baseFormat == "O")
    {
        return  8;
    }
    else if (baseFormat == "b" || baseFormat == "B")
    {
        return 2;
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::isNotValidExpression()
//-----------------------------------------------------------------------------
bool SystemVerilogExpressionParser::isNotValidExpression(QString const& expression) const
{
    QRegExp validatingExp("\\s*" + LITERALS.pattern() + "(\\s*" + NEXT_OPERAND.pattern() + ")*\\s*");
    return !validatingExp.exactMatch(expression);
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::toStringList()
//-----------------------------------------------------------------------------
QStringList SystemVerilogExpressionParser::toStringList(QString const& expression) const
{
    QStringList equationList;

    LITERALS.indexIn(expression);
    equationList.append(LITERALS.cap());

    int pos = LITERALS.matchedLength();
    while(NEXT_OPERAND.indexIn(expression, pos) != -1)
    {
        equationList.append(NEXT_OPERAND.cap(1));
        equationList.append(NEXT_OPERAND.cap(2));

        pos = NEXT_OPERAND.indexIn(expression, pos) + NEXT_OPERAND.matchedLength(); 
    }

    return equationList;
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::parseMultiplyAndDivide()
//-----------------------------------------------------------------------------
QStringList SystemVerilogExpressionParser::solveMultiplyAndDivide(QStringList const& equation) const
{
    QRegExp multiplyDivide("[/*//]");

    QStringList solvedEquation = equation;
    while(solvedEquation.indexOf(multiplyDivide) != -1)
    {
        int operatorPosition = solvedEquation.indexOf(multiplyDivide);
        int firstOperandPosition = operatorPosition - 1;
        int secondOperandPosition = operatorPosition + 1;

        QString operation = solvedEquation.at(operatorPosition);
        int operand1 = parseConstant(solvedEquation.at(firstOperandPosition));
        int operand2 = parseConstant(solvedEquation.at(secondOperandPosition));

        int result = 0;
        if (operation == "*")
        {
            result = operand1 * operand2;
        }
        else if (operation == "/")
        {
            if (operand2 == 0)
            {
                return QStringList("x");
            }

            result = operand1  / operand2;
        }

        solvedEquation.replace(firstOperandPosition, QString::number(result));
        solvedEquation.removeAt(secondOperandPosition);
        solvedEquation.removeAt(operatorPosition);
    }

    return solvedEquation;
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::solveAdditionAndSubtraction()
//-----------------------------------------------------------------------------
QStringList SystemVerilogExpressionParser::solveAdditionAndSubtraction(QStringList const& equation) const
{
    QStringList solvedEquation = equation;

    while(solvedEquation.size() > 2)
    {
        QString operation = solvedEquation.at(1);
        int firstTerm = parseConstant(solvedEquation.at(0));
        int secondTerm = parseConstant(solvedEquation.at(2));

        int result = 0;
        if (operation == "+")
        {
            result = firstTerm + secondTerm;
        }
        else if (operation == "-")
        {
            result = firstTerm - secondTerm;
        }

        solvedEquation.replace(0, QString::number(result));
        solvedEquation.removeAt(2);
        solvedEquation.removeAt(1);
    }

    return solvedEquation;
}
