//-----------------------------------------------------------------------------
// File: Structured.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 20.11.2023
//
// Description:
// Implementation of ipxact:structured in component port.
//-----------------------------------------------------------------------------

#include "Structured.h"

//-----------------------------------------------------------------------------
// Function: Structural::toString()
//-----------------------------------------------------------------------------
QString Structured::toString(Type type)
{
    if (type == Type::Struct)
    {
        return QStringLiteral("struct");
    }
    else if (type == Type::Union)
    {
        return QStringLiteral("union");
    }
    else if (type == Type::Interface)
    {
        return QStringLiteral("interface");
    }

    return QString();
}
