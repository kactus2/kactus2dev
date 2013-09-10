//-----------------------------------------------------------------------------
// File: KactusAttribute.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 23.6.2011
//
// Description:
// Kactus attribute definitions and related utility functions.
//-----------------------------------------------------------------------------

#include "KactusAttribute.h"

namespace KactusAttribute
{
    namespace
    {
        QString const PROD_HIER_NAMES[KTS_PRODHIER_COUNT] =
        {
            "Global",
            "Product",
            "Board",
            "Chip",
            "SoC",
            "IP"
        };

        QString const FIRMNESS_NAMES[KTS_REUSE_LEVEL_COUNT] =
        {
            "Mutable",
            "Template",
            "Fixed"
        };

        QString const IMPLEMENTATION_NAMES[KTS_IMPLEMENTATION_COUNT] =
        {
            "HW",
            "SW",
            "SYS"
        };
    }

    //-----------------------------------------------------------------------------
    // Function: valueToString()
    //-----------------------------------------------------------------------------
    QString valueToString(ProductHierarchy prodHier)
    {
        return PROD_HIER_NAMES[prodHier];
    }

    //-----------------------------------------------------------------------------
    // Function: stringToValue()
    //-----------------------------------------------------------------------------
    bool stringToValue(QString const& str, KactusAttribute::ProductHierarchy& val)
    {
        QString lowStr = str.toLower();

        for (unsigned int i = 0; i < KTS_PRODHIER_COUNT; ++i)
        {
            if (lowStr == PROD_HIER_NAMES[i].toLower())
            {
                val = static_cast<ProductHierarchy>(i);
                return true;
            }
        }

        return false;
    }

    //-----------------------------------------------------------------------------
    // Function: valueToString()
    //-----------------------------------------------------------------------------
    QString valueToString(Firmness firmness)
    {
        return FIRMNESS_NAMES[firmness];
    }

    //-----------------------------------------------------------------------------
    // Function: stringToValue()
    //-----------------------------------------------------------------------------
    bool stringToValue(QString const& str, KactusAttribute::Firmness& val)
    {
        QString lowStr = str.toLower();

        for (unsigned int i = 0; i < KTS_REUSE_LEVEL_COUNT; ++i)
        {
            if (lowStr == FIRMNESS_NAMES[i].toLower())
            {
                val = static_cast<Firmness>(i);
                return true;
            }
        }

        return false;
    }

    //-----------------------------------------------------------------------------
    // Function: valueToString()
    //-----------------------------------------------------------------------------
    QString valueToString(Implementation implementation)
    {
        return IMPLEMENTATION_NAMES[implementation];
    }

    //-----------------------------------------------------------------------------
    // Function: stringToValue()
    //-----------------------------------------------------------------------------
    bool stringToValue(QString const& str, KactusAttribute::Implementation& val)
    {
        QString lowStr = str.toLower();

        for (unsigned int i = 0; i < KTS_IMPLEMENTATION_COUNT; ++i)
        {
            if (lowStr == IMPLEMENTATION_NAMES[i].toLower())
            {
                val = static_cast<Implementation>(i);
                return true;
            }
        }

        return false;
    }
}
