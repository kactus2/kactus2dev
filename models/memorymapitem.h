/* 
 *
 *  Created on: 29.7.2010
 *      Author: Antti Kamppi
 */

#ifndef MEMORYMAPITEM_H_
#define MEMORYMAPITEM_H_

#include "generaldeclarations.h"

#include <QDomNode>
#include <QMap>
#include <QString>
#include <QXmlStreamWriter>

/*! \brief MemoryMapItem is a base class to be used for generalization
 *
 * This class is base class for AddressBlock, Bank and SubspaceMap classes.
 * MemoryMapItem instances dont represent any element in IP-Xact specification
 * so only it is meant to be purely a base class.
 */
class MemoryMapItem {

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

	//! \brief Copy constructor
	MemoryMapItem(const MemoryMapItem &other);

	//! \brief The assignment operator
	MemoryMapItem &operator=(const MemoryMapItem &other);

	/*! \brief The destructor
	 *
	 */
	virtual ~MemoryMapItem();

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

	/*! \brief This function writes the MemoryMapItem information to writer
	 *
	 * This function is used to print the base class information for
	 * AddressBlock, Bank and SubSpaceMap instances.
	 *
	 * \param writer A reference to a QXmlStreamWriter used to create the
	 * document.
	 *
	 * Exception guarantee: basic
	 * \exception Write_error Occurs when this class or one of it's member
	 * classes is not valid IP-Xact at the moment of writing.
	 */
	virtual void write(QXmlStreamWriter& writer);

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
