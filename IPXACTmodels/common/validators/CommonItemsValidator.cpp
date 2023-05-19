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
bool CommonItemsValidator::hasValidName(std::string_view name)
{
    return (name.empty() == false && std::all_of(name.begin(), name.end(), isspace) == false);
}

//-----------------------------------------------------------------------------
// Function: InterconnectionConfigurationValidator::hasValidIsPresent()
//-----------------------------------------------------------------------------
bool CommonItemsValidator::hasValidIsPresent(std::string const& isPresent, QSharedPointer<ExpressionParser> parser)
{
    if (isPresent.empty() == true)
    {
        return true;
    }

    QString solvedValue = parser->parseExpression(QString::fromStdString(isPresent));

    bool toIntOk = true;
    int intValue = solvedValue.toInt(&toIntOk);

    return (toIntOk && intValue >= 0 && intValue < 2);
}
