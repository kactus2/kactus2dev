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
#include <qmath.h>


namespace
{
    const QRegExp BINARY_OPERATOR("[+-/*//]|[/*][/*]");
    const QRegExp LITERAL("((?:[(]\\s*)*)(\\s*[+-]?(?:[1-9]?[0-9]*'[sS]?[hHoObBdD])?[0-9a-fA-F_]+\\s*)([)]*)");

    const QRegExp NEXT_OPERAND("(" + BINARY_OPERATOR.pattern() + ")\\s*(" + LITERAL.pattern() + ")");
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
        equation = solveExpressionsInParentheses(equation);
        equation = solvePower(equation);
        equation = solveMultiplyAndDivide(equation);
        equation = solveAdditionAndSubtraction(equation);
        return equation.first();
    }
}
//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::parseConstantToString()
//-----------------------------------------------------------------------------
QString SystemVerilogExpressionParser::parseConstantToString(QString const& constantNumber) const
{
    int precision = 0;
    if (constantNumber.contains('.'))
    {
        precision = constantNumber.size() - constantNumber.indexOf('.') - 1;
    }

    return QString::number(parseConstant(constantNumber), 'f', precision);
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::parseConstant()
//-----------------------------------------------------------------------------
qreal SystemVerilogExpressionParser::parseConstant(QString const& constantNumber) const
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

    if (constantNumber.contains('.'))
    {
        return result.toDouble();
    }
    else
    {
        return result.toInt(0, baseForFormat(baseFormat.cap(1)));
    }
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
    QRegExp validatingExp("\\s*" + LITERAL.pattern() + "(\\s*" + NEXT_OPERAND.pattern() + ")*\\s*");

    int openParenthesisCount = expression.count('(');
    int closeParenthesisCount = expression.count(')');

    return !validatingExp.exactMatch(expression) || openParenthesisCount != closeParenthesisCount;
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::toStringList()
//-----------------------------------------------------------------------------
QStringList SystemVerilogExpressionParser::toStringList(QString const& expression) const
{
    QStringList equationList;

    LITERAL.indexIn(expression);
    QString firstOperand = LITERAL.cap();

    equationList.append(parseLiteralAndParentheses(firstOperand));

    int pos = firstOperand.length();
    while(NEXT_OPERAND.indexIn(expression, pos) != -1)
    {
        QString operation = NEXT_OPERAND.cap(1);
        QString operand = NEXT_OPERAND.cap(2);
        
        equationList.append(operation);
        equationList.append(parseLiteralAndParentheses(operand));

        pos = NEXT_OPERAND.indexIn(expression, pos) + NEXT_OPERAND.matchedLength(); 
    }

    return equationList;
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::parseLiteral()
//-----------------------------------------------------------------------------
QStringList SystemVerilogExpressionParser::parseLiteralAndParentheses(QString const& operand) const
{
    QStringList parseResult;
    LITERAL.indexIn(operand);

    int openParentheses = LITERAL.cap(1).count('(');
    int closingParentheses = LITERAL.cap(3).count(')');

    for (int i = 0; i < openParentheses; i++)
    {
        parseResult.append("(");
    }

    parseResult.append(LITERAL.cap(2));

    for (int i = 0; i < closingParentheses; i++)
    {
        parseResult.append(")");
    }

    return parseResult;
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::solveExpressionsInParentheses()
//-----------------------------------------------------------------------------
QStringList SystemVerilogExpressionParser::solveExpressionsInParentheses(QStringList const& equation) const
{
    QStringList equationWithoutParentheses;

    int position = 0;
    while(equation.indexOf("(", position) != -1)
    {
        int parenthesesStart = equation.indexOf("(", position);
        int parenthesesEnd = findMatchingEndParenthesis(equation, parenthesesStart);

        int itemsBeforeParenthesesStart =  parenthesesStart - position;

        equationWithoutParentheses.append(equation.mid(position, itemsBeforeParenthesesStart));

        int itemsInsideParenthesis = parenthesesEnd - parenthesesStart - 1;

        QString expressionInParentheses;
        foreach (QString item, equation.mid(parenthesesStart + 1, itemsInsideParenthesis))
        {
            expressionInParentheses.append(item);
        }

        equationWithoutParentheses.append(parseExpression(expressionInParentheses));
        position = parenthesesEnd + 1;
    }

    equationWithoutParentheses.append(equation.mid(position));

    return equationWithoutParentheses;
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::findMatchingEndParenthesis()
//-----------------------------------------------------------------------------
int SystemVerilogExpressionParser::findMatchingEndParenthesis(QStringList const& equation, 
    int parenthesesStart) const
{
    QRegExp parentheses("[()]");

    int position = parenthesesStart + 1;
    int depth = 1;

    while (depth > 0)
    {
        position = equation.indexOf(parentheses, position);
        if (parentheses.cap() == "(")
        {
            depth++;
        }
        else
        {
            depth--;
        }
        position++;
    }

    return position - 1;
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::solvePower()
//-----------------------------------------------------------------------------
QStringList SystemVerilogExpressionParser::solvePower(QStringList const& equation) const
{
    QRegExp power("[/*][/*]");
    return solveBinaryOperationsFromLeftToRight(equation, power);
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::parseMultiplyAndDivide()
//-----------------------------------------------------------------------------
QStringList SystemVerilogExpressionParser::solveMultiplyAndDivide(QStringList const& equation) const
{
    QRegExp multiplyDivide("[/*//]");
    return solveBinaryOperationsFromLeftToRight(equation, multiplyDivide);
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::solveAdditionAndSubtraction()
//-----------------------------------------------------------------------------
QStringList SystemVerilogExpressionParser::solveAdditionAndSubtraction(QStringList const& equation) const
{
    QRegExp addSubtract("[+-]");
    return solveBinaryOperationsFromLeftToRight(equation, addSubtract);
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::solveBinaryOperations()
//-----------------------------------------------------------------------------
QStringList SystemVerilogExpressionParser::solveBinaryOperationsFromLeftToRight(QStringList const& equation, 
    QRegExp const& binaryOperator) const
{
    QStringList solvedEquation = equation;
    while(solvedEquation.indexOf(binaryOperator) != -1)
    {
        int operatorPosition = solvedEquation.indexOf(binaryOperator);
        int firstOperandPosition = operatorPosition - 1;
        int secondOperandPosition = operatorPosition + 1;

        QString operation = solvedEquation.at(operatorPosition);
        int operand1 = parseConstant(solvedEquation.at(firstOperandPosition));
        int operand2 = parseConstant(solvedEquation.at(secondOperandPosition));

        QString result = solve(operand1, operation, operand2);

        solvedEquation.replace(firstOperandPosition, result);
        solvedEquation.removeAt(secondOperandPosition);
        solvedEquation.removeAt(operatorPosition);
    }

    return solvedEquation;
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::solve()
//-----------------------------------------------------------------------------
QString SystemVerilogExpressionParser::solve(int firstTerm, QString const& operation, int secondTerm) const
{
    int result = 0;

    if (operation == "+")
    {
        result = firstTerm + secondTerm;
    }
    else if (operation == "-")
    {
        result = firstTerm - secondTerm;
    }
    else if (operation == "*")
    {
        result = firstTerm * secondTerm;
    }
    else if (operation == "**")
    {
        if (firstTerm == 0 && secondTerm < 0)
        {
            return "x";
        }

        result = qPow(firstTerm, secondTerm);
    }
    else if (operation == "/")
    {
        if (secondTerm == 0)
        {
            return "x";
        }

        result = firstTerm  / secondTerm;
    }

    return QString::number(result);
}
