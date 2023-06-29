//-----------------------------------------------------------------------------
// File: MirroredTargetInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 29.06.2023
//
// Description:
// Implementation of ipxact:mirroredTarget in bus interface.
//-----------------------------------------------------------------------------

#ifndef MIRRORED_TARGET_INTERFACE_H_
#define MIRRORED_TARGET_INTERFACE_H_

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QMap>
#include <QList>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Implementation of ipxact:mirroredTarget in bus interface.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT MirroredTargetInterface
{
public:

	//! Implementation of ipxact:remapAddress element in mirrored target.
	struct RemapAddress
    {
		//! The address offset to apply to the connected target interface.
		QString remapAddress_;

		//! The remap state for which the remapAddress and range apply.		 
		QString state_;

		//! Any other attributes specified for the remapAddress.		 
		QMap<QString, QString> remapAttributes_;

		//! The mode references.
		QStringList modeRefs_;

		//! The mode priorities.
		QMap<QString, unsigned int> priorities_;

		/*! The constructor.
		 *
		 *      @param [in] remapAddress The remap address to set.
		*/
		IPXACTMODELS_EXPORT explicit RemapAddress(QString const& remapAddress);
	};

	//! The default constructor.
	MirroredTargetInterface();

	//! Copy constructor
	MirroredTargetInterface(const MirroredTargetInterface& other);

	//! Assignment operator
	MirroredTargetInterface& operator=(const MirroredTargetInterface& other);

	/*! The destructor
	 *
	 */
	~MirroredTargetInterface();

	/*! Get the range of the mirrored target interface.
	 *
	 *      @return The range.
	 */
	QString getRange() const;

	/*! Set the range for this mirrored target interface.
	 *
	 *      @param [in] range The range of the mirrored target interface.
	 */
	void setRange(QString const& range);

	/*!
	 *  Get the remap addresses for the mirrored target interface.
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

	//!  Specifies the address range to apply to the connected target interface.
	QString range_;

	//! The remap addresses for the target interface.
	QSharedPointer<QList<QSharedPointer<RemapAddress> > > remapAddresses_ =
		QSharedPointer<QList<QSharedPointer<RemapAddress> > >(new QList<QSharedPointer<RemapAddress> >);
};

#endif /* MIRRORED_TARGET_INTERFACE_H_ */
