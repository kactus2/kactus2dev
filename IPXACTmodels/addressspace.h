/* 
 *
 *  Created on: 28.7.2010
 *      Author: Antti Kamppi
 */

#ifndef ADDRESSSPACE_H_
#define ADDRESSSPACE_H_

#include "memorymap.h"
#include "segment.h"

#include <common/Global.h>

#include <QSharedPointer>
#include <QDomNode>
#include <QString>
#include <QList>
#include <QMap>
#include <QXmlStreamWriter>

class parameter;
class VendorExtension;

/*! \brief Equals the spirit:addressSpace element in the IP-Xact specification
 *
 * Each AddressSpace defines a logical address space seen by a master bus
 * interface.
 */
class KACTUS2_API AddressSpace {

public:

	/*! \brief The constructor
	 *
	 *\param addrSpaceNode A reference to a QDomNode that can be used to parse
	 * the information from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs if mandatory IP-Xact element is missing in
	 * this class or one of it's member classes.
	 */
	AddressSpace(QDomNode &addrSpaceNode);

	/*! \brief The default constructor
	 *
	*/
	AddressSpace();

	//! \brief The copy constructor
	AddressSpace(const AddressSpace &other);

	//! \brief The assignment operator
	AddressSpace &operator=(const AddressSpace &other);

	//! \brief The destructor
	~AddressSpace();

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to writer the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWrite instance that is used to
	* write the document into file.
	*/
	void write(QXmlStreamWriter& writer);

	/*! \brief Check if the address space is in a valid state.
	 *
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(QStringList& errorList, 
		const QString& parentIdentifier) const;

	/*! \brief Check if the address space is in a valid state.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid() const;

	/*! \brief Get the address unit bits
	 *
	 * \return the amount of bits.
	 */
	unsigned int getAddressUnitBits() const;

	/*! \brief Get the name of the address space
	 *
	 * \return QString containing the name of the address space
	 */
	QString getName() const;

	/*! \brief Get the range of the address space
	 *
	 * \return QString containing the range of the address space
	 */
	QString getRange() const;

	/*! \brief Get the range attributes
	 *
	 * \return QMap containing attribute names and values for the range
	 */
	const QMap<QString, QString>& getRangeAttributes();

	/*! \brief Get the width of a row in an address space
	 *
	 * \return The width
	 */
	int getWidth() const;

	/*! \brief Get the width attributes
	 *
	 * \return QMap containing attribute names and values for the width
	 */
	const QMap<QString, QString>& getWidthAttributes();

	/*! \brief Set the address unit bits
	 *
	 * \param addressUnitBits The wanted setting
	 */
	void setAddressUnitBits(unsigned int addressUnitBits);

	/*! \brief Set the name for the address space
	 *
	 * \param name The wanted name
	 */
	void setName(const QString& name);

	/*! \brief Set the range for the address space
	 *
	 * \param range The wanted range
	 */
	void setRange(const QString &range);

	/*! \brief Set the attributes for the range
	 *
	 * Calling this function deletes the old range attributes.
	 *
	 * \param rangeAttributes A QMap containing the names and values of the
	 * range attributes
	 */
	void setRangeAttributes(const QMap<QString, QString> &rangeAttributes);

	/*! \brief Set the width of the address space
	 *
	 * \param width The wanted width
	 */
	void setWidth(int width);

	/*! \brief Set the attributes for the width
	 *
	 * Calling this function deletes the old width attributes.
	 *
	 * \param widthAttributes A QMap containing the names and values of the
	 * width attributes.
	 */
	void setWidthAttributes(const QMap<QString, QString> &widthAttributes);

	/*! \brief Get the pointer to the local memory map
	 *
	 * \return A pointer to the local memory map
	 */
	const QSharedPointer<MemoryMap> getLocalMemoryMap() const;

	/*! \brief Get the pointer to the local memory map
	 *
	 * \return A pointer to the local memory map
	 */
	QSharedPointer<MemoryMap> getLocalMemoryMap();

	/*! \brief Check if the address space contains local memory map.
	 *
	 * Method: 		hasLocalMemoryMap
	 * Full name:	AddressSpace::hasLocalMemoryMap
	 * Access:		public 
	 *
	 *
	 * \return True if a local memory map is found.
	*/
	bool hasLocalMemoryMap() const;

	/*! \brief Set the local memory map
	 *
	 * \param localMemoryMap a pointer to the new local memory map
	 */
	void setLocalMemoryMap(QSharedPointer<MemoryMap> localMemoryMap);

	/*! \brief Set the display name for the address space.
	 *
	 * \param dispName The display name to set.
	 *
	*/
	void setDisplayName(const QString& dispName);

	/*! \brief Get the display name of the address space.
	 *
	 * \return QString contains the display name.
	*/
	QString getDisplayName() const;

	/*! \brief Set the description for the address space.
	 *
	 * \param description contains the description to set.
	 *
	*/
	void setDescription(const QString& description);

	/*! \brief Get the description of the address space.
	 *
	 * \return QString contains the description of the address space.
	*/
	QString getDescription() const;

	/*! \brief Get the parameters of the address space.
	 *
	 * \return QList containing pointers to the parameters.
	*/
	QList<QSharedPointer<Parameter> >& getParameters();

	/*! \brief Get the parameters of the address space.
	 *
	 * \return QList containing pointers to the parameters.
	*/
	const QList<QSharedPointer<Parameter> >& getParameters() const;

	/*! \brief Get the name group that contains the name, display name and description.
	 *
	 * \return Reference to the group.
	*/
	NameGroup& getNameGroup();

	/*! \brief Get the name group that contains the name, display name and description.
	 *
	 * \return Reference to the group.
	*/
	const NameGroup& getNameGroup() const;

	/*! \brief Get the segments contained in this address space.
	*
	* \return QList containing pointers to the segments.
	*/
	QList<QSharedPointer<Segment> >& getSegments();

	/*! \brief Get the segments contained in this address space.
	*
	* \return QList containing pointers to the segments.
	*/
	const QList<QSharedPointer<Segment> >& getSegments() const;

	/*! \brief Set the segments for this address space.
	*
	* \param segments QList containing pointers to set for the address space.
	*/
	void setSegments(const QList<QSharedPointer<Segment> >& segments);

	/*! \brief Get the last address of the address space that has segment assigned to it.
	 * 
	 * If there are no segments defined then 0 is returned.
	 * 
	 * \return The last address contained in a segment.
	*/
	quint64 getLastSegmentedAddress() const;

	/*! \brief Get the last address contained in the address space.
	 * 
	 * The last address is defined by the range specified for address space.
	 * 
	 * \return The last address of the address space.
	*/
	quint64 getLastAddress() const;

private:

	//! \brief Contains the name and description.
	NameGroup nameGroup_;

	//! \brief MANDATORY, address range of an address space.
	QString range_;

	//! \brief The attributes for configuring the range element.
	QMap<QString, QString> rangeAttributes_;

	//! \brief MANDATORY The bit width of a row in the address space.
	int width_;

	//! \brief The attributes for configuring the width element
	QMap<QString, QString> widthAttributes_;

	//! \brief OPTIONAL Contains the segments of the address space
	QList<QSharedPointer<Segment> > segments_;

	/*! \brief The number of data bits in each address increment.
	 * OPTIONAL
	 * (default value 8)
	 */
	unsigned int addressUnitBits_;

	/*! \brief Block of memory that can only be accessed by the master interfaces
	 * OPTIONAL spirit:localMemoryMap
	 */
	QSharedPointer<MemoryMap> localMemoryMap_;

	//! \brief Contains the parameters for the addressSpace
	//! OPTIONAL spirit:parameters
	QList<QSharedPointer<Parameter> > parameters_;

    /*!
	 * OPTIONAL (spirit: vendorExtensions)
	 * Address space vendor extensions.
	 */
    QList<QSharedPointer<VendorExtension> > vendorExtensions_;
};

#endif /* ADDRESSSPACE_H_ */
