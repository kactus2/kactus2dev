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
// Function: SystemVerilogExpressionParser::parseConstant()
//-----------------------------------------------------------------------------
QString SystemVerilogExpressionParser::parseConstant(QString const& constantNumber)
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

    return QString::number(result.toInt(0, baseForFormat(baseFormat.cap(1))));
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogExpressionParser::baseForFormat()
//-----------------------------------------------------------------------------
int SystemVerilogExpressionParser::baseForFormat(QString const& baseFormat)
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
