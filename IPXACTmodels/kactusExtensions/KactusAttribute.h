//-----------------------------------------------------------------------------
// File: KactusAttribute.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 23.6.2011
//
// Description:
// Kactus attribute definitions and related utility functions.
//-----------------------------------------------------------------------------

#ifndef KACTUSATTRIBUTE_H
#define KACTUSATTRIBUTE_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>

//-----------------------------------------------------------------------------
//! KactusAttribute namespace for encapsulating Kactus 2 -specific
//! attribute values.
//-----------------------------------------------------------------------------
namespace KactusAttribute
{
    //-----------------------------------------------------------------------------
    //! ProductHierarchy enumeration.
    //-----------------------------------------------------------------------------
    enum ProductHierarchy
    {
        FLAT = 0,
        PRODUCT,
        BOARD,
        CHIP,
        SOC,
        IP,
        KTS_PRODHIER_COUNT
    };

    /*!
     *  Converts the given product hierarchy enum value to a string.
     *
     *      @param [i] prodHier The product hierarchy.
     *
     *      @return The equivalent string.
     */
    IPXACTMODELS_EXPORT QString valueToString(ProductHierarchy prodHier);

    /*!
     *  Converts the given string to a product hierarchy enum value.
     *
     *      @param [in]  str  The product hierarchy string.
     *      @param [out] val  The equivalent enum value.
     *
     *      @return False if the string does not match any value; otherwise true.
     */
    bool stringToValue(QString const& str, KactusAttribute::ProductHierarchy& val);

    //-----------------------------------------------------------------------------
    //! Firmness enumeration.
    //-----------------------------------------------------------------------------
    enum Firmness
    {
        MUTABLE = 0,
        TEMPLATE,
        FIXED,
        KTS_REUSE_LEVEL_COUNT
    };

    /*!
     *  Converts the given firmness enum value to a string.
     *
     *      @param [i] firmness specifies the firmness.
     *
     *      @return The equivalent string.
     */
    IPXACTMODELS_EXPORT QString valueToString(Firmness firmness);

    /*!
     *  Converts the given string to a firmness enum value.
     *
     *      @param [in]  str  The product hierarchy string.
     *      @param [out] val  The equivalent enum value.
     *
     *      @return False if the string does not match any value; otherwise true.
     */
    bool stringToValue(QString const& str, Firmness& val);

    //--------------------------------------------------------------------------
    //! Firmness enumeration.
    //--------------------------------------------------------------------------
    enum Implementation
    {
        HW = 0,
        SW,
        SYSTEM,
        KTS_IMPLEMENTATION_COUNT
    };

    /*!
     *  Converts the given firmness enum value to a string.
     *
     *      @param [i] firmness The firmness.
     *
     *      @return The equivalent string.
     */
    IPXACTMODELS_EXPORT QString valueToString(Implementation implementation);

    /*!
     *  Converts the given string to a firmness enum value.
     *
     *      @param [in]  str  The product hierarchy string.
     *      @param [out] val  The equivalent enum value.
     *
     *      @return False if the string does not match any value; otherwise true.
     */
    bool stringToValue(QString const& str, Implementation& val);
}

//-----------------------------------------------------------------------------

#endif // KACTUSATTRIBUTE_H
