/* 
 *
 *  Created on: 29.7.2010
 *      Author: Antti Kamppi
 */

#ifndef MEMORYMAPITEM_H_
#define MEMORYMAPITEM_H_

#include "generaldeclarations.h"

#include <common/Global.h>

#include <QDomNode>
#include <QMap>
#include <QString>
#include <QXmlStreamWriter>

/*! \brief MemoryMapItem is a base class to be used for generalization
 *
 * This class is base class for AddressBlock, Bank and SubspaceMap classes.
 * MemoryMapItem instances don't represent any element in IP-Xact specification
 * so only it is meant to be purely a base class.
 */
class KACTUS2_API MemoryMapItem {

public:

	/*! \brief The constructor
	 *
	 * \param memoryMapNode A reference to a QDomNode to parse the information
	 * from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	MemoryMapItem(QDomNode &memoryMapNode);

	/*! \brief The default constructor.
	 *
	*/
	MemoryMapItem();

	//! \brief Copy constructor
	MemoryMapItem(const MemoryMapItem &other);

	//! \brief The assignment operator
	MemoryMapItem &operator=(const MemoryMapItem &other);

	/*! \brief The destructor
	 *
	 */
	virtual ~MemoryMapItem();

	/*! \brief Clone this memory map item and return pointer to the copy.
	 * 
	 * This is virtual function so it can be used to make a copy of classes that
	 * inherit MemoryMapItem.
	 *
	 * \return QSharedPointer<MemoryMapItem> Pointer to the cloned memory map item.
	*/
	virtual QSharedPointer<MemoryMapItem> clone() const = 0;

	/*! \brief This function writes the MemoryMapItem information to writer
	*
	* This function is used to print the base class information for
	* AddressBlock, Bank and SubSpaceMap instances.
	*
	* \param writer A reference to a QXmlStreamWriter used to create the
	* document.
	*/
	virtual void write(QXmlStreamWriter& writer);

	/*! \brief Check if the memory map item is in a valid state.
	 * 
	 * This is pure virtual function and should be reimplemented in all sub-classes.
	 * 
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	virtual bool isValid(QStringList& errorList, 
		const QString& parentIdentifier) const = 0;

	/*! \brief Check if the memory map item is in a valid state.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	 virtual bool isValid() const = 0;

	/*! \brief Get list of attributes for this element
	 *
	 * \return QMap containing pointers to the attributes
	 */
	virtual const QMap<QString, QString>& getAttributes();

	/*! \brief Get the attributes linked to the base address element
	 *
	 * \return QMap containing pointers to the attributes for base address
	 */
	virtual const QMap<QString, QString>& getBaseAddrAttributes();

	/*! \brief Get the base address of the element
	 *
	 * \return QString containing the base address
	 */
	virtual QString getBaseAddress() const;

	/*! \brief Get the name of the element
	 *
	 * \return QString containing the element
	 */
	virtual QString getName() const;

	/*! \brief Get the displayName of this memoryMapItem
	 *
	 * \return QString containing the displayName
	 */
	virtual QString getDisplayName() const;

	/*! \brief Get the description of this memoryMapItem
	 *
	 * \return QString containing the description.
	 */
	virtual QString getDescription() const;

	/*! \brief Set the displayName for this memoryMapItem
	 *
	 * \param name The name to be set.
	 */
	virtual void setDisplayName(const QString& name);

	/*! \brief Set the description for this memoryMapItem
	 *
	 * \param description The description to be set.
	 */
	virtual void setDescription(const QString& description);

	/*! \brief Set the attributes for the element
	 *
	 * Calling this function deletes the old attributes. If old values must
	 * be saved then function getAttributes() must first be called to
	 * add the old values to the QList given as a parameter for this function.
	 *
	 * \param attributes QMap containing pointers to the attributes
	 */
	virtual void setAttributes(const QMap<QString, QString> &attributes);

	/*! \brief Set the attributes for the base address element
	 *
	 * Calling this function deletes the old attributes. If old values must
	 * be saved then function getBaseAttributes() must first be called to
	 * add the old values to the QList given as a parameter for this function.
	 *
	 * \param baseAddressAttributes QMap containing pointers to the attributes
	 */
	virtual void setBaseAddrAttributes(const
			QMap<QString, QString> &baseAddrAttributes);

	/*! \brief Set base address for the elemen
	 *
	 * \param baseAddress QString containing the base address
	 */
	virtual void setBaseAddress(const QString &baseAddress);

	/*! \brief Set the name for the element
	 *
	 * \param name QString containing the name
	 */
	virtual void setName(const QString &name);

protected:

	/*!
	 * MANDATORY
	 * Identifies the containing element
	 */
	QString name_;

	//! \brief OPTIONAL spirit:displayName
	QString displayName_;

	//! \brief OPTIONAL spirit:description
	QString description_;

	/*!
	 * OPTIONAL
	 * Contains the attributes given to the base node
	 */
	QMap<QString, QString> attributes_;

	/*!
	 * OPTIONAL
	 * Contains the base address of the element
	 * This is mandatory in the top-level spirit:bank but not in nested
	 * banks so this is not considered as mandatory element.
	 */
	QString baseAddress_;

	/*!
	 * OPTIONAL
	 * Contains the attributes linked to the base address element.
	 */
	QMap<QString, QString> baseAddrAttributes_;
};

#endif /* MEMORYMAPITEM_H_ */
