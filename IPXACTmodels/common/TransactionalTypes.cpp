//-----------------------------------------------------------------------------
// File: TransactionalTypes.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 10.09.2015
//
// Description:
// NameSpace TransactionalTypes is used to store functions related to transactional ports.
//-----------------------------------------------------------------------------

#include "TransactionalTypes.h"

//-----------------------------------------------------------------------------
// Function: TransactionalTypes::isIpXactInitiativeType()
//-----------------------------------------------------------------------------
bool TransactionalTypes::isIpXactInitiativeType(QString const& initiativeType)
{
    return INITIATIVE_TYPES.values().contains(initiativeType);
}

//-----------------------------------------------------------------------------
// Function: TransactionalTypes::isIpXactKindType()
//-----------------------------------------------------------------------------
bool TransactionalTypes::isIpXactKindType(QString const& kindType)
{
    return KIND_TYPES.contains(kindType);
}

//-----------------------------------------------------------------------------
// Function: TransactionalTypes::initiativeToString()
//-----------------------------------------------------------------------------
QString TransactionalTypes::initiativeToString(TransactionalTypes::Initiative const& initiative)
{
    return INITIATIVE_TYPES.value(initiative, QString());
}

//-----------------------------------------------------------------------------
// Function: TransactionalTypes::strToInitiative()
//-----------------------------------------------------------------------------
TransactionalTypes::Initiative TransactionalTypes::strToInitiative(QString const& str)
{
    return INITIATIVE_TYPES.key(str, Initiative::INITIATIVE_INVALID);
}

//-----------------------------------------------------------------------------
// Function: TransactionalTypes::convertToMirrored()
//-----------------------------------------------------------------------------
TransactionalTypes::Initiative TransactionalTypes::convertToMirrored(QString const& initiative)
{
    TransactionalTypes::Initiative mirroredInitiative = TransactionalTypes::strToInitiative(initiative);
    if (mirroredInitiative == Initiative::REQUIRES)
    {
        mirroredInitiative = Initiative::PROVIDES;
    }
    else if (mirroredInitiative == Initiative::PROVIDES)
    {
        mirroredInitiative = Initiative::REQUIRES;
    }

    return mirroredInitiative;
}
