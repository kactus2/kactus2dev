//-----------------------------------------------------------------------------
// File: OtherClockDriverValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 14.12.2015
//
// Description:
// Validator for the ipxact:otherClockDriver.
//-----------------------------------------------------------------------------

#ifndef OTHERCLOCKDRIVERVALIDATOR_H
#define OTHERCLOCKDRIVERVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QSharedPointer>
#include <QString>
#include <QVector>

class ExpressionParser;
class OtherClockDriver;
class ClockUnit;
//-----------------------------------------------------------------------------
//! Validator for the ipxact:otherClockDriver.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT OtherClockDriverValidator
{
public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] expressionParser        The parser to use for solving expressions.
	 */
    OtherClockDriverValidator(QSharedPointer<ExpressionParser> expressionParser);

	//! The destructor.
	~OtherClockDriverValidator();
    
    /*!
     *  Validates the given other clock driver.
     *
     *      @param [in] otherClockDriver    The selected other clock driver.
     *
     *      @return True, if the other clock driver is valid IP-XACT, otherwise false.
     */
    bool validate(QSharedPointer<OtherClockDriver> otherClockDriver) const;

    /*!
     *  Check if the name is valid.
     *
     *      @param [in] name    The selected name.
     *
     *      @return True, if the name is valid, otherwise false.
     */
    bool hasValidName(QString const& name) const;

    /*!
     *  Check if a clock value is valid.
     *
     *      @param [in] clockUnit   The selected clock unit.
     *
     *      @return True, if the clock value is valid, otherwise false.
     */
    bool hasValidClockValue(QSharedPointer<ClockUnit> clockUnit) const;

    /*!
     *  Check if the clock pulse value is valid.
     *
     *      @param [in] otherClockDriver    The selected other clock driver.
     *
     *      @return True, if the pulse value is valid, otherwise false.
     */
    bool hasValidClockPulseValue(QSharedPointer<OtherClockDriver> otherClockDriver) const;

    /*!
     *  Locate errors within an other clock driver.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] otherClockDriver    The selected other clock driver.
     *      @param [in] context             Context to help locate the error.
     */
    void findErrorsIn(QVector<QString>& errors, QSharedPointer<OtherClockDriver> otherClockDriver,
        QString const& context) const;

private:

	// Disable copying.
	OtherClockDriverValidator(OtherClockDriverValidator const& rhs);
	OtherClockDriverValidator& operator=(OtherClockDriverValidator const& rhs);

    /*!
     *  Find errors within name.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] otherClockDriver    The selected other clock driver.
     *      @param [in] context             Context to help locate the error.
     */
    void findErrorsInName(QVector<QString>& errors, QSharedPointer<OtherClockDriver> otherClockDriver,
        QString const& context) const;

    /*!
     *  Find errors within clock units.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] otherClockDriver    The selected other clock driver.
     *      @param [in] context             Context to help locate the error.
     */
    void findErrorsInClockUnits(QVector<QString>& errors, QSharedPointer<OtherClockDriver> otherClockDriver,
        QString const& context) const;

    /*!
     *  Find errors within clock pulse value.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] otherClockDriver    The selected other clock driver.
     *      @param [in] context             Context to help locate the error.
     */
    void findErrorsInClockPulseValue(QVector<QString>& errors, QSharedPointer<OtherClockDriver> otherClockDriver,
        QString const& context) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;
};

#endif // OTHERCLOCKDRIVERVALIDATOR_H
