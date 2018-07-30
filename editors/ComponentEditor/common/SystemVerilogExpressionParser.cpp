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

#include <QMap>
#include <QDebug>

#include <algorithm>

namespace
{
    const QRegularExpression PRIMARY_LITERAL(SystemVerilogSyntax::REAL_NUMBER + "|" +
        SystemVerilogSyntax::INTEGRAL_NUMBER + "|" +
        SystemVerilogSyntax::BOOLEAN_VALUE + "|" +
        SystemVerilogSyntax::STRING_LITERAL);



    const QRegularExpression BINARY_OPERATOR("\\*\\*|[*/%]|[+-]|<<|>>|<=?|>=?|!==?|===?|[&|]{1,2}|[\\^]|[$]pow");

    const QRegularExpression UNARY_OPERATOR("[$]clog2|[$]exp|[$]sqrt");

    const QChar OPEN_PARENTHESIS('(');
    const QChar CLOSE_PARENTHESIS(')');
    const QChar OPEN_ARRAY('{');
    const QChar CLOSE_ARRAY('}');

    const QString OPEN_PARENTHESIS_STRING("(");
    const QString OPEN_ARRAY_STRING("{");
    const QString CLOSE_ARRAY_STRING("}");

    const QRegularExpression ANY_OPERATOR(BINARY_OPERATOR.pattern() + "|" + UNARY_OPERATOR.pattern());
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
QStringList SystemVerilogExpressionParser::convertToRPN(QString const& expression) const
{
    // Convert expression to Reverse Polish Notation (RPN) using the Shunting Yard algorithm.
    QStringList output;
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
            while (stack.size() > 0 &&
                stack.last() != OPEN_PARENTHESIS_STRING &&
                operatorPrecedence(stack.last()) >= operatorPrecedence(operatorMatch.captured()))
            {
                output.append(stack.takeLast());
            }

            stack.append(operatorMatch.captured());

            index = operatorMatch.capturedEnd();
            nextMayBeLiteral = true;
        }
        else if (current == OPEN_PARENTHESIS)
        {
            stack.append(current);
            ++index;
            ++openParenthesis;
            nextMayBeLiteral = true;
        }
        else if (current == CLOSE_PARENTHESIS)
        {
            while (stack.size() > 0 && (stack.last() != OPEN_PARENTHESIS))
            {
                output.append(stack.takeLast());
            }
            
            if (stack.size() != 0)
            {
                stack.takeLast(); //!< Remove open parenthesis.
            }

            ++index;
            --openParenthesis;
            nextMayBeLiteral = false;
        }
        else if (current == OPEN_ARRAY)
        {
            output.append(current);
            ++index;
            ++openParenthesis;
            nextMayBeLiteral = true;
        }
        else if (current == CLOSE_ARRAY)
        {
            while (stack.size() > 0 && stack.last() != OPEN_ARRAY)
            {
                output.append(stack.takeLast());
            }

            if (stack.size() != 0)
            {
                output.append(stack.takeLast()); //!< Add open array.
            }
            stack.append(current); //!< Add close array.

            ++index;
            --openParenthesis;
            nextMayBeLiteral = false;
        }
        else if (current == QLatin1Char(','))
        {
            while (stack.size() > 0 && (stack.last() != OPEN_ARRAY && stack.last() != OPEN_PARENTHESIS))
            {
                output.append(stack.takeLast());
            }

            ++index;
            nextMayBeLiteral = true;
        }
        else
        {
            QRegularExpression separator(ANY_OPERATOR.pattern() + QStringLiteral("|[(){}]"));
            QString unknown = expression.mid(index, separator.match(expression, index).capturedStart() - index);
            output.append(unknown.trimmed());

            index += unknown.length();
            nextMayBeLiteral = false;
        }
    }

    if (openParenthesis != 0)
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
    QStringList resultStack;
    bool isWellFormed = true;

    for (QString const& token : rpn)
    {
        QRegularExpressionMatch match = PRIMARY_LITERAL.match(token, 0, QRegularExpression::NormalMatch,
            QRegularExpression::AnchoredMatchOption);

        if (match.hasMatch())
        {
            resultStack.append(parseConstant(token));
        }
        else if (isBinaryOperator(token))
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

            QString arrayItem(resultStack.takeLast() + items.join(QLatin1Char(',')) + token);
            resultStack.append(arrayItem);
        }
        else if (isSymbol(token))
        {
            resultStack.append(findSymbolValue(token));
        }
        else
        {
            isWellFormed = false;
            break;
        }
    }

    if (validExpression != nullptr)
    {
        *validExpression = isWellFormed && resultStack.contains(QStringLiteral("x")) == false;
    }

    return resultStack.join(QString());
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::isStringLiteral()
//-----------------------------------------------------------------------------
bool SystemVerilogExpressionParser::isStringLiteral(QString const& expression) const
{
    static QRegularExpression stringExpression(SystemVerilogSyntax::STRING_LITERAL);
    return stringExpression.match(expression, 0, QRegularExpression::NormalMatch,
        QRegularExpression::AnchoredMatchOption).hasMatch();
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::isLiteral()
//-----------------------------------------------------------------------------
bool SystemVerilogExpressionParser::isLiteral(QString const& expression) const
{
    static QRegularExpression literalExpression("^\\s*(" + SystemVerilogSyntax::INTEGRAL_NUMBER + "|" +
        SystemVerilogSyntax::REAL_NUMBER + ")\\s*$");

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
    if (isLiteral(leftTerm) && isLiteral(rightTerm))
    {
        qreal leftOperand = leftTerm.toDouble();
        qreal rightOperand = rightTerm.toDouble();

        qreal result = 0;

        if (operation.compare(QLatin1String("**")) == 0 || operation.compare(QLatin1String("$pow")) == 0){
            if (leftOperand == 0 && rightOperand < 0)
            {
                return QStringLiteral("x");
            }

            result = qPow(leftOperand, rightOperand);
        }
        else if (operation.compare(QLatin1String("*")) == 0){
            result = leftOperand*rightOperand;
        }
        else if (operation.compare(QLatin1String("/")) == 0){
            if (rightOperand == 0)
            {
                return QStringLiteral("x");
            }

            result = leftOperand/rightOperand;
        }
        else if (operation.compare(QLatin1String("%")) == 0){
            if (rightOperand == 0 or leftTerm.contains(".") || rightTerm.contains("."))
            {
                return QStringLiteral("x");
            }

            result = leftTerm.toLongLong() % rightTerm.toLongLong();
        }
        else if (operation.compare(QLatin1String("+")) == 0){
            result = leftOperand + rightOperand;
        }
        else if (operation.compare(QLatin1String("-")) == 0){
            result = leftOperand - rightOperand;
        }
        else if (operation.compare(QLatin1String("<<")) == 0){
          if (leftTerm.contains('.') || rightTerm.contains(".")){
            return QStringLiteral("x");
          }
          result = leftTerm.toLongLong() << rightTerm.toLongLong();

        }
        else if (operation.compare(QLatin1String(">>")) == 0){
          if (leftTerm.contains('.') || rightTerm.contains(".")){
            return QStringLiteral("x");
          }
          result = leftTerm.toLongLong() << rightTerm.toLongLong();
        }
        else if ((operation.compare(QLatin1String(">")) == 0 && leftOperand > rightOperand) ||
            (operation.compare(QLatin1String("<")) == 0 && leftOperand < rightOperand) ||
            (operation.compare(QLatin1String("==")) == 0 && leftOperand == rightOperand) ||
            (operation.compare(QLatin1String(">=")) == 0 && leftOperand >= rightOperand) ||
            (operation.compare(QLatin1String("<=")) == 0 && leftOperand <= rightOperand) ||
            (operation.compare(QLatin1String("!=")) == 0 && leftOperand != rightOperand)){
            return QStringLiteral("1");
        }
        else if (operation.compare(QLatin1String("||"))==0){
          result = leftTerm.toLongLong() || rightTerm.toLongLong();
        }
        else if (operation.compare(QLatin1String("&&"))==0){
          result = leftTerm.toLongLong() && rightTerm.toLongLong();
        }
        else if (operation.compare(QLatin1String("|"))==0){
          if (leftTerm.contains('.') || rightTerm.contains(".")){
            return QStringLiteral("x");
          }
          result = leftTerm.toLongLong() | rightTerm.toLongLong();
        }
        else if (operation.compare(QLatin1String("^"))==0){
          if (leftTerm.contains('.') || rightTerm.contains(".")){
            return QStringLiteral("x");
          }
          result = leftTerm.toLongLong() ^ rightTerm.toLongLong();
        }
        else if (operation.compare(QLatin1String("&"))==0){
          if (leftTerm.contains('.') || rightTerm.contains(".")){
            return QStringLiteral("x");
          }
          result = leftTerm.toLongLong() & rightTerm.toLongLong();
        }

        if (!leftTerm.contains('.') &&  (operation.compare(QLatin1String("/")) == 0 ||
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
    else
    {
        return QStringLiteral("x");
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
    else if (operation.compare(QLatin1String("$exp")) == 0)
    {
        return QString::number(qExp(term.toLongLong()));
    }
    else if (operation.compare(QLatin1String("$sqrt")) == 0)
    {
        return solveSqrt(term);
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
    if (QRegularExpression(QStringLiteral("\\b(?i)true\\b")).match(token).hasMatch())
    {
        return QStringLiteral("1");
        // Expression 'false' will evaluate to 0 by QString::number.
    }    
    else if (isStringLiteral(token) || token.contains(QLatin1Char('.')))
    {
        return token;
    }

    int base = getBaseForNumber(token);

    // Remove formating of the number.
    static QRegularExpression prefix(QStringLiteral("^([1-9][0-9_]*)?'[sS]?[dDbBoOhH]?"));
    token.remove(prefix);
    token.remove('_');

    return QString::number(token.toLongLong(nullptr, base));
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
       {"||"     , 1},
       {"&&"     , 2},
       {"|"      , 3},
       {"^"      , 4},
       {"&"      , 5},
       {"=="     , 6},
       {"!="     , 6},
       {"<"      , 7},
       {">"      , 7},
       {"<="     , 7},
       {">="     , 7},
       {"<<"     , 8},
       {">>"     , 8},
       {"+"      , 9},
       {"-"      , 9},
       {"%"      , 10},
       {"*"      , 10},
       {"/"      , 10},
       {"**"     , 11},
       {"$clog2" , 12},
       {"$pow"   , 12},
       {"("      , 13},
       {")"      , 13},
       {"{"      , 13},
       {"}"      , 13}
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
        { "", 10 },
        { "d", 10 },
        { "D", 10 },
        { "h", 16 },
        { "H", 16 },
        { "o", 8 },
        { "O", 8 },
        { "b", 2 },
        { "B", 2 },
    };

    static QRegularExpression baseFormat("'" + SystemVerilogSyntax::SIGNED + "([dDbBoOhH]?)");

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
