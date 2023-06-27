//-----------------------------------------------------------------------------
// File: Utils.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 9.5.2012
//
// Description:
// Common utilities for writing/parsing IP-XACT documents.
//-----------------------------------------------------------------------------

#include "XmlUtils.h"

#include <QSharedPointer>

//-----------------------------------------------------------------------------

namespace XmlUtils
{
    //-----------------------------------------------------------------------------
    // Function: removeWhiteSpace()
    //-----------------------------------------------------------------------------
    IPXACTMODELS_EXPORT QString removeWhiteSpace(QString const& str)
    {
        return str.simplified().replace(QLatin1Char(' '), QLatin1Char('_'));
    }

}
