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
        KTS_GLOBAL = 0,
        KTS_PRODUCT,
        KTS_BOARD,
        KTS_CHIP,
        KTS_SOC,
        KTS_IP,
        KTS_PRODHIER_COUNT
    };

    /*!
     *  Converts the given product hierarchy enum value to a string.
     *
     *      @param [i] prodHier The product hierarchy.
     *
     *      @return The equivalent string.
     */
    QString valueToString(ProductHierarchy prodHier);

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
        KTS_TEMPLATE = 0,
        KTS_MUTABLE,
        KTS_PARAMETERIZABLE,
        KTS_FIXED,
        KTS_REUSE_LEVEL_COUNT
    };

    /*!
     *  Converts the given firmness enum value to a string.
     *
     *      @param [i] firmness specifies the firmness.
     *
     *      @return The equivalent string.
     */
    QString valueToString(Firmness firmness);

    /*!
     *  Converts the given string to a firmness enum value.
     *
     *      @param [in]  str  The product hierarchy string.
     *      @param [out] val  The equivalent enum value.
     *
     *      @return False if the string does not match any value; otherwise true.
     */
    bool stringToValue(QString const& str, Firmness& val);

    //-----------------------------------------------------------------------------
    //! Firmness enumeration.
    //-----------------------------------------------------------------------------
    enum Implementation
    {
        KTS_HW = 0,
        KTS_SW,
        KTS_SYS,
        KTS_FIRMNESS_COUNT
    };

    /*!
     *  Converts the given firmness enum value to a string.
     *
     *      @param [i] firmness The firmness.
     *
     *      @return The equivalent string.
     */
    QString valueToString(Implementation implementation);

    /*!
     *  Converts the given string to a firmness enum value.
     *
     *      @param [in]  str  The product hierarchy string.
     *      @param [out] val  The equivalent enum value.
     *
     *      @return False if the string does not match any value; otherwise true.
     */
    bool stringToValue(QString const& str, Implementation& val);

    //-----------------------------------------------------------------------------
    //! SWType enumeration.
    //-----------------------------------------------------------------------------
    enum SWType
    {
        KTS_SW_MAPPING = 0,
        KTS_SW_APPLICATION,
        KTS_SW_ENDPOINTS,
        KTS_SW_PLATFORM,
        KTS_SW_TYPE_COUNT
    };

    /*!
     *  Converts the given SW type enum value to a string.
     *
     *      @param [i] swType The SW type.
     *
     *      @return The equivalent string.
     */
    QString valueToString(SWType swType);

    /*!
     *  Converts the given string to a SW type enum value.
     *
     *      @param [in]  str  The SW type string.
     *      @param [out] val  The equivalent enum value.
     *
     *      @return False if the string does not match any value; otherwise true.
     */
    bool stringToValue(QString const& str, SWType& val);

    //-----------------------------------------------------------------------------
    //! BusDefType enumeration.
    //-----------------------------------------------------------------------------
    enum BusDefType
    {
        KTS_BUSDEF_HW = 0,
        KTS_BUSDEF_API,
        KTS_BUSDEF_MCAPI,
        KTS_BUSDEF_TYPE_COUNT
    };

    /*!
     *  Converts the given busdef type enum value to a string.
     *
     *      @param [i] busDefType The bus definition type.
     *
     *      @return The equivalent string.
     */
    QString valueToString(BusDefType busDefType);

    /*!
     *  Converts the given string to a busdef type enum value.
     *
     *      @param [in]  str  The bus definition type string.
     *      @param [out] val  The equivalent enum value.
     *
     *      @return False if the string does not match any value; otherwise true.
     */
    bool stringToValue(QString const& str, BusDefType& val);
}

//-----------------------------------------------------------------------------

#endif // KACTUSATTRIBUTE_H
