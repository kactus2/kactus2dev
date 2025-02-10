//-----------------------------------------------------------------------------
// File: Region.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 15.06.2023
//
// Description:
// Region describes the location and size of an area in memory.
//-----------------------------------------------------------------------------

#ifndef REGION_H
#define REGION_H

#include <IPXACTmodels/common/NameGroup.h>
#include <IPXACTmodels/common/Extendable.h>

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QMap>

//-----------------------------------------------------------------------------
//! Region describes the location and size of an area in memory.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Region : public NameGroup, public Extendable
{

public:

	/*!
	 *  The default constructor.
	 *
	 *      @param [in] name            Name of the region.
	 */
	Region(QString const& name = QString(), QString const& addressOffset = QString(), QString const& range = QString());
	
	//! The destructor.
	virtual ~Region();

	//! The copy constructor.
	Region(const Region& other);

	//! The assignment operator.
	Region& operator=(const Region& other);

	/*!
     *  Get the address offset of the region.
	 *
	 *      @return The address offset.
	 */
	QString getAddressOffset() const;

    /*!
     *  Set the address offset for the region.
	 *
	 *      @param [in] addressOffset   The offset within the containing memory.
	 */
	void setOffset(QString const& addressOffset);

	/*!
     *  Get the range of the region.
	 *
	 *      @return  The number of addressable units the region has.
	 */
	QString getRange() const;
    
	/*!
     *  Set the range for the region.
	 *
	 *      @param [in] range   The number of addressable units the region contains.
	 */
	void setRange(QString const& range);

private:

	//! The offset of the region from the beginning of the containing memory.
	QString addressOffset_;

	//! The range of the region in addressable units.
	QString range_;

};

#endif // REGION_H