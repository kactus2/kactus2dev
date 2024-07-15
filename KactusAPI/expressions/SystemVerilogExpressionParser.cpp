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
#include <QMap>
#include <QStringBuilder>
#include <QStringList>
#include <QVector>

#include <qmath.h>

#include <algorithm>

namespace
{
    const QRegularExpression PRIMARY_LITERAL(SystemVerilogSyntax::REAL_NUMBER % QStringLiteral("|") %
        SystemVerilogSyntax::INTEGRAL_NUMBER % QStringLiteral("|") %
        SystemVerilogSyntax::BOOLEAN_VALUE % QStringLiteral("|") %
        SystemVerilogSyntax::STRING_LITERAL);

    const QRegularExpression BINARY_OPERATOR(QStringLiteral("[/%^+-]|<<|>>|<=?|>=?|!==?|===?|[&|*]{1,2}|[$]pow"));

    const QRegularExpression UNARY_OPERATOR(QStringLiteral(
        "[$]clog2|[$]exp|[$]sqrt|[$]ipxact_mode_condition|[$]ipxact_port_value|[$]ipxact_field_value|~|`"));

    const QRegularExpression TERNARY_OPERATOR(QStringLiteral("[?:]"));

    const QLatin1Char OPEN_PARENTHESIS('(');
    const QLatin1Char CLOSE_PARENTHESIS(')');
    const QLatin1Char OPEN_ARRAY('{');
    const QLatin1Char CLOSE_ARRAY('}');

    const QString OPEN_PARENTHESIS_STRING(QStringLiteral("("));
    const QString OPEN_ARRAY_STRING(QStringLiteral("{"));
    const QString CLOSE_ARRAY_STRING(QStringLiteral("}"));

    const QString TERNARY_QUESTION_STRING(QStringLiteral("?"));
    const QString TERNARY_COLON_STRING(QStringLiteral(":"));

    const QRegularExpression ANY_OPERATOR(
        BINARY_OPERATOR.pattern() % QStringLiteral("|") %
        UNARY_OPERATOR.pattern() % QStringLiteral("|") %
        TERNARY_OPERATOR.pattern());

    const QRegularExpression LITERAL_EXPRESSION(QStringLiteral("^\\s*(") % SystemVerilogSyntax::INTEGRAL_NUMBER%
        QStringLiteral("|") % SystemVerilogSyntax::REAL_NUMBER% QStringLiteral(")\\s*$"));

    const QRegularExpression BASE_FORMAT(QStringLiteral("'[sS]?([dDbBoOhH]?)"));
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::parseExpression()
//-----------------------------------------------------------------------------
QString SystemVerilogExpressionParser::parseExpression(QStringView expression, bool* validExpression) const
{
    // Copy of expression needs to be created for replacing unary minuses with special character.
    QString expressionCopy = expression.toString();
    return solveRPN(convertToRPN(expressionCopy), validExpression);
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::isArrayExpression()
//-----------------------------------------------------------------------------
bool SystemVerilogExpressionParser::isArrayExpression(QStringView expression) const noexcept
{
    return expression.contains(OPEN_ARRAY) && expression.contains(CLOSE_ARRAY);
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::isPlainValue()
//-----------------------------------------------------------------------------
bool SystemVerilogExpressionParser::isPlainValue(QStringView expression) const
{
    return expression.isEmpty() || isLiteral(expression) || isStringLiteral(expression);
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::baseForExpression()
//-----------------------------------------------------------------------------
int SystemVerilogExpressionParser::baseForExpression(QStringView expression) const
{
    int greatestBase = 0;
    QString asStr = expression.toString();
    for (auto const& token : convertToRPN(asStr))
    {
        if (isLiteral(token))
        {
            greatestBase = qMax(greatestBase, baseOf(token));
        }
        else if (isSymbol(token))
        {
            greatestBase = qMax(greatestBase, getBaseForSymbol(token));
        }
    }

    return greatestBase;
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::convertToRPN()
//-----------------------------------------------------------------------------
QVector<QStringView> SystemVerilogExpressionParser::convertToRPN(QString& expression)
{
    // Convert expression to Reverse Polish Notation (RPN) using the Shunting Yard algorithm.
    QVector<QStringView> output;
    QVector<QStringView> stack;

    int openParenthesis = 0;
    bool nextMayBeLiteral = true;

    qsizetype previousIndex = -1;
    const auto SIZE = expression.size();

    // Convert unary minuses to backticks. Needs to be done before main loop, otherwise string views will be invalidated.
    // SolveRPN knows to treat backticks as unary minus.
    for (qsizetype index = 0; index < SIZE; ++index)
    {
        const QChar current = expression.at(index);
        if (current.isSpace())
        {
            continue;
        }

        // Minus == unary minus if preceded by opening parenthesis or if first token
        if (current == QLatin1Char('-') && (previousIndex == -1 || expression.at(previousIndex) == OPEN_PARENTHESIS_STRING))
        {
            expression[index] = QLatin1Char('`');
        }

        previousIndex = index;
    }

    for (qsizetype index = 0; index < SIZE; /*index incremented inside loop*/)
    {
        const QChar current = expression.at(index);
        if (current.isSpace())
        {
            ++index;
            continue;
        }

        if (QRegularExpressionMatch literalMatch = PRIMARY_LITERAL.match(expression, index,
            QRegularExpression::NormalMatch, QRegularExpression::AnchorAtOffsetMatchOption);
            nextMayBeLiteral && literalMatch.hasMatch())
        {
            output.append(literalMatch.capturedView());

            index += literalMatch.capturedLength();
            nextMayBeLiteral = false;
        }
        else if (QRegularExpressionMatch operatorMatch = ANY_OPERATOR.match(expression, index,
                 QRegularExpression::NormalMatch, QRegularExpression::AnchorAtOffsetMatchOption); 
            operatorMatch.hasMatch())
        {
            const auto operation = operatorMatch.capturedView();
            while (stack.isEmpty() == false &&
                stack.last() != OPEN_PARENTHESIS_STRING &&
                stack.last() != TERNARY_QUESTION_STRING &&
                operatorPrecedence(stack.last()) >= operatorPrecedence(operation))
            {
                output.append(stack.takeLast());
            }

            stack.append(operation);

            index += operation.length();
            nextMayBeLiteral = true;
        }
        else if (current == OPEN_PARENTHESIS)
        {
            stack.append(OPEN_PARENTHESIS_STRING);
            ++index;
            ++openParenthesis;
            nextMayBeLiteral = true;
        }
        else if (current == CLOSE_PARENTHESIS)
        {
            while (stack.isEmpty() == false && (stack.last() != OPEN_PARENTHESIS_STRING))
            {
                output.append(stack.takeLast());
            }

            if (stack.isEmpty() == false)
            {
                stack.removeLast(); //!< Remove open parenthesis.
            }

            ++index;
            --openParenthesis;
            nextMayBeLiteral = false;
        }
        else if (current == OPEN_ARRAY)
        {
            output.append(OPEN_ARRAY_STRING);
            ++index;
            ++openParenthesis;
            nextMayBeLiteral = true;
        }
        else if (current == CLOSE_ARRAY)
        {
            while (stack.isEmpty() == false && stack.last() != OPEN_ARRAY_STRING)
            {
                output.append(stack.takeLast());
            }

            if (stack.isEmpty() == false)
            {
                output.append(stack.takeLast()); //!< Add open array.
            }
            stack.append(CLOSE_ARRAY_STRING); //!< Add close array.

            ++index;
            --openParenthesis;
            nextMayBeLiteral = false;
        }
        else if (current == QLatin1Char(','))
        {
            while (stack.isEmpty() == false &&
                (stack.last() != OPEN_ARRAY_STRING && stack.last() != OPEN_PARENTHESIS_STRING))
            {
                output.append(stack.takeLast());
            }

            ++index;
            nextMayBeLiteral = true;
        }
        else
        {
            static const QRegularExpression separator(ANY_OPERATOR.pattern() % QStringLiteral("|[(){},]"));
            auto unknown = QStringView(expression);
            unknown = unknown.mid(index, separator.match(expression, index).capturedStart() - index);

            output.append(unknown.trimmed());

            index += unknown.length();
            nextMayBeLiteral = false;
        }
    }

    if (openParenthesis != 0)
    {
        output.clear();
        output.append(QStringLiteral("x"));
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
QString SystemVerilogExpressionParser::solveRPN(QVector<QStringView> rpn, bool* validExpression) const
{
    QStringList result;
    bool isWellFormed = true;
    int ternaryCount = 0;

    for (auto const& token : rpn)
    {
        if (isUnaryOperator(token))
        {
            if (result.isEmpty())
            {
                isWellFormed = false;
            }
            else
            {
                result.append(solveUnary(token, result.takeLast()));
            }
        }
        else if (isBinaryOperator(token))
        {
            if (result.size() < 2)
            {
                isWellFormed = false;
            }
            else
            {
                result.append(solveBinary(token, result.takeLast(), result.takeLast()));
            }
        }
        else if (isTernaryOperator(token))
        {
            if (token.compare(TERNARY_COLON_STRING) == 0)
            {
                ++ternaryCount;

                if (result.size() < 3)
                {
                    isWellFormed = false;
                }
                else
                {
                    result.append(solveTernary(result.takeLast(), result.takeLast(), result.takeLast()));
                }
            }
            else
            {
                --ternaryCount;
            }
        }
        else if (token.compare(OPEN_ARRAY_STRING) == 0)
        {
            result.append(token.toString());
        }
        else if (token.compare(CLOSE_ARRAY_STRING) == 0)
        {
            QStringList items;
            while (result.isEmpty() == false && result.last().compare(OPEN_ARRAY_STRING) != 0)
            {
                items.prepend(result.takeLast());
            }

            if (result.isEmpty())
            {
                isWellFormed = false;
            }
            else
            {
                QString arrayItem(result.takeLast() % items.join(QLatin1Char(',')) % token);
                result.append(arrayItem);
            }
        }
        else if (token.compare(QLatin1String("true"), Qt::CaseInsensitive) == 0)
        {
            result.append(QStringLiteral("1"));
        }
        else if (token.compare(QLatin1String("false"), Qt::CaseInsensitive) == 0)
        {
            result.append(QStringLiteral("0"));
        }
        else if (isStringLiteral(token))
        {
            result.append(token.toString());
        }
        else if (isSymbol(token))
        {
            result.append(findSymbolValue(token));
        }
        else
        {
            const auto constant = parseConstant(token);

            if (constant == QLatin1String("x"))
            {
                isWellFormed = false;
            }

            result.append(constant);
        }

        if (isWellFormed == false)
        {
            break;
        }
    }

    if (validExpression != nullptr)
    {
        *validExpression = (isWellFormed && result.contains(QLatin1String("x")) == false && ternaryCount == 0);
    }

    return result.join(QString());
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::isStringLiteral()
//-----------------------------------------------------------------------------
bool SystemVerilogExpressionParser::isStringLiteral(QStringView expression) const noexcept
{
    return expression.startsWith(QLatin1Char('"')) && expression.endsWith(QLatin1Char('"'));
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::isLiteral()
//-----------------------------------------------------------------------------
bool SystemVerilogExpressionParser::isLiteral(QStringView expression) const
{
    return LITERAL_EXPRESSION.match(expression).hasMatch();
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::isUnaryOperator()
//-----------------------------------------------------------------------------
bool SystemVerilogExpressionParser::isUnaryOperator(QStringView token) const
{
    return UNARY_OPERATOR.match(token).hasMatch();
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::isBinaryOperator()
//-----------------------------------------------------------------------------
bool SystemVerilogExpressionParser::isBinaryOperator(QStringView token) const
{
    //! Check for operator length so e.g. negative numbers are not interpret as operators.
    auto match = BINARY_OPERATOR.match(token);
    return match.hasMatch() && match.capturedLength() == token.length();
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::isTernaryOperator()
//-----------------------------------------------------------------------------
bool SystemVerilogExpressionParser::isTernaryOperator(QStringView token) const
{
    return TERNARY_OPERATOR.match(token).hasMatch();
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::solveTernary()
//-----------------------------------------------------------------------------
QString SystemVerilogExpressionParser::solveTernary(QStringView condition,
    QString const& trueCase, QString const& falseCase) const
{
    if (condition.toDouble() != 0)
    {
        return trueCase;
    }
    else
    {
        return falseCase;
    }
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::solveBinary()
//-----------------------------------------------------------------------------
QString SystemVerilogExpressionParser::solveBinary(QStringView operation, QString const& leftTerm,
    QString const& rightTerm) const
{
    if (isLiteral(leftTerm))
    {
        const auto leftOperand = leftTerm.toDouble();
        const auto rightOperand = rightTerm.toDouble();

        qreal result = 0;

        if (operation.compare(QLatin1String("**")) == 0 || operation.compare(QLatin1String("$pow")) == 0)
        {
            if (leftOperand == 0 && rightOperand < 0)
            {
                return QStringLiteral("x");
            }

            result = qPow(leftOperand, rightOperand);
        }

        else if (operation.compare(QLatin1String("*")) == 0)
        {
            result = leftOperand * rightOperand;
        }

        else if (operation.compare(QLatin1String("/")) == 0)
        {
            if (rightOperand == 0)
            {
                return QStringLiteral("x");
            }

            result = leftOperand / rightOperand;
        }

        else if (operation.compare(QLatin1String("%")) == 0)
        {
            if (rightOperand == 0 || leftTerm.contains(QLatin1Char('.')) || rightTerm.contains(QLatin1Char('.')))
            {
                return QStringLiteral("x");
            }

            result = leftTerm.toLongLong() % rightTerm.toLongLong();
        }

        else if (operation.compare(QLatin1String("+")) == 0)
        {
            result = leftOperand + rightOperand;
        }

        else if (operation.compare(QLatin1String("-")) == 0)
        {
            result = leftOperand - rightOperand;
        }

        else if (operation.compare(QLatin1String("<<")) == 0)
        {
            if (leftTerm.contains(QLatin1Char('.')) || rightTerm.contains(QLatin1Char('.')))
            {
                return QStringLiteral("x");
            }

            result = leftTerm.toLongLong() << rightTerm.toLongLong();
        }

        else if (operation.compare(QLatin1String(">>")) == 0)
        {
            if (leftTerm.contains(QLatin1Char('.')) || rightTerm.contains(QLatin1Char('.')))
            {
                return QStringLiteral("x");
            }

            result = leftTerm.toLongLong() >> rightTerm.toLongLong();
        }

        else if ((operation.compare(QLatin1String(">")) == 0 && leftOperand > rightOperand) ||
            (operation.compare(QLatin1String("<")) == 0 && leftOperand < rightOperand) ||
            (operation.compare(QLatin1String("==")) == 0 && leftOperand == rightOperand) ||
            (operation.compare(QLatin1String(">=")) == 0 && leftOperand >= rightOperand) ||
            (operation.compare(QLatin1String("<=")) == 0 && leftOperand <= rightOperand) ||
            (operation.compare(QLatin1String("!=")) == 0 && leftOperand != rightOperand))
        {
            return QStringLiteral("1");
        }

        else if (operation.compare(QLatin1String("||")) == 0)
        {
            result = leftTerm.toLongLong() || rightTerm.toLongLong();
        }

        else if (operation.compare(QLatin1String("&&")) == 0)
        {
            result = leftTerm.toLongLong() && rightTerm.toLongLong();
        }

        else if (operation.compare(QLatin1String("|")) == 0)
        {
            if (leftTerm.contains(QLatin1Char('.')) || rightTerm.contains(QLatin1Char('.')))
            {
                return QStringLiteral("x");
            }

            result = leftTerm.toLongLong() | rightTerm.toLongLong();
        }

        else if (operation.compare(QLatin1String("^")) == 0)
        {
            if (leftTerm.contains(QLatin1Char('.')) || rightTerm.contains(QLatin1Char('.')))
            {
                return QStringLiteral("x");
            }

            result = leftTerm.toLongLong() ^ rightTerm.toLongLong();
        }

        else if (operation.compare(QLatin1String("&")) == 0)
        {
            if (leftTerm.contains(QLatin1Char('.')) || rightTerm.contains(QLatin1Char('.')))
            {
                return QStringLiteral("x");
            }

            result = leftTerm.toLongLong() & rightTerm.toLongLong();
        }

        if (!leftTerm.contains(QLatin1Char('.')) && (operation.compare(QLatin1String("/")) == 0 ||
            (operation.compare(QLatin1String("**")) == 0 && rightOperand < 0)))
        {
            return QString::number(static_cast<int>(result));
        }
        else
        {
            return QString::number(result, 'f', qMax(precisionOf(leftTerm), precisionOf(rightTerm)));
        }
    }

    else if (isStringLiteral(leftTerm) && isStringLiteral(rightTerm))
    {
        if ((operation.compare(QLatin1String("==")) == 0 && leftTerm.compare(rightTerm) == 0) ||
            (operation.compare(QLatin1String("!=")) == 0 && leftTerm.compare(rightTerm) != 0))
        {
            return QStringLiteral("1");
        }
        else
        {
            return QStringLiteral("0");
        }
    }

    return QStringLiteral("x");
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::solveUnary()
//-----------------------------------------------------------------------------
QString SystemVerilogExpressionParser::solveUnary(QStringView operation, QString const& term) const
{
    if (operation.compare(QLatin1String("$clog2")) == 0)
    {
        return solveClog2(term);
    }
    else if (operation.compare(QLatin1String("$exp")) == 0)
    {
        return QString::number(qExp(term.toLongLong()));
    }
    else if (operation.compare(QLatin1String("$sqrt")) == 0)
    {
        return solveSqrt(term);
    }
    else if (operation.compare(QLatin1String("~")) == 0)
    {
        return QString::number(~term.toLongLong());
    }
    else if (operation.compare(QLatin1String("`")) == 0)
    {
        return QString::number(~term.toLongLong() + 1);
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

    int answer = 0;
    while (quotient > 1)
    {
        quotient /= 2;
        ++answer;
    }

    return QString::number(answer);
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::solveSqrt()
//-----------------------------------------------------------------------------
QString SystemVerilogExpressionParser::solveSqrt(QString const& value) const
{
    const qreal radicand = value.toLongLong();
    if (radicand < 0)
    {
        return QStringLiteral("x");
    }

    return QString::number(qSqrt(radicand));
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::parseConstant()
//-----------------------------------------------------------------------------
QString SystemVerilogExpressionParser::parseConstant(QStringView token) const
{
    if (token.contains(QLatin1Char('.')))
    {
        return token.toString();
    }

    const auto base = baseOf(token);

    // Remove formating of the number.
    auto formattedToken = token.toString();
    static QRegularExpression prefix(QStringLiteral("^([1-9][0-9_]*)?'[sS]?[dDbBoOhH]?"));
    formattedToken.remove(0, prefix.match(token).capturedLength());
    formattedToken.remove(QLatin1Char('_'));

    bool valid = false;
    const qlonglong value = formattedToken.toLongLong(&valid, base);
    if (valid == false)
    {
        return QStringLiteral("x");
    }

    return QString::number(value);
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::isSymbol()
//-----------------------------------------------------------------------------
bool SystemVerilogExpressionParser::isSymbol(QStringView /*expression*/) const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::findSymbolValue()
//-----------------------------------------------------------------------------
QString SystemVerilogExpressionParser::findSymbolValue(QStringView symbol) const
{
    return symbol.toString();
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::getBaseForSymbol()
//-----------------------------------------------------------------------------
int SystemVerilogExpressionParser::getBaseForSymbol(QStringView symbol) const
{
    return baseOf(symbol);
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::operatorPrecedence()
//-----------------------------------------------------------------------------
unsigned int SystemVerilogExpressionParser::operatorPrecedence(QStringView oper)
{
    // Higher value means higher precedence.
    const static QMap<QStringView, int> operator_precedence =
    {
        {QStringLiteral(":")      , 1},
        {QStringLiteral("?")      , 2},
        {QStringLiteral("||")     , 3},
        {QStringLiteral("&&")     , 4},
        {QStringLiteral("|")      , 5},
        {QStringLiteral("^" )     , 6},
        {QStringLiteral("&")      , 7},
        {QStringLiteral("==")     , 8},
        {QStringLiteral("!=")     , 8},
        {QStringLiteral("<")      , 9},
        {QStringLiteral(">")      , 9},
        {QStringLiteral("<=")     , 9},
        {QStringLiteral(">=")     , 9},
        {QStringLiteral("<<")     , 10},
        {QStringLiteral(">>")     , 10},
        {QStringLiteral("+")      , 11},
        {QStringLiteral("-")      , 11},
        {QStringLiteral("%")      , 12},
        {QStringLiteral("*")      , 12},
        {QStringLiteral("/")      , 12},
        {QStringLiteral("**")     , 13},
        {QStringLiteral("`")      , 13}, // unary minus
        {QStringLiteral("~")      , 14},
        {QStringLiteral("$clog2") , 14},
        {QStringLiteral("$pow")   , 14},
        {QStringLiteral("$sqrt")  , 14},
        {QStringLiteral("$exp")   , 14},
        {QStringLiteral("$ipxact_port_value"), 14},
        {QStringLiteral("$ipxact_field_value"), 14},
        {QStringLiteral("$ipxact_mode_condition"), 14},
        {QStringLiteral("(")      , 15},
        {QStringLiteral(")")      , 15},
        {QStringLiteral("{")      , 15},
        {QStringLiteral("}")      , 15}
    };

    return operator_precedence.value(oper, 0);
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::getBaseForNumber()
//-----------------------------------------------------------------------------
int SystemVerilogExpressionParser::baseOf(QStringView constantNumber)
{
    const static QMap<QStringView, int> base_formats =
    {
        { QString(), 10 },
        { QStringLiteral("d"), 10 },
        { QStringLiteral("D"), 10 },
        { QStringLiteral("h"), 16 },
        { QStringLiteral("H"), 16 },
        { QStringLiteral("o"), 8 },
        { QStringLiteral("O"), 8 },
        { QStringLiteral("b"), 2 },
        { QStringLiteral("B"), 2 },
    };

    const auto format = BASE_FORMAT.match(constantNumber).capturedView(1);

    return base_formats.value(format, 0);
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::getDecimalPrecision()
//-----------------------------------------------------------------------------
int SystemVerilogExpressionParser::precisionOf(QStringView term) noexcept
{
    int decimalLength = 0;
    
    if  (const int commaPosition = term.indexOf(QLatin1Char('.')); commaPosition != -1)
    {
        decimalLength = term.length() - commaPosition - 1;
    }

    return decimalLength;
}
