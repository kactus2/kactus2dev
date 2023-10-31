//-----------------------------------------------------------------------------
// File: NullParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 02.02.2015
//
// Description:
// Expression parser implementation that does nothing.
//-----------------------------------------------------------------------------

#include "NullParser.h"

//-----------------------------------------------------------------------------
// Function: NullParser::()
//-----------------------------------------------------------------------------
NullParser::NullParser() : ExpressionParser()
{

}

//-----------------------------------------------------------------------------
// Function: NullParser::parseExpression()
//-----------------------------------------------------------------------------
QString NullParser::parseExpression(QStringView expression, bool* /*isValidExpression*/) const
{
    return expression.toString();
}

//-----------------------------------------------------------------------------
// Function: NullParser::isPlainValue()
//-----------------------------------------------------------------------------
bool NullParser::isPlainValue(QStringView /*expression*/) const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: NullParser::baseForExpression()
//-----------------------------------------------------------------------------
int NullParser::baseForExpression(QStringView /*expression*/) const
{
    return 0;
}