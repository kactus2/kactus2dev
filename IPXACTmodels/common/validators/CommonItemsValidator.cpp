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
// Function: InterconnectionConfigurationValidator::hasValidName()
//-----------------------------------------------------------------------------
bool CommonItemsValidator::hasValidName(QString const& name)
{
    return (name.isEmpty() == false && name.trimmed().isEmpty() == false);
}

//-----------------------------------------------------------------------------
// Function: InterconnectionConfigurationValidator::hasValidIsPresent()
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
