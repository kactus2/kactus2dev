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
    for (unsigned int i = 0; i < TransactionalTypes::INITIATIVE_TYPE_COUNT; ++i)
    {
        if (TransactionalTypes::INITIATIVE_TYPES[i] == initiativeType)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: TransactionalTypes::isIpXactKindType()
//-----------------------------------------------------------------------------
bool TransactionalTypes::isIpXactKindType(QString const& kindType)
{
    for (unsigned int i = 0; i < TransactionalTypes::KIND_TYPE_COUNT; ++i)
    {
        if (TransactionalTypes::KIND_TYPES[i] == kindType)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: TransactionalTypes::initiativeToString()
//-----------------------------------------------------------------------------
QString TransactionalTypes::initiativeToString(TransactionalTypes::Initiative const& initiative)
{
    if (initiative == TransactionalTypes::REQUIRES)
    {
        return TransactionalTypes::INITIATIVE_REQUIRES;
    }
    else if (initiative == TransactionalTypes::PROVIDES)
    {
        return TransactionalTypes::INITIATIVE_PROVIDES;
    }
    else if (initiative == TransactionalTypes::BOTH)
    {
        return TransactionalTypes::INITIATIVE_BOTH;
    }
    else if (initiative == TransactionalTypes::PHANTOM)
    {
        return TransactionalTypes::INITIATIVE_PHANTOM;
    }
    else
    {
        return QString();
    }
}

//-----------------------------------------------------------------------------
// Function: TransactionalTypes::strToInitiative()
//-----------------------------------------------------------------------------
TransactionalTypes::Initiative TransactionalTypes::strToInitiative(QString const& str)
{
    if (str.compare(TransactionalTypes::INITIATIVE_REQUIRES, Qt::CaseInsensitive) == 0)
    {
        return TransactionalTypes::REQUIRES;
    }
    else if (str.compare(TransactionalTypes::INITIATIVE_PROVIDES, Qt::CaseInsensitive) == 0)
    {
        return TransactionalTypes::PROVIDES;
    }
    else if (str.compare(TransactionalTypes::INITIATIVE_BOTH, Qt::CaseInsensitive) == 0)
    {
        return TransactionalTypes::BOTH;
    }
    else if (str.compare(TransactionalTypes::INITIATIVE_PHANTOM, Qt::CaseInsensitive) == 0)
    {
        return TransactionalTypes::PHANTOM;
    }
    else
    {
        return TransactionalTypes::INITIATIVE_INVALID;
    }
}
