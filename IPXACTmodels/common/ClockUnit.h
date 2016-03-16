//-----------------------------------------------------------------------------
// File: ClockUnit.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 13.10.2015
//
// Description:
// Container for a clock unit.
//-----------------------------------------------------------------------------

#ifndef CLOCKUNIT_H
#define CLOCKUNIT_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>

//-----------------------------------------------------------------------------
//! Container for a clock unit.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ClockUnit
{
public:

    //! Describes the currently selected time units.
    enum TimeUnit
    {
        NS,
        PS,
        TIMEUNIT_UNSPECIFIED
    };

    /*!
     *  The constructor.
     *
     *      @param [in] value   The contained value.
     */
    ClockUnit(QString const& value = QString());

    //! Copy constructor.
    ClockUnit(const ClockUnit& other);

    //! Assignment operator.
    ClockUnit& operator=(const ClockUnit& other);

    //! The destructor.
    ~ClockUnit();

    /*!
     *  Get the assigned value.
     *
     *      @return Value of the clock unit.
     */
    QString getValue() const;

    /*!
     *  Set the value.
     *
     *      @param [in] newValue    The new value to be set.
     */
    void setValue(QString const& newValue);

    /*!
     *  Get the used time units.
     *
     *      @return The used time unit.
     */
    TimeUnit getTimeUnit() const;

    /*!
     *  Set a new time unit.
     *
     *      @param [in] newTimeUnit     The new time unit.
     */
    void setTimeUnit(TimeUnit newTimeUnit);

    /*!
     *  Set a new time unit.
     *
     *      @param [in] newTimeUnit     String form of the new time unit.
     */
    void setTimeUnit(QString const& newTimeUnit);

    /*!
     *  Change the used time unit to string.
     *
     *      @return String containing the used time unit.
     */
    QString timeUnitToString() const;

private:

    //! The contained value.
    QString value_;

    //! The used time unit.
    TimeUnit timeUnit_;
};

#endif // CLOCKUNIT_H
