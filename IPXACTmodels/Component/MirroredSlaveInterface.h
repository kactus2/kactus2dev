//-----------------------------------------------------------------------------
// File: mirroredslaveinterface.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 20.10.2015
//
// Description:
// Implementation of ipxact:mirroredSlave in bus interface.
//-----------------------------------------------------------------------------

#ifndef MIRROREDSLAVEINTERFACE_H_
#define MIRROREDSLAVEINTERFACE_H_

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QMap>
#include <QList>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Implementation of ipxact:mirroredSlave in bus interface.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT MirroredSlaveInterface
{
public:

	//! Implementation of ipxact:remapAddress element in mirrored slave.
	struct RemapAddress
    {
		//! The address offset to apply to the connected slave interface.
		QString remapAddress_;

		//! The remap state for which the remapAddress and range apply.		 
		QString state_;

		//! Any other attributes specified for the remapAddress.		 
		QMap<QString, QString> remapAttributes_;

		/*! The constructor.
		 *
		 *      @param [in] remapAddress The remap address to set.
		*/
		IPXACTMODELS_EXPORT RemapAddress(QString const& remapAddress);
	};

	//! The default constructor.
	MirroredSlaveInterface();

	//! Copy constructor
	MirroredSlaveInterface(const MirroredSlaveInterface& other);

	//! Assignment operator
	MirroredSlaveInterface& operator=(const MirroredSlaveInterface& other);

	/*! The destructor
	 *
	 */
	~MirroredSlaveInterface();

	/*! Get the range of the mirrored slave interface.
	 *
	 *      @return The range.
	 */
	QString getRange() const;

	/*! Set the range for this mirrored slave interface.
	 *
	 *      @param [in] range The range of the mirrored slave interface.
	 */
	void setRange(QString const& range);

	/*!
	 *  Get the remap addresses for the mirrored slave interface.
	 *
	 *      @return The remap addresses.
	 */
	QSharedPointer<QList<QSharedPointer<RemapAddress> > > getRemapAddresses() const;

    /*!
     *  Set the remap address.
     *
     *      @param [in] newRemapAddress     The new remap address.
     */
    void setRemapAddress(QString const& newRemapAddress);

private:

	//!  Specifies the address range to apply to the connected slave interface.
	QString range_;

	//! The remap addresses for the slave interface.
	QSharedPointer<QList<QSharedPointer<RemapAddress> > > remapAddresses_;
};

#endif /* MIRROREDSLAVEINTERFACE_H_ */
