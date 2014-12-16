#include "IPXactValueParser.h"

#include <IPXACTmodels/StringPromtAtt.h>

#include <QRegExp>

#include "qmath.h"

//-----------------------------------------------------------------------------
// Function: IPXactValueParser::()
//-----------------------------------------------------------------------------
IPXactValueParser::IPXactValueParser() : ExpressionParser()
{

}

//-----------------------------------------------------------------------------
// Function: IPXactValueParser::()
//-----------------------------------------------------------------------------
IPXactValueParser::~IPXactValueParser()
{

}

//-----------------------------------------------------------------------------
// Function: IPXactValueParser::parseExpression()
//-----------------------------------------------------------------------------
QString IPXactValueParser::parseExpression(QString const& expression) const
{
    QString format = getFormatForExpression(expression);
    if ((format == "bool" || format == "string" || format.isEmpty()) && 
        isValidExpression(expression, format))
    {
        return expression;
    }

    return QString::number(parseConstant(expression, format));
}

//-----------------------------------------------------------------------------
// Function: IPXactValueParser::parseExpression()
//-----------------------------------------------------------------------------
qreal IPXactValueParser::parseConstant(QString const& expression, QString const& format) const
{
    if (format == "long")
    {
        return longValueOf(expression);
    }
    else if (format == "float")
    {
        return floatValueOf(expression);
    }
    else
    {
        return expression.toDouble();
    }	
}

//-----------------------------------------------------------------------------
// Function: IPXactValueParser::isValidExpression()
//-----------------------------------------------------------------------------
bool IPXactValueParser::isValidExpression(QString const& expression) const
{
    return !expression.isEmpty();
}

//-----------------------------------------------------------------------------
// Function: IPXactValueParser::isValidExpression()
//-----------------------------------------------------------------------------
bool IPXactValueParser::isValidExpression(QString const& expression, QString const& format) const
{
    if (format.isEmpty() || format == "string")
    {
        return true;
    }

    QRegExp validatingExp;
    if (format == "bool")
    {
        validatingExp.setPattern(StringPromptAtt::VALID_BOOL_VALUE);
    }
    else if (format == "bitString")
    {
        validatingExp.setPattern(StringPromptAtt::VALID_BITSTRING_VALUE);
    }
    else if (format == "long")
    {
        validatingExp.setPattern(StringPromptAtt::VALID_LONG_VALUE);
    }
    else if (format == "float")
    {
        validatingExp.setPattern(StringPromptAtt::VALID_FLOAT_VALUE);
    }

    return validatingExp.exactMatch(expression);
}

//-----------------------------------------------------------------------------
// Function: IPXactValueParser::isPlainValue()
//-----------------------------------------------------------------------------
bool IPXactValueParser::isPlainValue(QString const& expression) const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: IPXactValueParser::baseForExpression()
//-----------------------------------------------------------------------------
int IPXactValueParser::baseForExpression(QString const& expression) const
{
    QString format = getFormatForExpression(expression);
    if (format == "bitString")
    {
        return 2;
    }
    else if (format == "long")
    {
        if (isHexadecimal(expression))
        {
            return 16;
        }
        else
        {
            return 10;
        }
    }
    else if (format == "float")
    {
        return 10;
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: IPXactValueParser::longValueOf()
//-----------------------------------------------------------------------------
qreal IPXactValueParser::longValueOf(QString const& value) const
{
    if (isHexadecimal(value))
    {
        QString hexValue = value;
        return hexValue.remove('#').toLong(0, 16);
    }
    else if (value.endsWith('k', Qt::CaseInsensitive))
    {
        QString coefficient = value;
        coefficient.chop(1);

        return coefficient.toLong()*1024;
    }
    else if (value.endsWith('M', Qt::CaseInsensitive))
    {
        QString coefficient = value;
        coefficient.chop(1);

        return coefficient.toLong()*1024*1024;
    }
    else if (value.endsWith('G', Qt::CaseInsensitive))
    {
        QString coefficient = value;
        coefficient.chop(1);

        return coefficient.toLong()*1024*1024*1024;
    }
    else if (value.endsWith('T', Qt::CaseInsensitive))
    {
        QString coefficient = value;
        coefficient.chop(1);

        return coefficient.toLong()*1024*1024*1024*1024;
    }
    else
    {
        return value.toLong();
    }
}

//-----------------------------------------------------------------------------
// Function: IPXactValueParser::isHexadecimal()
//-----------------------------------------------------------------------------
bool IPXactValueParser::isHexadecimal(QString const& value) const
{
    return value.startsWith("0x", Qt::CaseInsensitive) || value.startsWith('#') || 
        value.contains(QRegExp("[a-fA-F]"));
}

//-----------------------------------------------------------------------------
// Function: IPXactValueParser::floatValueOf()
//-----------------------------------------------------------------------------
qreal IPXactValueParser::floatValueOf(QString const& value) const
{
    if (value.contains('e'))
    {
        qreal coefficient = value.left(value.indexOf('e')).toFloat();
        qreal exponent = value.mid(value.indexOf('e') + 1).toFloat();
        qreal result = coefficient * qPow(10, exponent);
        return result;
    }
    else
    {
        return value.toFloat();
    }
}

//-----------------------------------------------------------------------------
// Function: IPXactValueParser::getFormatForExpression()
//-----------------------------------------------------------------------------
QString IPXactValueParser::getFormatForExpression(QString const& expression) const
{
    QRegExp boolExp(StringPromptAtt::VALID_BOOL_VALUE);
    if (boolExp.exactMatch(expression))
    {
        return "bool";
    }

    QRegExp bitStringExp(StringPromptAtt::VALID_BITSTRING_VALUE);
    if (bitStringExp.exactMatch(expression))
    {
        return "bitString";
    }

    QRegExp longExp(StringPromptAtt::VALID_LONG_VALUE);
    if (longExp.exactMatch(expression))
    {
        return "long";
    }
 
    QRegExp floatExp(StringPromptAtt::VALID_FLOAT_VALUE);
    if (floatExp.exactMatch(expression))
    {
        return "float";
    }

    return "string";
}
