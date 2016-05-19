//-----------------------------------------------------------------------------
// File: SystemVerilogExpressionParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 25.11.2014
//
// Description:
// Parser for SystemVerilog expressions.
//-----------------------------------------------------------------------------

#include "SystemVerilogExpressionParser.h"
#include "SystemVerilogSyntax.h"

#include <QRegularExpression>
#include <QStringList>
#include <qmath.h>
#include "qnamespace.h"

namespace
{
    const QString CLOG2_FUNCTION = "[$]clog2[(](?:[^)]+)[)]";

	const QString BOOLEAN_VALUE = "(?i)true|(?i)false";

    const QString REAL_NUMBER("[+-]?[0-9_]+[.][0-9_]+");

    const QRegularExpression PRIMARY_LITERAL("((?:[(]\\s*)*)\\s*(" + REAL_NUMBER + "|" + BOOLEAN_VALUE + "|"
		+ SystemVerilogSyntax::INTEGRAL_NUMBER + "|"  + CLOG2_FUNCTION + ")\\s*((?:[)]\\s*)*)");

    const QRegularExpression BINARY_OPERATOR("[+-/*//]|[/*][/*]");
    const QRegularExpression COMPARISON_OPERATOR("[<>]=?|[!=]=");

    const QRegularExpression COMBINED_OPERATOR(BINARY_OPERATOR.pattern() + "|" + COMPARISON_OPERATOR.pattern());

    const QRegularExpression NEXT_OPERAND("(" + COMBINED_OPERATOR.pattern() + ")\\s*"
        "(" + PRIMARY_LITERAL.pattern() + ")");
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
    
    if (isArrayExpression(expression))
    {
        QStringList subExpressions = expression.split(',');

        QRegularExpression removeFromStart ("^'?{");

        subExpressions.first().remove(removeFromStart);
        subExpressions.last().chop(1);

        QString arrayExpression("{");

        for (int arrayIndex = 0; arrayIndex < subExpressions.size(); ++arrayIndex)
        {
            arrayExpression.append(parseExpression(subExpressions.at(arrayIndex)));
            if (arrayIndex < subExpressions.size() - 1)
            {
                arrayExpression.append(',');
            }
        }
        arrayExpression.append('}');

        return arrayExpression;
    }

    if (isComparison(expression))
    {
        return parseComparison(expression);
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
// Function: SystemVerilogExpressionParser::isComparison()
//-----------------------------------------------------------------------------
bool SystemVerilogExpressionParser::isComparison(QString const& expression) const
{
    QRegularExpression comparisonExpression (".+" + COMPARISON_OPERATOR.pattern() + ".+");

    return comparisonExpression.match(expression).hasMatch();
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::parseComparison()
//-----------------------------------------------------------------------------
QString SystemVerilogExpressionParser::parseComparison(QString const& expression) const
{
    QString solvedComparison = expression;

    QString selectedOperator = COMPARISON_OPERATOR.match(solvedComparison).captured();

    int operatorPosition = solvedComparison.indexOf(selectedOperator);

    QString partialComparison =
        solvedComparison.right(solvedComparison.size() - (operatorPosition + selectedOperator.size()));

    while(COMPARISON_OPERATOR.match(partialComparison).hasMatch())
    {
        selectedOperator =
            COMPARISON_OPERATOR.match(partialComparison).captured();
        operatorPosition = partialComparison.indexOf(selectedOperator);

        partialComparison =
            partialComparison.right(partialComparison.size() - (operatorPosition + selectedOperator.size()));
    }

    operatorPosition = solvedComparison.indexOf(selectedOperator);

    QString leftOperand = solvedComparison.left(operatorPosition);
    QString rightOperand =
        solvedComparison.right(solvedComparison.size() - (operatorPosition + selectedOperator.size()));

    int leftResult = parseExpression(leftOperand).toInt();
    int rightResult = parseExpression(rightOperand).toInt();

    QString comparisonResult = "0";

    if ((selectedOperator == ">" && leftResult > rightResult) ||
        (selectedOperator == "<" && leftResult < rightResult) ||
        (selectedOperator == "==" && leftResult == rightResult) ||
        (selectedOperator == ">=" && leftResult >= rightResult) ||
        (selectedOperator == "<=" && leftResult <= rightResult) ||
        (selectedOperator == "!=" && leftResult != rightResult))
    {
        comparisonResult = "1";
    }

    return comparisonResult;
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::isValidExpression()
//-----------------------------------------------------------------------------
bool SystemVerilogExpressionParser::isValidExpression(QString const& expression) const
{
    static QString stringOrExpression ("\\s*(" + SystemVerilogSyntax::STRING_LITERAL + "|"
        "(" + PRIMARY_LITERAL.pattern() + "(\\s*" + NEXT_OPERAND.pattern() + ")*))\\s*");

    static QString arrayExpression("('?{" + stringOrExpression + "(," + stringOrExpression + ")*})");

    static QRegularExpression validatingExp("^\\s*(" + SystemVerilogSyntax::STRING_LITERAL + "|"
        "(" + PRIMARY_LITERAL.pattern() + "(\\s*" + NEXT_OPERAND.pattern() + ")*)|" + arrayExpression +
        ")\\s*$");

    int openParenthesisCount = expression.count('(');
    int closeParenthesisCount = expression.count(')');

    int arrayOpenCount = expression.count('{');
    int arrayCloseCount = expression.count('}');

    return validatingExp.match(expression).hasMatch() && 
        openParenthesisCount == closeParenthesisCount &&
        arrayOpenCount == arrayCloseCount;
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::isPlainValue()
//-----------------------------------------------------------------------------
bool SystemVerilogExpressionParser::isPlainValue(QString const& expression) const
{
    return expression.isEmpty() || isLiteral(expression) || isStringLiteral(expression);
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
    static QRegularExpression stringExpression("^\\s*" + SystemVerilogSyntax::STRING_LITERAL + "\\s*$");
    return expression.indexOf(stringExpression) == 0;
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::isArrayExpression()
//-----------------------------------------------------------------------------
bool SystemVerilogExpressionParser::isArrayExpression(QString const& expression) const
{
    return expression.contains('{') && expression.contains('}');
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::isLiteral()
//-----------------------------------------------------------------------------
bool SystemVerilogExpressionParser::isLiteral(QString const& expression) const
{
    static QRegularExpression literalExpression("^\\s*[(]*\\s*(" + SystemVerilogSyntax::INTEGRAL_NUMBER +
        "|" + REAL_NUMBER + ")\\s*[)]*\\s*$");

    return literalExpression.match(expression).hasMatch();
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::toStringList()
//-----------------------------------------------------------------------------
QStringList SystemVerilogExpressionParser::toStringList(QString const& expression) const
{
    QStringList equationList;

    QString firstOperand = PRIMARY_LITERAL.match(expression).captured();

    equationList.append(parseLiteralAndParentheses(firstOperand));

    int pos = firstOperand.length();
    while(expression.indexOf(NEXT_OPERAND, pos) != -1)
    {
        QRegularExpressionMatch matchedOperatorAndOperand = NEXT_OPERAND.match(expression, pos);
        QString operation = matchedOperatorAndOperand.captured(1);
        QString operand = matchedOperatorAndOperand.captured(2);
        
        equationList.append(operation);
        equationList.append(parseLiteralAndParentheses(operand));

        pos = expression.indexOf(NEXT_OPERAND, pos) + matchedOperatorAndOperand.capturedLength(); 
    }

    return equationList;
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::parseLiteral()
//-----------------------------------------------------------------------------
QStringList SystemVerilogExpressionParser::parseLiteralAndParentheses(QString const& operand) const
{
    QStringList parseResult;
    
    QRegularExpressionMatch matchedParenthesesAndLiteral = PRIMARY_LITERAL.match(operand);

    int openParentheses = matchedParenthesesAndLiteral.captured(1).count('(');
    int closingParentheses = matchedParenthesesAndLiteral.captured(3).count(')');

    for (int i = 0; i < openParentheses; i++)
    {
        parseResult.append("(");
    }

    parseResult.append(matchedParenthesesAndLiteral.captured(2));

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
   
    QRegularExpression clog2Expression(QString(CLOG2_FUNCTION).replace("(?:", "("));

    int functionIndex = solvedEquation.indexOf(clog2Expression);
    while (functionIndex != -1)
    {
        QRegularExpressionMatch matchedFunction = clog2Expression.match(equation.at(functionIndex));
        QString argument = matchedFunction.captured(1);

        solvedEquation.replace(functionIndex, solveClog2(argument));

        functionIndex = solvedEquation.indexOf(clog2Expression);
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
    QRegularExpression parenthesesExpression("[()]");

    int position = parenthesesStart + 1;
    int depth = 1;

    while (depth > 0)
    {
        position = equation.indexOf(parenthesesExpression, position);
        if (parenthesesExpression.match(equation.at(position)).captured() == "(")
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
    QRegularExpression power("[/*][/*]");
    return solveBinaryOperationsFromLeftToRight(equation, power);
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::parseMultiplyAndDivide()
//-----------------------------------------------------------------------------
QStringList SystemVerilogExpressionParser::solveMultiplyAndDivide(QStringList const& equation) const
{
    QRegularExpression multiplyDivide("[/*//]");
    return solveBinaryOperationsFromLeftToRight(equation, multiplyDivide);
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::solveAdditionAndSubtraction()
//-----------------------------------------------------------------------------
QStringList SystemVerilogExpressionParser::solveAdditionAndSubtraction(QStringList const& equation) const
{
    QRegularExpression addSubtract("[+-]");
    return solveBinaryOperationsFromLeftToRight(equation, addSubtract);
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::solveBinaryOperations()
//-----------------------------------------------------------------------------
QStringList SystemVerilogExpressionParser::solveBinaryOperationsFromLeftToRight(QStringList const& equation, 
    QRegularExpression const& binaryOperator) const
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

    if (!firstTerm.contains('.') && (operation == "/" || (operation == "**" && rightOperand < 0)))
    {
        int integerResult = result;
        return QString::number(integerResult);
    }

    else
    {
        qint64 resultInInt = result;
        QString baseNumberString = QString::number(resultInInt);

        qreal decimalPart = abs(result - resultInInt);

        int decimalPrecision = getDecimalPrecision(firstTerm, secondTerm);
        QString decimalString = QString::number(decimalPart, 'g', decimalPrecision);

        decimalString = decimalString.right(decimalString.size() - 1);

        QString resultInString = baseNumberString + decimalString;
        return resultInString;
    }
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::parseConstant()
//-----------------------------------------------------------------------------
qreal SystemVerilogExpressionParser::parseConstantToDecimal(QString const& constantNumber) const
{
    QRegularExpression size("([1-9][0-9_]*)?(?=')");
    QRegularExpression baseFormat("'" + SystemVerilogSyntax::SIGNED + "[dDbBoOhH]?");

	// Remove formating off the number.
    QString result = constantNumber;
    result.remove(size);
    result.remove(baseFormat);
    result.remove('_');

	// True = 1 and False = 0.
	result.replace( QString( "true" ), QString( "1" ), Qt::CaseInsensitive );
	result.replace( QString( "false" ), QString( "0" ), Qt::CaseInsensitive );

	// Number containing a dot is interpreted as a floating number. Otherwise it is seen as an integer.
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
// Function: SystemVerilogExpressionParser::getDecimalPrecision()
//-----------------------------------------------------------------------------
int SystemVerilogExpressionParser::getDecimalPrecision(QString const& firstTerm, QString const& secondTerm) const
{
    int precision = 0;
    if (firstTerm.contains('.') || secondTerm.contains('.'))
    {
        int firstDecimalLength = 0;
        int secondDecimalLength = 0;

        if (firstTerm.contains('.'))
        {
            QStringList firstList = firstTerm.split('.');
            if (firstList.size() == 2)
            {
                firstDecimalLength = firstList.at(1).size();
            }
        }
        if (secondTerm.contains('.'))
        {
            QStringList secondList = secondTerm.split('.');
            if (secondList.size() == 2)
            {
                secondDecimalLength = secondList.at(1).size();
            }
        }

        precision = qMax(firstDecimalLength, secondDecimalLength);
    }

    return precision;
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::getBaseForNumber()
//-----------------------------------------------------------------------------
int SystemVerilogExpressionParser::getBaseForNumber(QString const& constantNumber) const
{
    QRegularExpression baseFormat("'" + SystemVerilogSyntax::SIGNED + "([dDbBoOhH]?)");
    QString base = baseFormat.match(constantNumber).captured(1);

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
