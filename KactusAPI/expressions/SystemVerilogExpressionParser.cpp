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

    const QRegularExpression BINARY_OPERATOR(QStringLiteral("\\*\\*|[*/%]|[+-]|<<|>>|<=?|>=?|!==?|===?|[&|]{1,2}|[\\^]|[$]pow"));

    const QRegularExpression UNARY_OPERATOR(QStringLiteral("[$]clog2|[$]exp|[$]sqrt|~"));

    const QChar OPEN_PARENTHESIS(QLatin1Char('('));
    const QChar CLOSE_PARENTHESIS(QLatin1Char(')'));
    const QChar OPEN_ARRAY(QLatin1Char('{'));
    const QChar CLOSE_ARRAY(QLatin1Char('}'));

    const QString OPEN_PARENTHESIS_STRING(QStringLiteral("("));
    const QString OPEN_ARRAY_STRING(QStringLiteral("{"));
    const QString CLOSE_ARRAY_STRING(QStringLiteral("}"));

    const QRegularExpression ANY_OPERATOR(BINARY_OPERATOR.pattern() % QStringLiteral("|") % UNARY_OPERATOR.pattern());
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::SystemVerilogExpressionParser()
//-----------------------------------------------------------------------------
SystemVerilogExpressionParser::SystemVerilogExpressionParser()
{

}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::parseExpression()
//-----------------------------------------------------------------------------
QString SystemVerilogExpressionParser::parseExpression(QString const& expression, bool* validExpression) const
{
    return solveRPN(convertToRPN(expression), validExpression);
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::isArrayExpression()
//-----------------------------------------------------------------------------
bool SystemVerilogExpressionParser::isArrayExpression(QString const& expression) const
{
    return expression.contains(OPEN_ARRAY) && expression.contains(CLOSE_ARRAY);
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
    
    for (QString const& token : convertToRPN(expression))
    {
        if (isLiteral(token))
        {
            greatestBase = qMax(greatestBase, getBaseForNumber(token));
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
QVector<QString> SystemVerilogExpressionParser::convertToRPN(QString const& expression) const
{
    // Convert expression to Reverse Polish Notation (RPN) using the Shunting Yard algorithm.
    QVector<QString> output;
    QVector<QString> stack;

    int openParenthesis = 0;
    bool nextMayBeLiteral = true;
    
    for (int index = 0; index < expression.size(); /*index incremented inside loop*/)
    {
        const QChar current = expression.at(index);
        if (current.isSpace())
        {
            ++index;
            continue;
        }

        QRegularExpressionMatch operatorMatch = ANY_OPERATOR.match(expression, index,
            QRegularExpression::NormalMatch, QRegularExpression::AnchoredMatchOption);
        QRegularExpressionMatch literalMatch = PRIMARY_LITERAL.match(expression, index,
            QRegularExpression::NormalMatch, QRegularExpression::AnchoredMatchOption);

        if (nextMayBeLiteral && literalMatch.hasMatch())
        {
            output.append(literalMatch.captured());

            index = literalMatch.capturedEnd();
            nextMayBeLiteral = false;
        }
        else if (operatorMatch.hasMatch())
        {
            QString operation = operatorMatch.captured();
            while (stack.isEmpty() == false &&
                stack.last() != OPEN_PARENTHESIS_STRING &&
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
            
            if (stack.size() != 0)
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
            QRegularExpression separator(ANY_OPERATOR.pattern() % QStringLiteral("|[(){},]"));
            QString unknown = expression.mid(index, separator.match(expression, index).capturedStart() - index);
            
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
QString SystemVerilogExpressionParser::solveRPN(QVector<QString> rpn, bool* validExpression) const
{
    QStringList resultStack;
    bool isWellFormed = true;
    
    for (QString const& token : rpn)
    {
        if (isBinaryOperator(token))
        {
            if (resultStack.size() < 2)
            {
                isWellFormed = false;
                break;
            }

            resultStack.append(solveBinary(token, resultStack.takeLast(), resultStack.takeLast()));
        }
        else if (isUnaryOperator(token))
        {
            if (resultStack.isEmpty())
            {
                isWellFormed = false;
                break;
            }

            resultStack.append(solveUnary(token, resultStack.takeLast()));
        }
        else if (token.compare(OPEN_ARRAY_STRING) == 0)
        {
            resultStack.append(token);
        }
        else if (token.compare(CLOSE_ARRAY_STRING) == 0)
        {
            QStringList items;
            while (resultStack.size() > 0 && resultStack.last().compare(OPEN_ARRAY_STRING) != 0)
            {
                items.prepend(resultStack.takeLast());
            }

            if (resultStack.size() == 0)
            {
                isWellFormed = false;
                break;
            }

            QString arrayItem(resultStack.takeLast() % items.join(QLatin1Char(',')) % token);
            resultStack.append(arrayItem);
        }
        else if (token.compare(QLatin1String("true"), Qt::CaseInsensitive) == 0)
        {
            resultStack.append(QStringLiteral("1"));
        }
        else if (token.compare(QLatin1String("false"), Qt::CaseInsensitive) == 0)
        {
            resultStack.append(QStringLiteral("0"));
        }
        else if (isStringLiteral(token))
        {
            resultStack.append(token);
        }
        else if (isSymbol(token))
        {
            resultStack.append(findSymbolValue(token));
        }
        else
        {
            QString constant = parseConstant(token);            
            resultStack.append(constant);           
            
            if (constant == QLatin1String("x"))
            {
                isWellFormed = false;
                break;
            }
        }
    }

    if (validExpression != nullptr)
    {
        *validExpression = (isWellFormed && resultStack.contains(QStringLiteral("x")) == false);
    }

    return resultStack.join(QString());
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::isStringLiteral()
//-----------------------------------------------------------------------------
bool SystemVerilogExpressionParser::isStringLiteral(QString const& expression) const
{
    return expression.startsWith(QLatin1Char('"')) && expression.endsWith(QLatin1Char('"'));
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::isLiteral()
//-----------------------------------------------------------------------------
bool SystemVerilogExpressionParser::isLiteral(QString const& expression) const
{
    static QRegularExpression literalExpression(QStringLiteral("^\\s*(") % SystemVerilogSyntax::INTEGRAL_NUMBER % 
        QStringLiteral("|") % SystemVerilogSyntax::REAL_NUMBER % QStringLiteral(")\\s*$"));

    return literalExpression.match(expression).hasMatch();
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::isUnaryOperator()
//-----------------------------------------------------------------------------
bool SystemVerilogExpressionParser::isUnaryOperator(QString const& token) const
{
    static const QVector<QString> unaryOperators({ QStringLiteral("$clog2"),
        QStringLiteral("$exp"), QStringLiteral("$sqrt"), QStringLiteral("~") });

    return unaryOperators.contains(token);
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::isBinaryOperator()
//-----------------------------------------------------------------------------
bool SystemVerilogExpressionParser::isBinaryOperator(QString const& token) const
{    
    static const QVector<QString> binaryOperators({ QStringLiteral("+"),
        QStringLiteral("-"), QStringLiteral("*"), QStringLiteral("/"), 
        QStringLiteral("%"), QStringLiteral("**"), QStringLiteral("<<"),
        QStringLiteral(">>"), QStringLiteral("<"), QStringLiteral("<="),
        QStringLiteral(">"), QStringLiteral(">="), QStringLiteral("!="), 
        QStringLiteral("!=="), QStringLiteral("=="), QStringLiteral("==="), 
        QStringLiteral("&"), QStringLiteral("&&"), QStringLiteral("|"), 
        QStringLiteral("||"), QStringLiteral("^"), QStringLiteral("$pow")});

    return binaryOperators.contains(token);
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::solveBinary()
//-----------------------------------------------------------------------------
QString SystemVerilogExpressionParser::solveBinary(QString const& operation, QString const& leftTerm,
    QString const& rightTerm) const
{
    if (isLiteral(leftTerm))
    {
        qreal leftOperand = leftTerm.toDouble();
        qreal rightOperand = rightTerm.toDouble();

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
            result = leftOperand*rightOperand;
        }

        else if (operation.compare(QLatin1String("/")) == 0)
        {
            if (rightOperand == 0)
            {
                return QStringLiteral("x");
            }

            result = leftOperand/rightOperand;
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

        else if (operation.compare(QLatin1String("&"))==0)
        {
          if (leftTerm.contains(QLatin1Char('.')) || rightTerm.contains(QLatin1Char('.')))
          {
                return QStringLiteral("x");
          }
          
          result = leftTerm.toLongLong() & rightTerm.toLongLong();
        }

        if (!leftTerm.contains(QLatin1Char('.')) &&  (operation.compare(QLatin1String("/")) == 0 ||
           (operation.compare(QLatin1String("**")) == 0 && rightOperand < 0)))
        {
            int integerResult = result;
            return QString::number(integerResult);
        }
        else
        {
            return QString::number(result, 'f',
                qMax(getDecimalPrecision(leftTerm), getDecimalPrecision(rightTerm)));
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
QString SystemVerilogExpressionParser::solveUnary(QString const& operation, QString const& term) const
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
// Function: SystemVerilogExpressionParser::solveSqrt()
//-----------------------------------------------------------------------------
QString SystemVerilogExpressionParser::solveSqrt(QString const& value) const
{
    qreal radicand = value.toLongLong();
    if (radicand < 0)
    {
        return QStringLiteral("x");
    }

    return QString::number(qSqrt(radicand));
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::parseConstant()
//-----------------------------------------------------------------------------
QString SystemVerilogExpressionParser::parseConstant(QString token) const
{
    if (token.contains(QLatin1Char('.')))
    {
        return token;
    }

    int base = getBaseForNumber(token);

    // Remove formating of the number.   
    static QRegularExpression prefix(QStringLiteral("^([1-9][0-9_]*)?'[sS]?[dDbBoOhH]?"));
    token.remove(0, prefix.match(token).capturedLength());
    token.remove(QLatin1Char('_'));

    bool valid = false;
    qlonglong value = token.toLongLong(&valid, base);
    if (valid == false)
    {
        return QStringLiteral("x");
    }

    return QString::number(value);
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
QString SystemVerilogExpressionParser::findSymbolValue(QString const& symbol) const
{
    return symbol;
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::getBaseForSymbol()
//-----------------------------------------------------------------------------
int SystemVerilogExpressionParser::getBaseForSymbol(QString const& symbol) const
{
    return getBaseForNumber(symbol);
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::operatorPrecedence()
//-----------------------------------------------------------------------------
unsigned int SystemVerilogExpressionParser::operatorPrecedence(QString oper) const
{
    const static QMap<QString, int> operator_precedence =
    {
       {QStringLiteral("||")     , 1},
       {QStringLiteral("&&")     , 2},
       {QStringLiteral("|")      , 3},
       {QStringLiteral("^" )     , 4},
       {QStringLiteral("&")      , 5},
       {QStringLiteral("==")     , 6},
       {QStringLiteral("!=")     , 6},
       {QStringLiteral("<")      , 7},
       {QStringLiteral(">")      , 7},
       {QStringLiteral("<=")     , 7},
       {QStringLiteral(">=")     , 7},
       {QStringLiteral("<<")     , 8},
       {QStringLiteral(">>")     , 8},
       {QStringLiteral("+")      , 9},
       {QStringLiteral("-")      , 9},
       {QStringLiteral("%")      , 10},
       {QStringLiteral("*")      , 10},
       {QStringLiteral("/")      , 10},
       {QStringLiteral("**")     , 11},
       {QStringLiteral("~")     , 12},
       {QStringLiteral("$clog2") , 12},
       {QStringLiteral("$pow")   , 12},
       {QStringLiteral("(")      , 13},
       {QStringLiteral(")")      , 13},
       {QStringLiteral("{")      , 13},
       {QStringLiteral("}")      , 13}
    };

    return operator_precedence.value(oper, 0);
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::getBaseForNumber()
//-----------------------------------------------------------------------------
int SystemVerilogExpressionParser::getBaseForNumber(QString const& constantNumber) const
{
    const static QMap<QString, int> base_formats =
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

    static QRegularExpression baseFormat(QStringLiteral("'[sS]?([dDbBoOhH]?)"));

    QString format = baseFormat.match(constantNumber).captured(1);

    return base_formats.value(format, 0);
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::getDecimalPrecision()
//-----------------------------------------------------------------------------
int SystemVerilogExpressionParser::getDecimalPrecision(QString const& term) const
{
    int decimalLength = 0;
    int commaPosition = term.indexOf(QLatin1Char('.'));
    if  (commaPosition != -1)
    {
        decimalLength = term.length() - commaPosition - 1;
    }

    return decimalLength;
}
