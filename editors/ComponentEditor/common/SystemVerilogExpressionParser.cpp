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


namespace
{
    const QString CLOG2_FUNCTION = "[$]clog2[(](?:[^)]+)[)]";

	const QString BOOLEAN_VALUE = "(?i)true|(?i)false";

    const QString REAL_NUMBER("[+-]?[0-9_]+[.][0-9_]+");

    const QRegularExpression PRIMARY_LITERAL("((?:[(]\\s*)*)\\s*(" + REAL_NUMBER + "|" + BOOLEAN_VALUE + "|"
		+ SystemVerilogSyntax::INTEGRAL_NUMBER + "|"  + CLOG2_FUNCTION + ")\\s*((?:[)]\\s*)*)");

    const QRegularExpression BINARY_OPERATOR("[/*][/*]|[-+/*//]");
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
        return QStringLiteral("x");
    }

    if (isStringLiteral(expression))
    {
        return expression.trimmed();
    }
    
    if (isArrayExpression(expression))
    {
        return  parseArray(expression);
    }

    if (isComparison(expression))
    {
        return parseComparison(expression);
    }

    QStringList equation = toStringList(expression);

    solveMathFuctions(equation);
    solveExpressionsInParentheses(equation);
    solvePower(equation);
    solveMultiplyAndDivide(equation);
    solveAdditionAndSubtraction(equation);

    if (equation.first().compare(QLatin1String("x")) == 0)
    {
        return QStringLiteral("x");
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
    static QRegularExpression comparisonExpression (".+" + COMPARISON_OPERATOR.pattern() + ".+");
    return comparisonExpression.match(expression).hasMatch();
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
    foreach(QString const& term, toStringList(expression))
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
bool SystemVerilogExpressionParser::isStringLiteral(QString const& expression) const
{
    static QRegularExpression stringExpression("^\\s*" + SystemVerilogSyntax::STRING_LITERAL + "\\s*$");
    return stringExpression.match(expression).hasMatch();
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
    static QRegularExpression literalExpression("^\\s*[(]*\\s*(" + SystemVerilogSyntax::INTEGRAL_NUMBER + "|" +
        REAL_NUMBER + ")\\s*[)]*\\s*$");

    return literalExpression.match(expression).hasMatch();
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
        selectedOperator = COMPARISON_OPERATOR.match(partialComparison).captured();
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

    if ((selectedOperator.compare(QLatin1String(">")) == 0 && leftResult > rightResult) ||
        (selectedOperator.compare(QLatin1String("<")) == 0 && leftResult < rightResult) ||
        (selectedOperator.compare(QLatin1String("==")) == 0 && leftResult == rightResult) ||
        (selectedOperator.compare(QLatin1String(">=")) == 0 && leftResult >= rightResult) ||
        (selectedOperator.compare(QLatin1String("<=")) == 0 && leftResult <= rightResult) ||
        (selectedOperator.compare(QLatin1String("!=")) == 0 && leftResult != rightResult))
    {
        return QStringLiteral("1");
    }

    return QStringLiteral("0");
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::parseArray()
//-----------------------------------------------------------------------------
QString SystemVerilogExpressionParser::parseArray(QString const& expression) const
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
//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::toStringList()
//-----------------------------------------------------------------------------
QStringList SystemVerilogExpressionParser::toStringList(QString const& expression) const
{
    QStringList equationList;

    static QRegularExpression splitter("([$]clog2[(]|[()]|" +  COMBINED_OPERATOR.pattern() + ")");
    
    QRegularExpressionMatch match = splitter.match(expression);
    int pos = 0;
    while (match.hasMatch() && pos != -1)
    {
        int len = match.capturedStart() - pos;

        QString operation = match.captured();
        if (operation.compare(QStringLiteral("(")) == 0 || operation.compare(QStringLiteral("$clog2(")) == 0)
        {
            int start = match.capturedStart() + operation.length();
            int end = findMatchingEndParenthesis(expression, start - 1);
            if (end < start)
            {
                end = expression.length() - 1;
            }

            QString term = expression.mid(start, end - start).trimmed();

            equationList.append(operation);
            equationList.append(toStringList(term));
            equationList.append(")");

            pos = end + 1;
        }
        else
        {
            QString term = expression.mid(pos, len).trimmed();

            if (!term.isEmpty())
            {
                equationList.append(term);
            }  
            else if (equationList.isEmpty() || equationList.last().compare(")") != 0)
            {
                int prevPos = match.capturedStart();

                pos = match.capturedEnd();
                match = splitter.match(expression, pos);

                term = expression.mid(prevPos, match.capturedStart() - prevPos);
                equationList.append(term);
            }

            equationList.append(match.captured());
            pos = match.capturedEnd();
        }

        match = splitter.match(expression, pos);
    }

    equationList.append(expression.mid(pos).trimmed());

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
void SystemVerilogExpressionParser::solveMathFuctions(QStringList& equation) const
{
    QRegularExpression mathFunctionExpression("[$]clog2[(]");

    int functionIndex = equation.indexOf(mathFunctionExpression);
    while (functionIndex != -1)
    {
        int functionEnd = findMatchingEndParenthesis(equation, functionIndex);

        QString functionArgument;
        int argumentCount = functionEnd - functionIndex -1;
        for (int i = 0; i < argumentCount; i++)
        {
            functionArgument.append(equation.takeAt(functionIndex + 1));
        }        

        equation.replace(functionIndex, solveClog2(functionArgument));
        equation.removeAt(functionIndex + 1); //<! Remove closing parenthesis.

        functionIndex = equation.indexOf(mathFunctionExpression);
    }
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::solveClog2()
//-----------------------------------------------------------------------------
QString SystemVerilogExpressionParser::solveClog2(QString const& value) const
{
    qreal quotient = parseConstantToDecimal(parseExpression(value));
    
    if (quotient < 0)
    {
        return QStringLiteral("x");
    }
    else if (quotient == 1)
    {
        return QStringLiteral("1");
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
void SystemVerilogExpressionParser::solveExpressionsInParentheses(QStringList& equation) const
{
    int position = 0;
    while(equation.indexOf("(", position) != -1)
    {
        int parenthesesStart = equation.indexOf("(", position);
        int parenthesesEnd = findMatchingEndParenthesis(equation, parenthesesStart);

        int itemsInsideParenthesis = parenthesesEnd - parenthesesStart - 1;

        QString expressionInParentheses;
        for (int i = 0; i < itemsInsideParenthesis; i++)
        {
            expressionInParentheses.append(equation.takeAt(parenthesesStart + 1));
        }

        equation.replace(parenthesesStart, parseExpression(expressionInParentheses));
        equation.takeAt(parenthesesStart + 1); //<! Remove closing parenthesis.

        position = parenthesesStart + 1;
    }
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
// Function: SystemVerilogExpressionParser::findMatchingEndParenthesis()
//-----------------------------------------------------------------------------
int SystemVerilogExpressionParser::findMatchingEndParenthesis(QString const& equation, int parenthesesStart) const
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
void SystemVerilogExpressionParser::solvePower(QStringList& equation) const
{
    QRegularExpression power("[/*][/*]");
    solveBinaryOperationsFromLeftToRight(equation, power);
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::parseMultiplyAndDivide()
//-----------------------------------------------------------------------------
void SystemVerilogExpressionParser::solveMultiplyAndDivide(QStringList& equation) const
{
    QRegularExpression multiplyDivide("[/*//]");
    solveBinaryOperationsFromLeftToRight(equation, multiplyDivide);
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::solveAdditionAndSubtraction()
//-----------------------------------------------------------------------------
void SystemVerilogExpressionParser::solveAdditionAndSubtraction(QStringList& equation) const
{
    QRegularExpression addSubtract("[+-]");
    solveBinaryOperationsFromLeftToRight(equation, addSubtract);
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::solveBinaryOperations()
//-----------------------------------------------------------------------------
void SystemVerilogExpressionParser::solveBinaryOperationsFromLeftToRight(QStringList& equation, 
    QRegularExpression const& binaryOperator) const
{
    QString operation;
    QString operand1;
    QString operand2;

    while(equation.indexOf(binaryOperator) != -1)
    {
        int operatorPosition = equation.indexOf(binaryOperator);
        int firstOperandPosition = operatorPosition - 1;
        int secondOperandPosition = operatorPosition + 1;

        operation = equation.at(operatorPosition);
        operand1 = equation.at(firstOperandPosition);
        operand2 = equation.at(secondOperandPosition);

        QString result = solve(operand1, operation, operand2);

        equation.replace(firstOperandPosition, result);
        equation.removeAt(secondOperandPosition);
        equation.removeAt(operatorPosition);
    }

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

    if (operation.compare(QLatin1String("+")) == 0)
    {
        result = leftOperand + rightOperand;
    }
    else if (operation.compare(QLatin1String("-")) == 0)
    {
        result = leftOperand - rightOperand;
    }
    else if (operation.compare(QLatin1String("*")) == 0)
    {
        result = leftOperand*rightOperand;
    }
    else if (operation.compare(QLatin1String("**")) == 0)
    {
        if (leftOperand == 0 && rightOperand < 0)
        {
            return QStringLiteral("x");
        }

        result = qPow(leftOperand, rightOperand);
    }
    else if (operation.compare(QLatin1String("/")) == 0)
    {
        if (rightOperand == 0)
        {
            return QStringLiteral("x");
        }

        result = leftOperand/rightOperand;
    }

    if (!firstTerm.contains('.') && 
        (operation.compare(QLatin1String("/")) == 0 || 
        (operation.compare(QLatin1String("**")) == 0 && rightOperand < 0)))
    {
        int integerResult = result;
        return QString::number(integerResult);
    }
    else
    {
        qint64 integerResult = result;
        QString integerPart = QString::number(integerResult);

        qreal decimals = qAbs(result - integerResult);

        QString decimalPart = QString::number(decimals, 'g', getDecimalPrecision(firstTerm, secondTerm));

        decimalPart = decimalPart.right(decimalPart.size() - 1);

        QString resultInString = integerPart + decimalPart;
        return resultInString;
    }
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::parseConstant()
//-----------------------------------------------------------------------------
qreal SystemVerilogExpressionParser::parseConstantToDecimal(QString const& constantNumber) const
{
    static QRegularExpression size("([1-9][0-9_]*)?(?=')");
    static QRegularExpression baseFormat("'" + SystemVerilogSyntax::SIGNED + "[dDbBoOhH]?");

	// Remove formating off the number.
    QString result = constantNumber;
    result.remove(size);
    result.remove(baseFormat);
    result.remove('_');

	// Number containing a dot is interpreted as a floating number. Otherwise it is seen as an integer.
    if (constantNumber.compare(QStringLiteral("true"), Qt::CaseInsensitive) == 0)
    {
        return 1;
    }
    else if (constantNumber.compare(QStringLiteral("false"), Qt::CaseInsensitive) == 0)
    {
        return 0;
    }
    else if (constantNumber.contains('.'))
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
    int firstDecimalLength = 0;
    int secondDecimalLength = 0;

    QStringList firstList = firstTerm.split('.');
    if (firstList.size() == 2)
    {
        firstDecimalLength = firstList.at(1).size();
    }

    QStringList secondList = secondTerm.split('.');
    if (secondList.size() == 2)
    {
        secondDecimalLength = secondList.at(1).size();
    }

    return qMax(firstDecimalLength, secondDecimalLength);
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::getBaseForNumber()
//-----------------------------------------------------------------------------
int SystemVerilogExpressionParser::getBaseForNumber(QString const& constantNumber) const
{
    QRegularExpression baseFormat("'" + SystemVerilogSyntax::SIGNED + "([dDbBoOhH]?)");

    return baseForFormat(baseFormat.match(constantNumber).captured(1));
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::baseForFormat()
//-----------------------------------------------------------------------------
int SystemVerilogExpressionParser::baseForFormat(QString const& baseFormat) const
{
    if (baseFormat.isEmpty() || baseFormat.compare(QLatin1String("d"), Qt::CaseInsensitive) == 0)
    {
        return 10;
    }
    else if (baseFormat.compare(QLatin1String("h"), Qt::CaseInsensitive) == 0)
    {
        return 16;
    }
    else if (baseFormat.compare(QLatin1String("o"), Qt::CaseInsensitive) == 0)
    {
        return  8;
    }
    else if (baseFormat.compare(QLatin1String("b"), Qt::CaseInsensitive) == 0)
    {
        return 2;
    }

    return 0;
}
