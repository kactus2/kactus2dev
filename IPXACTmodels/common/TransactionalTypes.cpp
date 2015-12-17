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
        if (QString(TransactionalTypes::KIND_TYPES[i]) == kindType)
        {
            return true;
        }
    }

    return false;
}