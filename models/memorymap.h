/* 
 *
 *  Created on: 29.7.2010
 *      Author: Antti Kamppi
 */

#ifndef MEMORYMAP_H_
#define MEMORYMAP_H_

#include "memorymapitem.h"
#include "generaldeclarations.h"
#include <common/Global.h>

#include <QList>
#include <QString>
#include <QSharedPointer>
#include <QDomNode>
#include <QXmlStreamWriter>

/*! \brief Equals to the spirit:memoryMap element in IP-Xact specification
 *
 * MemoryMap contains an unbounded list of memoryMap elements
 */
class KACTUS2_API MemoryMap {

public:

	/*! \brief The constructor
	 *
	 * \param memoryMapNode A reference to the QDomNode to parse the information
	 * from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	MemoryMap(QDomNode &memoryMapNode);

	/*! \brief The default constructor.
	 *
	*/
	MemoryMap();

	//! \brief Copy constructor
	MemoryMap(const MemoryMap &other);

	//! \brief Assignment operator
	MemoryMap &operator=(const MemoryMap &other);

	/*! \brief The constructor
	 *
	 */
	~MemoryMap();

	/*! \brief Writes the memoryMap information with the writer
	*
	* Before calling this function the caller must use writer to write the
	* root element for memory map i.e spirit:memoryMap or
	* spirit:localMemoryMap. This is because this same class is used to
	* represent multiple different elements in IP-Xact specification.
	*
	* \param writer A reference to a QXmlStreamWriter instance used to write
	* the information.
	*/
	void write(QXmlStreamWriter& writer);

	/*! \brief Check if the memory map is in a valid state.
	 *
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(QStringList& errorList, 
		const QString& parentIdentifier) const;

	/*! \brief Check if the memory map is in a valid state.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid() const;

	/*! \brief Get the address unit bits value
	 *
	 * \return The address unit bit value
	 */
	unsigned int getAddressUnitBits() const;

    /*!
     *  Returns the last address of the memory map.
     */
    unsigned int getLastAddress() const;

	/*! \brief Get list of memory map items
	 *
	 * \return QList containing pointers to the memory map items stored in
	 * this instance.
	 */
	const QList<QSharedPointer<MemoryMapItem> >& getItems() const;

	/*! \brief Get list of memory map item.
	 *
	 * \return QList containing pointers to the memory map items.
	*/
	QList<QSharedPointer<MemoryMapItem> >& getItems();

	/*! \brief Get pointer to the list containing the memory map items.
	 *
	 *
	 * \return QList containing pointers to the memory map items of this memory map.
	*/
	QList<QSharedPointer<MemoryMapItem> >* getItemsPointer();

	/*! \brief Set the address unit bits for this memory map
	 *
	 * \param addressUnitBit value to be set
	 */
	void setAddressUnitBits(unsigned int addressUnitBits);

	/*! \brief Set the memorymap elements for this memory map
	 *
	 * Calling this function removes old memory map items stored and saves only
	 * the new items given in the parameter list.
	 *
	 * \param items QList containing pointers to the items to be stored
	 * in this memory map.
	 */
	void setItems(const QList<QSharedPointer<MemoryMapItem> > &items);

	/*! \brief Get the name of the memoryMap
	*
	* \return QString containing the name of the memoryMap
	*/
	QString getName() const;

	/*! \brief Set the name for the memory map
	 *
	 * \param name QString containing the name
	 */
	void setName(const QString &name);

	/*! \brief Get the display name of the memory map.
	 *
	 * \return QString containing the display name.
	*/
	QString getDisplayName() const;

	/*! \brief Set the display name for the memory map.
	 *
	 * \param displayName The display name to set.
	 *
	*/
	void setDisplayName(const QString& displayName);

	/*! \brief Get the description of the memory map.
	 *
	 * \return QString containing the description.
	*/
	QString getDescription() const;

	/*! \brief Set the description for the memory map.
	 *
	 * \param description The description to set.
	 *
	*/
	void setDescription(const QString& description);

	/*! \brief Check if the memory map contains any sub items.
	 *
	 * \return True if at least one sub item is found.
	*/
	bool isEmpty() const;

	/*! \brief Get reference to the name group-struct of the memory map.
	 *
	 * \return Reference to the name group.
	*/
	General::NameGroup& getNameGroup();

private:

	//! \brief Contains the name, display name and description of the memory map.
	General::NameGroup nameGroup_;

	/*!
	 * OPTIONAL (spirit:id)
	 * Assigns an unique identifier for the containing element.
	 */
	QString id_;

	/*!
	 * OPTIONAL
	 * Contains pointers to the memory map elements
	 */
	QList<QSharedPointer<MemoryMapItem> > items_;

	/*!
	 * OPTIONAL (spirit:addressUnitBits)
	 * Defines the number of data bits in each address increment of the memory
	 * map.
	 */
	unsigned int addressUnitBits_;

};

#endif /* MEMORYMAP_H_ */
