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

    Range getRange() const;

    void setRange(Range const& range);

	QString getDefaultValue() const;

	void setDefaultValue(QString const& value);

private:

	Range range_{ QString(), QString() };

    QString defaultValue_;

	//! ClockDriver not supported.
	
	//! SingleShotDriver not supported.
};



#endif // DRIVER_H
