//-----------------------------------------------------------------------------
// File: OtherClockDriver.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: 
// Date: 
//
// Description:
// Class for ipxact:otherClockDriver element.
//-----------------------------------------------------------------------------

#ifndef OTHERCLOCKDRIVER_H
#define OTHERCLOCKDRIVER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/ClockUnit.h>

#include <QList>
#include <QString>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Class for ipxact:otherClockDriver element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT OtherClockDriver
{

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] clockName   The name of the clock driver.
	 */
	OtherClockDriver(QString const& clockName = QString());

	//! Copy constructor.
	OtherClockDriver(const OtherClockDriver &other);

	//! Assignment operator.
	OtherClockDriver &operator=(const OtherClockDriver &other);

	/*!
     *  The destructor.
	 */
	~OtherClockDriver();

    /*!
     *  Get the name of the clock.
	 *
	 *      @return QString containing the name
	 */
	QString getClockName() const;
    
	/*!
     *  Set the clock name.
	 *
	 *      @param [in] clockName   The new name of the clock.
	 */
	void setClockName(QString const& clockName);
    
	/*!
     *  Get the clock source.
	 *
	 *      @return QString containing the path and name of the clock generation cell.
	 */
	QString getClockSource() const;
    
	/*!
     *  Set the clock source.
	 *
	 *      @param [in] clockSource     QString containing the name and path of the clock generation cell.
	 */
	void setClockSource(QString const& clockSource);

	/*!
     *  Get the clock period.
	 *
	 *      @return A pointer to the clock period.
	 */
    QSharedPointer<ClockUnit> getClockPeriod() const;

	/*!
     *  Set the clock period.
	 *
	 *      @param [in] clockPeriod     A pointer to the new clock period.
	 */
    void setClockPeriod(QSharedPointer<ClockUnit> newClockPeriod);

	/*!
     *  Get the clock pulse offset.
	 *
	 *      @return Pointer to the clock pulse offset.
	 */
    QSharedPointer<ClockUnit> getClockPulseOffset() const;
    
	/*!
     *  Set the clock pulse offset.
	 *
	 *      @param [in] clockPulseOffset    Pointer to the new clock pulse offset.
	 */
    void setClockPulseOffset(QSharedPointer<ClockUnit> newClockPulseOffset);

	/*!
     *  Get the clock pulse value.
	 *
	 *      @return The clock pulse value.
	 */
    QString getClockPulseValue() const;
    
	/*!
     *  Set the clock pulse value.
	 *
	 *      @param [in]     The new clock pulse value.
	 */
    void setClockPulseValue(QString const& newClockPulseValue);

	/*!
     *  Get the clock pulse duration.
	 *
	 *      @return A pointer to the new clock pulse duration.
	 */
    QSharedPointer<ClockUnit> getClockPulseDuration() const;

	/*!
     *  Set the clock pulse duration.
	 *
	 *      @param [in] clockPulseDuration  A pointer to the new clock pulse duration.
	 */
    void setClockPulseDuration(QSharedPointer<ClockUnit> newClockPulseDuration);

private:

    /*!
     *  Copy the clock data.
     *
     *      @param [in] other   The other clock driver being copied.
     */
    void copyClockData(const OtherClockDriver &other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The name of the clock for reference by a constraint.
	QString clockName_;

	//! Defines the physical path and name of the clock generation cell.
	QString clockSource_;

	//! Specifies the overall length of one cycle of the waveform.
    QSharedPointer<ClockUnit> clockPeriod_;

	//! Specifies the time delay from the start of the waveform to the first transition.
    QSharedPointer<ClockUnit> clockPulseOffset_;

	//! Specifies the logic value to which the port transitions.
    QString clockPulseValue_;

	//! Specifies how long the waveform remains at the value specified by clockPulseValue.
    QSharedPointer<ClockUnit> clockPulseDuration_;
};

#endif // OTHERCLOCKDRIVER_H
