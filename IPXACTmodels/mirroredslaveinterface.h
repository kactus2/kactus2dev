/* 
 *
 *  Created on: 31.8.2010
 *      Author: Antti Kamppi
 */

#ifndef MIRROREDSLAVEINTERFACE_H_
#define MIRROREDSLAVEINTERFACE_H_

#include "ipxactmodels_global.h"

#include <QString>
#include <QMap>
#include <QList>
#include <QSharedPointer>

/*! \brief Equals the spirit:mirroredSlave element in IP-Xact specification
 *
 * MirroredSlave can provide addition address offsets connected to the
 * slave interface.
 */
class IPXACTMODELS_EXPORT MirroredSlaveInterface {

public:

	/*! \brief Equals the spirit:remapAddress element in IP-Xact specification
	 *
	 */
	struct RemapAddress {

		/*! \brief MANDATORY ipxact:remapAddress
		 * Specifies the address offset to apply to the connected slave
		 * interface.
		 */
		QString remapAddress_;

		/*! \brief OPTIONAL attribute state
		 * Identifies remap state for which the remapAddress and range apply.
		 */
		QString state_;

		/*! \brief Contains the attributes for the spirit:remapAddress
		 * key = attribute name
		 * value = attribute value
		 */
		QMap<QString, QString> remapAttributes_;

		/*! \brief The constructor
		 *
		 * \param remapNode A reference to a QDomNode to parse the information
		 * from.
		 *
		 * Exception guarantee: basic
		 * \exception Parse_error Occurs when a mandatory element is missing in
		 * this class or one of it's member classes.
		 */
		RemapAddress(){}

		/*! \brief The constructor.
		 *
		 * \param remapAddress The remap address to set.
		 *
		*/
		RemapAddress(const QString& remapAddress);
	};

	/*! \brief The default constructor
	 *
	*/
	MirroredSlaveInterface();

	//! \brief Copy constructor
	MirroredSlaveInterface(const MirroredSlaveInterface& other);

	//! \brief Assignment operator
	MirroredSlaveInterface& operator=(const MirroredSlaveInterface& other);

	/*! \brief The destructor
	 *
	 */
	~MirroredSlaveInterface();

	/*! \brief Get the range of the interface
	 *
	 * \return QString containing the range.
	 */
	QString getRange() const;

	/*! \brief Set the range for this interface
	 *
	 * \param range A reference to a QString containing the range.
	 */
	void setRange(const QString& range);

	/*! \brief Get the remap address of this mirrored slave interface for the given state.
	 * 
	 * \param state Identifies the state for which the remap address is specified.
	 * 
	 * If the state is left empty then the first remap address is returned.
	 * 
	 * \return QString containing the remap address.
	*/
	QList<QSharedPointer<RemapAddress> > getRemapAddresses() const;

	/*! \brief Set a remap address for this mirrored slave interface.
	 * 
	 * Setting this will remove any previous remap addresses.
	 * 
	 * \param remapAddress The remap address to set.
	 *
	*/
	void setRemapAddresses(const QList<QSharedPointer<RemapAddress> > remapAddresses);

private:

	/*! \brief MANDATORY spirit:range
	 * Specifies the address range to apply to the connected slave interface.
	 */
	QString range_;

	/*! \brief MANDATORY spirit:remapAddress
	 * Contains the pointers to the remapAddress instances.
	 */
	QList<QSharedPointer<RemapAddress> > remapAddresses_;
};

#endif /* MIRROREDSLAVEINTERFACE_H_ */
