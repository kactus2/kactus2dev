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
#include <QDomNode>
#include <QSharedPointer>
#include <QXmlStreamWriter>

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

		/*! \brief MANDATORY spirit:remapAddress
		 * Specifies the address offset to apply to the connected slave
		 * interface.
		 */
		QString remapAddress_;

		/*! \brief OPTIONAL attribute spirit:prompt
		 * Contains the string for the configuration
		 */
		QString prompt_;

		/*! \brief OPTIONAL attribute spirit:state
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
		RemapAddress(QDomNode& remapNode);

		/*! \brief The constructor.
		 *
		 * \param remapAddress The remap address to set.
		 *
		*/
		RemapAddress(const QString& remapAddress);
	};

	/*! \brief The constructor
	 *
	 * \param mirrorNode A reference to a QDomNode to parse the information
	 * from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	MirroredSlaveInterface(QDomNode& mirrorNode);

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

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWriter instance that is used to
	* write the document into file.
	*/
	void write(QXmlStreamWriter& writer);

	/*! \brief Get the range of the interface
	 *
	 * \return QString containing the range.
	 */
	QString getRange() const;

	/*! \brief Get the range attributes
	 *
	 * \return Reference to QMap containing the attributes of the range
	 * element.
	 */
	const QMap<QString,QString>& getRangeAttributes();

	/*! \brief Set the range for this interface
	 *
	 * \param range A reference to a QString containing the range.
	 */
	void setRange(const QString& range);

	/*! \brief Set the range attributes
	 *
	 * \param rangeAttributes Reference to QMap containing the attributes
	 * for range.
	 */
	void setRangeAttributes(const QMap<QString,QString>& rangeAttributes);

	/*! \brief Get the list of the remapAddresses
	 *
	 * \return Reference to QList containing pointers to the RemapAddresses.
	 */
    const QList<QSharedPointer<RemapAddress> >& getRemapAddresses();

	/*! \brief Get the remap address of this mirrored slave interface for the given state.
	 * 
	 * \param state Identifies the state for which the remap address is specified.
	 * 
	 * If the state is left empty then the first remap address is returned.
	 * 
	 * \return QString containing the remap address.
	*/
	 QString getRemapAddress(const QString& state = QString()) const;

    /*! \brief Set the remapAddresses for the interface.
     *
     * \param remapAddresses A reference to QList containing pointers to the
     * RemapAddresses.
     */
    void setRemapAddresses(const QList<QSharedPointer<RemapAddress> >& remapAddresses);

	/*! \brief Set a remap address for this mirrored slave interface.
	 * 
	 * Setting this will remove any previous remap addresses.
	 * 
	 * \param remapAddress The remap address to set.
	 *
	*/
	void setRemapAddress(const QString& remapAddress);

private:

	/*! \brief MANDATORY spirit:range
	 * Specifies the address range to apply to the connected slave interface.
	 */
	QString range_;

	/*! \brief OPTIONAL Contains the attributes for the spirit:mirroredSlave
	 * key = attribute name
	 * value = attribute value
	 */
	QMap<QString, QString> rangeAttributes_;

	/*! \brief MANDATORY spirit:remapAddress
	 * Contains the pointers to the remapAddress instances.
	 */
	QList<QSharedPointer<RemapAddress> > remapAddresses_;
};

#endif /* MIRROREDSLAVEINTERFACE_H_ */
