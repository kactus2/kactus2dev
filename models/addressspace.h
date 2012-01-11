/* 
 *
 *  Created on: 28.7.2010
 *      Author: Antti Kamppi
 */

#ifndef ADDRESSSPACE_H_
#define ADDRESSSPACE_H_

#include "generaldeclarations.h"
#include "memorymap.h"

#include <QSharedPointer>
#include <QDomNode>
#include <QString>
#include <QList>
#include <QMap>
#include <QXmlStreamWriter>

class parameter;

/*! \brief Equals the spirit:addressSpace element in the IP-Xact specification
 *
 * Each AddressSpace defines a logical address space seen by a master bus
 * interface.
 */
class AddressSpace {

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
	*
	* Exception guarantee: basic
	* \exception Write_error Occurs if class or one of it's member classes is
	* not valid IP-Xact in the moment of writing.
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
	unsigned int getWidth() const;

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
	void setWidth(unsigned int width);

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
	MemoryMap* getLocalMemoryMap() const;

	/*! \brief Set the local memory map
	 *
	 * \param localMemoryMap a pointer to the new local memory map
	 */
	void setLocalMemoryMap(MemoryMap* localMemoryMap);

private:

	//! \brief MANDATORY, Identifies the address space element
	QString name_;

	//! \brief MANDATORY, address range of an address space.
	QString range_;

	//! \brief The attributes for configuring the range element.
	QMap<QString, QString> rangeAttributes_;

	//! \brief MANDATORY The bit width of a row in the address space.
	int width_;

	//! \brief The attributes for configuring the width element
	QMap<QString, QString> widthAttributes_;

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
};

#endif /* ADDRESSSPACE_H_ */
