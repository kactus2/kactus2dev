//-----------------------------------------------------------------------------
// File: CommonItemsValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 19.05.2023
//
// Description:
// Functions for validating common items in IPXACT models.
//-----------------------------------------------------------------------------


#include "CommonItemsValidator.h"


//-----------------------------------------------------------------------------
// Function: CommonItemsValidator::hasValidName()
//-----------------------------------------------------------------------------
bool CommonItemsValidator::hasValidName(QString const& name)
{
    return (name.isEmpty() == false && name.trimmed().isEmpty() == false);
}

//-----------------------------------------------------------------------------
// Function: CommonItemsValidator::hasValidIsPresent()
//-----------------------------------------------------------------------------
bool CommonItemsValidator::hasValidIsPresent(QString const& isPresent, QSharedPointer<ExpressionParser> parser)
{
    if (isPresent.isEmpty())
    {
        return true;
    }

    QString solvedValue = parser->parseExpression(isPresent);

    bool toIntOk = true;
    int intValue = solvedValue.toInt(&toIntOk);

    return (toIntOk && intValue >= 0 && intValue < 2);
}

//-----------------------------------------------------------------------------
// Function: CommonItemsValidator::isValidExpression()
//-----------------------------------------------------------------------------
bool CommonItemsValidator::isValidExpression(QString const& expression, QSharedPointer<ExpressionParser> parser)
{
    bool expressionOk = false;
    QString solvedValue = parser->parseExpression(expression, &expressionOk);

    if (expressionOk == false)
    {
        return false;
    }

    bool changeOk = true;
    quint64 intValue = solvedValue.toULongLong(&changeOk);

    if (changeOk && intValue > 0)
    {
        return true;
    }

    return false;
}
