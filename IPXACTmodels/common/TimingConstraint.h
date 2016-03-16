//-----------------------------------------------------------------------------
// File: TimingConstraint.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 12.08.2015
//
// Description:
// Implementation for ipxact:timingConstraint.
//-----------------------------------------------------------------------------

#ifndef TIMINGCONSTRAINT_H
#define TIMINGCONSTRAINT_H

#include <QString>

//-----------------------------------------------------------------------------
//! Implementation for ipxact:timingConstraint.
//-----------------------------------------------------------------------------
class TimingConstraint 
{
public:

    //! Possible values for clock edge.
    enum ClockEdgeType
    {
        RISE = 0,
        FALL, 
        UNKNOWN
    };

    //! Possible values for delay type.
    enum DelayType
    {
        MINIMUM_DELAY = 0,
        MAXIMUM_DELAY, 
        BOTH
    };

	//! The constructor.
	TimingConstraint();

    //! Copy constructor.
    TimingConstraint(TimingConstraint const& other);

	//! The destructor.
	~TimingConstraint();

    /*!
     *  Sets the constraint value.
     *
     *      @param [in] delayPercentage   The value percentage.
     */
    void setValue(double delayPercentage);

    /*!
     *  Gets the constraint value.
     *
     *      @return The constraint value.
     */
    double getValue() const;

    /*!
     *  Sets the clock edge for the constraint.
     *
     *      @param [in] edge   The edge to set.
     */
    void setClockEdge(TimingConstraint::ClockEdgeType edge);
      
    /*!
     *  Gets the clock edge.
     *
     *      @return The clock edge type.
     */
    ClockEdgeType getClockEdge() const;

    /*!
     *  Sets the delay type for the constraint.
     *
     *      @param [in] delayType   The delay type to set.
     */
    void setDelayType(TimingConstraint::DelayType delayType);

    /*!
     *  Gets the delay type.
     *
     *      @return The delay type.
     */
    DelayType getDelayType() const;
        
    /*!
     *  Sets the clock name for the constraint.
     *
     *      @param [in] clockPort   The clock name to set.
     */
    void setClockName(QString const& clockPort);

    /*!
     *  Gets the clock name for the constraint.
     *
     *      @return The clock name.
     */
    QString getClockName() const;

private:

	// Disable copying.
	TimingConstraint& operator=(TimingConstraint const& rhs);

    //! The constraint value in percentage.
    double value_;

    //! The clock edge this constraint applies to.
    ClockEdgeType clockEdge_;

    //! Specifies the constraint applicability for timing analysis.
    DelayType delayType_;

    //! Specifies which clock the constraint is relative to.
    QString clockPortName_;
};

#endif // TIMINGCONSTRAINT_H