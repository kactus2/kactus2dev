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

#include <QDebug>

#include <algorithm>

namespace
{
	const QString BOOLEAN_VALUE = "(?i)true|(?i)false";

    const QString REAL_NUMBER("[-+]?[0-9_]+[.][0-9_]+");

    const QRegularExpression PRIMARY_LITERAL("(" + REAL_NUMBER + "|" + BOOLEAN_VALUE + "|"
		+ SystemVerilogSyntax::INTEGRAL_NUMBER + ")");

    const QRegularExpression BINARY_OPERATOR("[/*][/*]|[-+/*//]|[<>]=?|[!=]=|[$]pow");

    const QRegularExpression UNARY_OPERATOR("[$]clog2");

    const QChar OPEN_PARENTHESIS('(');

    const QChar CLOSE_PARENTHESIS(')');

    const QRegularExpression ANY_OPERATOR(BINARY_OPERATOR.pattern() + "|" + UNARY_OPERATOR.pattern());

}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::SystemVerilogExpressionParser()
//-----------------------------------------------------------------------------
SystemVerilogExpressionParser::SystemVerilogExpressionParser(): op_precedence()
{
    op_precedence["{"] = 1;
    op_precedence["}"] = 1;
    op_precedence["<"] = 1;
    op_precedence[">"] = 1;
    op_precedence["<="] = 1;
    op_precedence[">="] = 1;
    op_precedence["=="] = 1;
    op_precedence["+"] = 2;
    op_precedence["-"] = 2;
    op_precedence["*"] = 3;
    op_precedence["/"] = 3;
    op_precedence["**"] = 4;
    op_precedence["$clog2"] = 5;
    op_precedence["$pow"] = 5;
    op_precedence["("] = 6;
    op_precedence[")"] = 6;

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
QString SystemVerilogExpressionParser::parseExpression(QString const& expression, bool* validExpression) const
{
    return solveRPN(toRPN(expression), validExpression);
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::toRPN()
//-----------------------------------------------------------------------------
QStringList SystemVerilogExpressionParser::toRPN(QString const& expression) const
{
    // Convert expression to Reverse Polish Notation (RPN) using the Shunting Yard algorithm.
    // RPN is used to ensure the operations are calculated in the correct precedence order.
    QStringList output;
    QStringList stack;

    int openArrays = 0;
    int openParenthesis = 0;
    bool lastWasOperator = true;
    for (int index = 0; index < expression.size(); )
    {
        if (expression.at(index).isSpace())
        {
            index++;
            continue;
        }

        QRegularExpressionMatch operatorMatch = ANY_OPERATOR.match(expression, index,
            QRegularExpression::NormalMatch, QRegularExpression::AnchoredMatchOption);
        QRegularExpressionMatch literalMatch = PRIMARY_LITERAL.match(expression, index,
            QRegularExpression::NormalMatch, QRegularExpression::AnchoredMatchOption);

        if (lastWasOperator && literalMatch.hasMatch())
        {
            output.append(literalMatch.captured());
            
            index = literalMatch.capturedEnd();
            lastWasOperator = false;
        }
        else if (operatorMatch.hasMatch())
        {
            while (stack.size() > 0 and
                   op_precedence[stack.last()] >= op_precedence[operatorMatch.captured()] and
                   stack.last() != "(")
            {
                output.append(stack.takeLast());
            }

            stack.append(operatorMatch.captured());

            index = operatorMatch.capturedEnd();
            lastWasOperator = true;
        }
        else if (expression.at(index) == '{')
        {
            output.append(expression.at(index));

            index++;
            openArrays++;
            lastWasOperator = true;
        }
        else if (expression.at(index) == '}')
        {
            while (stack.size() > 0 && stack.last() != "{")
            {
                output.append(stack.takeLast());
            }

            if (stack.size() != 0)
            {
                output.append(stack.takeLast());
            }

            stack.append(expression.at(index));
            
            openArrays--;
            lastWasOperator = true;
            index++;
        }
        else if (expression.at(index) == ',')
        {
            while (stack.size() > 0 && (stack.last() != "{" && stack.last() != "("))
            {
                output.append(stack.takeLast());
            }

            lastWasOperator = true;
            index++;
        }
        else if (expression.at(index) == '(')
        {
            stack.append(expression.at(index));
            lastWasOperator = true;
            index++;
            openParenthesis++;
        }
        else if (expression.at(index) == ')')
        {
            if (openParenthesis <= 0)
            {
                return QStringList("x");
            }

            while (stack.size() > 0 && (stack.last() != "("))
            {
                output.append(stack.takeLast());
            }

            if (stack.isEmpty() == false)
            {
                stack.takeLast();
            }

            openParenthesis--;
            lastWasOperator = false;
            index++;
        }
        else
        {
            QRegularExpression separator(ANY_OPERATOR.pattern() + "|[(){}]");
            QString unknown = expression.mid(index, separator.match(expression, index).capturedStart() - index);
            output.append(unknown.trimmed());
            index += unknown.length();
            lastWasOperator = false;
        }
    }

    if (openParenthesis != 0 || openArrays != 0)
    {
        return QStringList("x");
    }

    while (stack.isEmpty() == false)
    {
        output.append(stack.takeLast());
    }

    return output;
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::solveRPN()
//-----------------------------------------------------------------------------
QString SystemVerilogExpressionParser::solveRPN(QStringList const& rpn, bool* validExpression) const
{
    //qDebug() << rpm;
    bool isValid = true;
    QStringList resultStack;

    for (QString const& token : rpn)
    {
        QRegularExpressionMatch match = PRIMARY_LITERAL.match(token, 0, QRegularExpression::NormalMatch,
            QRegularExpression::AnchoredMatchOption);

        if (match.hasMatch())
        {
            resultStack.append(parseConstant(match.captured()));
        }
        else if (isBinaryOperator(token))
        {
            if (resultStack.size() < 2)
            {
                isValid = false;
                break;
            }

            resultStack.append(solveBinary(token, resultStack.takeLast(), resultStack.takeLast()));
        }
        else if (isUnaryOperator(token))
        {
            if (resultStack.size() < 1)
            {
                isValid = false;
                break;
            }

            resultStack.append(solveUnary(token, resultStack.takeLast()));
        }
        else if (token.compare("{") == 0)
        {
            resultStack.append(token);
        }
        else if (token.compare("}") == 0)
        {
            QString asArray;
            while (resultStack.size() > 0 && resultStack.last() != "{")
            {
                asArray.prepend(",");
                asArray.prepend(resultStack.takeLast());
            }

            if (resultStack.size() == 0)
            {
                isValid = false;
                break;
            }
            else
            {
                asArray.prepend(resultStack.takeLast());
            }

            asArray.chop(1);
            asArray.append(token);
            resultStack.append(asArray);
        }
        else if (isSymbol(token))
        {
            resultStack.append(findSymbolValue(token));
        }
        else
        {
            isValid = false;
            break;
        }
    }

    if (validExpression != nullptr)
    {
        *validExpression = isValid;
    }

    return resultStack.join(QString());
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
    const QStringList expressionList = toRPN(expression);
    for (QStringList::const_iterator it = expressionList.constBegin(); 
        it != expressionList.constEnd(); ++it)
    {
        if (isLiteral(*it))
        {
            greatestBase = qMax(greatestBase, getBaseForNumber(*it));
        }
        else if (isSymbol(*it))
        {
            greatestBase = qMax(greatestBase, getBaseForNumber(findSymbolValue(*it)));
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
// Function: SystemVerilogExpressionParser::isUnaryOperator()
//-----------------------------------------------------------------------------
bool SystemVerilogExpressionParser::isUnaryOperator(QString const& token) const
{
    return UNARY_OPERATOR.match(token, 0, QRegularExpression::NormalMatch,
        QRegularExpression::AnchoredMatchOption).hasMatch();
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::isBinaryOperator()
//-----------------------------------------------------------------------------
bool SystemVerilogExpressionParser::isBinaryOperator(QString const& token) const
{
    return BINARY_OPERATOR.match(token, 0, QRegularExpression::NormalMatch,
        QRegularExpression::AnchoredMatchOption).hasMatch();
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::solveBinary()
//-----------------------------------------------------------------------------
QString SystemVerilogExpressionParser::solveBinary(QString const& operation, QString const& leftTerm,
    QString const& rightTerm) const
{
    qreal leftOperand = parseConstantToDecimal(leftTerm);
    qreal rightOperand = parseConstantToDecimal(rightTerm);

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
    else if (operation.compare(QLatin1String("**")) == 0 or operation.compare(QLatin1String("$pow")) == 0)
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

    if ((operation.compare(QLatin1String(">")) == 0 && leftOperand > rightOperand) ||
        (operation.compare(QLatin1String("<")) == 0 && leftOperand < rightOperand) ||
        (operation.compare(QLatin1String("==")) == 0 && leftOperand == rightOperand) ||
        (operation.compare(QLatin1String(">=")) == 0 && leftOperand >= rightOperand) ||
        (operation.compare(QLatin1String("<=")) == 0 && leftOperand <= rightOperand) ||
        (operation.compare(QLatin1String("!=")) == 0 && leftOperand != rightOperand))
    {
        return QStringLiteral("1");
    }

    if (!leftTerm.contains('.') &&
        (operation.compare(QLatin1String("/")) == 0 || 
        (operation.compare(QLatin1String("**")) == 0 && rightOperand < 0)))
    {
        int integerResult = result;
        return QString::number(integerResult);
    }
    else
    {
        return QString::number(result, 'f', getDecimalPrecision(leftTerm, rightTerm));
    }
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::solveUnary()
//-----------------------------------------------------------------------------
QString SystemVerilogExpressionParser::solveUnary(QString const& operation, QString const& term) const
{
    if (operation.compare(QLatin1String("$clog2")) == 0)
    {
        return solveClog2(term);
    }

    return QStringLiteral("x");
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::solveClog2()
//-----------------------------------------------------------------------------
QString SystemVerilogExpressionParser::solveClog2(QString const& value) const
{
    qreal quotient = value.toLongLong();

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
// Function: SystemVerilogExpressionParser::parseConstant()
//-----------------------------------------------------------------------------
QString SystemVerilogExpressionParser::parseConstant(QString const& constantNumber) const
{
	// Number containing a dot is interpreted as a floating number. Otherwise it is seen as an integer.
    if (constantNumber.contains(QRegularExpression("\\s*true\\s*", QRegularExpression::CaseInsensitiveOption)))
    {
        return "1";
    }
    else if (constantNumber.contains(QRegularExpression("\\s*false\\s*", QRegularExpression::CaseInsensitiveOption)))
    {
        return "0";
    }
    
    static QRegularExpression size("([1-9][0-9_]*)?(?=')");
    static QRegularExpression baseFormat("'" + SystemVerilogSyntax::SIGNED + "[dDbBoOhH]?");

	// Remove formating off the number.
    QString result = constantNumber;
    result.remove(size);
    result.remove(baseFormat);
    result.remove('_');
   
    if (constantNumber.contains('.'))
    {
        return constantNumber;
    }
    else
    {
        return QString::number(result.toLongLong(0, getBaseForNumber(constantNumber)));
    }
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::isSymbol()
//-----------------------------------------------------------------------------
bool SystemVerilogExpressionParser::isSymbol(QString const& /*expression*/) const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::findSymbolValue()
//-----------------------------------------------------------------------------
QString SystemVerilogExpressionParser::findSymbolValue(QString const& expression) const
{
    return expression;
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
