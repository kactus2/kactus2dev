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
#include <QRegularExpression>
#include <QStringList>
#include <qmath.h>


namespace
{
    const QString SIZE = "[1-9]?[0-9]*";
    const QString SIGNED = "[sS]?";

    const QString DECIMAL_NUMBER = "(?:" + SIZE + "'" + SIGNED + "[dD]?)?[0-9_]+";
    const QString HEXADECIMAL_NUMBER = SIZE + "'" + SIGNED + "[hH][0-9a-fA-F_]+";
    const QString OCTAL_NUMBER = SIZE + "'" + SIGNED + "[oO][0-7_]+";
    const QString BINARY_NUMBER = SIZE + "'" + SIGNED + "[bB][01_]+";

    const QString INTEGRAL_NUMBER("[+-]?" + DECIMAL_NUMBER + "|" + HEXADECIMAL_NUMBER + "|" +
        OCTAL_NUMBER + "|" + BINARY_NUMBER);

    const QString CLOG2_FUNCTION = "[$]clog2[(](?:[^)]+)[)]";

    const QString REAL_NUMBER("[+-]?[0-9_]+[.][0-9_]+");

    const QString STRING_LITERAL("\".*\"");

    const QRegExp PRIMARY_LITERAL("((?:[(]\\s*)*)\\s*(" + INTEGRAL_NUMBER + "|" + REAL_NUMBER + "|" + 
        CLOG2_FUNCTION + ")\\s*((?:[)]\\s*)*)");

    const QRegExp BINARY_OPERATOR("[+-/*//]|[/*][/*]");
    const QRegExp NEXT_OPERAND("(" + BINARY_OPERATOR.pattern() + ")\\s*(" + PRIMARY_LITERAL.pattern() + ")");
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
    if (!isValidExpression(expression))
    {
        return "x";
    }

    if (isStringLiteral(expression))
    {
        return expression.trimmed();
    }

    QStringList equation = toStringList(expression);

    equation = solveMathFuctions(equation);
    equation = solveExpressionsInParentheses(equation);
    equation = solvePower(equation);
    equation = solveMultiplyAndDivide(equation);
    equation = solveAdditionAndSubtraction(equation);

    if (equation.first() == "x")
    {
        return "x";
    }
    else
    {
        int precision = 0;
        if (equation.first().contains('.'))
        {
            precision = equation.first().size() - equation.first().indexOf('.') - 1;
        }

        return QString::number(parseConstantToDecimal(equation.first()), 'f', precision);
    }
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::isValidExpression()
//-----------------------------------------------------------------------------
bool SystemVerilogExpressionParser::isValidExpression(QString const& expression) const
{
    QRegularExpression validatingExp("^\\s*(" + STRING_LITERAL + "|"
        "(" + PRIMARY_LITERAL.pattern() + "(\\s*" + NEXT_OPERAND.pattern() + ")*))\\s*$");

    int openParenthesisCount = expression.count('(');
    int closeParenthesisCount = expression.count(')');

    return validatingExp.match(expression).hasMatch() && openParenthesisCount == closeParenthesisCount;
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::isPlainValue()
//-----------------------------------------------------------------------------
bool SystemVerilogExpressionParser::isPlainValue(QString const& expression) const
{
    return expression.isEmpty() || (!expression.contains(BINARY_OPERATOR) && 
        !expression.contains(QRegExp(CLOG2_FUNCTION)));
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::baseForExpression()
//-----------------------------------------------------------------------------
int SystemVerilogExpressionParser::baseForExpression(QString const& expression) const
{
    int greatestBase = 0;
    foreach(QString term, toStringList(expression))
    {
        if (isLiteral(term))
        {
            greatestBase = qMax(greatestBase, getBaseForNumber(term));
        }
    }

    return greatestBase;
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::isStringLiteral()
//-----------------------------------------------------------------------------
bool SystemVerilogExpressionParser::isStringLiteral(QString const &expression) const
{
    QRegExp stringExpression("^\\s*" + STRING_LITERAL + "\\s*$");
    return expression.indexOf(stringExpression) == 0;
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::isLiteral()
//-----------------------------------------------------------------------------
bool SystemVerilogExpressionParser::isLiteral(QString const& expression) const
{
    return QRegExp(INTEGRAL_NUMBER).exactMatch(expression);
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::toStringList()
//-----------------------------------------------------------------------------
QStringList SystemVerilogExpressionParser::toStringList(QString const& expression) const
{
    QStringList equationList;

    PRIMARY_LITERAL.indexIn(expression);
    QString firstOperand = PRIMARY_LITERAL.cap();

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
    PRIMARY_LITERAL.indexIn(operand);

    int openParentheses = PRIMARY_LITERAL.cap(1).count('(');
    int closingParentheses = PRIMARY_LITERAL.cap(3).count(')');

    for (int i = 0; i < openParentheses; i++)
    {
        parseResult.append("(");
    }

    parseResult.append(PRIMARY_LITERAL.cap(2));

    for (int i = 0; i < closingParentheses; i++)
    {
        parseResult.append(")");
    }

    return parseResult;
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::solveMathFuctions()
//-----------------------------------------------------------------------------
QStringList SystemVerilogExpressionParser::solveMathFuctions(QStringList const& equation) const
{
    QStringList solvedEquation = equation;
   
    QRegExp clog2(QString(CLOG2_FUNCTION).replace("(?:", "("));

    int functionIndex = solvedEquation.indexOf(clog2);
    while (functionIndex != -1)
    {
        QString argument = clog2.cap(1);

        solvedEquation.replace(functionIndex, solveClog2(argument));

        functionIndex = solvedEquation.indexOf(clog2);
    }

    return solvedEquation;
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::solveClog2()
//-----------------------------------------------------------------------------
QString SystemVerilogExpressionParser::solveClog2(QString const& value) const
{
    qreal quotient = parseConstantToDecimal(parseExpression(value));
    
    if (quotient < 0)
    {
        return "x";
    }
    else if (quotient == 1)
    {
        return "1";
    }    

    int answer = 0;
    while (quotient > 1)
    {
        quotient /= 2;
        answer++;
    }

    return QString::number(answer);
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
        QString operand1 = solvedEquation.at(firstOperandPosition);
        QString operand2 = solvedEquation.at(secondOperandPosition);

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
QString SystemVerilogExpressionParser::solve(QString const& firstTerm, QString const& operation, 
    QString const& secondTerm) const
{
    qreal leftOperand = parseConstantToDecimal(firstTerm);
    qreal rightOperand = parseConstantToDecimal(secondTerm);

    qreal result = 0;

    if (operation == "+")
    {
        result = leftOperand + rightOperand;
    }
    else if (operation == "-")
    {
        result = leftOperand - rightOperand;
    }
    else if (operation == "*")
    {
        result = leftOperand*rightOperand;
    }
    else if (operation == "**")
    {
        if (leftOperand == 0 && rightOperand < 0)
        {
            return "x";
        }

        result = qPow(leftOperand, rightOperand);
    }
    else if (operation == "/")
    {
        if (rightOperand == 0)
        {
            return "x";
        }

        result = leftOperand/rightOperand;
    }

    if (!firstTerm.contains('.'))
    {
        int integerResult = result;
        return QString::number(integerResult);
    }
    else
    {
        return QString::number(result);
    }
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::parseConstant()
//-----------------------------------------------------------------------------
qreal SystemVerilogExpressionParser::parseConstantToDecimal(QString const& constantNumber) const
{
    QRegExp size("([1-9][0-9_]*)?(?=')");
    QRegExp baseFormat("'" + SIGNED + "[dDbBoOhH]?");

    QString result = constantNumber;
    result.remove(size);
    result.remove(baseFormat);
    result.remove('_');

    if (constantNumber.contains('.'))
    {
        return result.toDouble();
    }
    else
    {
        return result.toLongLong(0, getBaseForNumber(constantNumber));
    }
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::getBaseForNumber()
//-----------------------------------------------------------------------------
int SystemVerilogExpressionParser::getBaseForNumber(QString const& constantNumber) const
{
    QRegExp baseFormat("'" + SIGNED + "([dDbBoOhH]?)");

    baseFormat.indexIn(constantNumber);
    QString base = baseFormat.cap(1);

    return baseForFormat(base);
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
    else if (baseFormat.isEmpty() || baseFormat == "d" || baseFormat == "D")
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
