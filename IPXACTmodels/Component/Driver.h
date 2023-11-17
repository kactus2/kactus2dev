//-----------------------------------------------------------------------------
// File: Driver.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 7.11.2023
//
// Description:
// Implementation of ipxact:driver for wire ports.
//-----------------------------------------------------------------------------

#ifndef DRIVER_H
#define DRIVER_H

#include <IPXACTmodels/common/Range.h>

#include <IPXACTmodels/ipxactmodels_global.h>
#include <QString>

//-----------------------------------------------------------------------------
//! Implementation of ipxact:driver for wire ports.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Driver
{
public:

    /*!
     *  Create a copy of the Driver.
     *
     *      @return A Driver identical to this.
     */
	Driver* clone() const;

    /*!
     *  Get the applied range for the driver.
     *
     *      @return The range the driver applies to.
     */
    Range getRange() const;

    /*!
     *  Set the range the driver applies to
     *
     *      @param [in] range The range to set.
     */
    void setRange(Range const& range);

	/*!
	 *  Get the default value of the driver.
	 *
	 *      @return The default value.
	 */
	QString getDefaultValue() const;

	/*!
	 *  Set the default value in the driver.
	 *
	 *      @param [in] value The default value to set.
	 */
	void setDefaultValue(QString const& value);

private:

    //! The range the driver applies to.
	Range range_{ QString(), QString() };

    //! The default value of the wire.
    QString defaultValue_;

	//! ClockDriver not supported.
	
	//! SingleShotDriver not supported.
};



#endif // DRIVER_H
