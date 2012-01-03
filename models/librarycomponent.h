/* 
 *
 *  Created on: 14.7.2010
 *      Author: Antti Kamppi
 */

#ifndef LIBRARYCOMPONENT_H_
#define LIBRARYCOMPONENT_H_

#include <LibraryManager/vlnv.h>
#include "generaldeclarations.h"

#include <QString>
#include <QList>
#include <QDomDocument>
#include <QSharedPointer>
#include <QStringList>
#include <QXmlStreamWriter>
#include <QFile>

/*! \brief LibraryComponent is a base class for the 7 IP-Xact document types
 *
 * This class is meant to be subclassed to create classes that represent models
 * of the components, designs, busdefinitions, etc.
 *
 */
class LibraryComponent {

public:

	/*! \brief The constructor
	 *
	 * This function constructs the base of the model.
	 *
	 * Exception guatantee: basic
	 * \exception Parse_error Occurs when mandatory element is missing
	 * \param doc The QDomDocument that contains the document to parse.
	 */
	LibraryComponent(QDomDocument &doc);

	/*! \brief The default constructor
	 *
	 * This constructor is used when an empty model is created.
	 */
	LibraryComponent();

	/*! \brief The constructor
	 *
	 * \param vlnv Reference to the vlnv that identifies this document.
	 *
	*/
	LibraryComponent(const VLNV &vlnv);

	//! \brief The copy constructor
	LibraryComponent(const LibraryComponent &other);

	//! \brief The assignment operator
	LibraryComponent &operator=(const LibraryComponent &other);

	/*! \brief The destructor
	 *
	 */
	virtual ~LibraryComponent();

	/*! \brief Clone this library component and return pointer to the copy.
	 * 
	 * This is virtual function so it can be used to make a copy of classes that
	 * inherit LibraryComponent.
	 *
	 * \return QSharedPointer<LibraryComponent> Pointer to the cloned library component.
	*/
	virtual QSharedPointer<LibraryComponent> clone() const = 0;

	/*! \brief Get the description of the document
	 *
	 * \return QString containing the description of the document.
	 */
	virtual QString getDescription() const;

	/*! \brief Get the vlnv of the element described in the document
	 *
	 * \return A pointer to a vlnv element
	 */
	virtual VLNV *getVlnv() const;

	/*! \brief Set the description of this document
	 *
	 * \return QString QString containing the description
	 */
	virtual void setDescription(const QString &description);

	/*! \brief Set the vlnv
	 *
	 * \param vlnv Reference to the vlnv to set
	 *
	*/
	virtual void setVlnv(const VLNV& vlnv);

	/*! \brief Find the vlnv element from the given document
	 *
	 * Exception guatantee: basic
	 * \exception Parse_error Occurs when mandatory element is missing
	 *
	 * \param doc A reference to the document to find the vlnv element from.
	 * \return vlnv instance
	 */
     static VLNV findVLNV(QDomDocument &doc);

	/*! \brief A pure virtual function to be implemented by subclasses.
	 *
	 * \return QStringList containing the file dependencies of the document.
	 */
	virtual const QStringList getDependentFiles() const = 0;

	/*! \brief A pure virtual function to be implemented by subclasses.
	 *
	 * \return QList containing VLNVs that are needed by this document.
	 */
	virtual const QList<VLNV> getDependentVLNVs()const = 0;

	/*! \brief Write the model to disk as an IP-Xact document.
	 *
	 * This function writes the base class information and is called by
	 * subclass write() implementation.
	 *
	 * \param writer A reference to a QXmlStreamWriter used to write the
	 * base class information.
	 */
	virtual void write(QXmlStreamWriter& writer);

	/*! \brief Fully abstract function that subclasses classes must implement
	 * 
	 * This function writes the entire document to given file.
	 * 
	 * \param file The file to write to document into.
	 */
	virtual void write(QFile& file) = 0;

	/*! \brief Write the VLNV information using the specified writer
	 *
	 * Call this function to write the VLNV of the base class into file using
	 * the writer. This function only writes four textElements and exits.
	 *
	 * \param writer A reference to a QXmlStreamWriter instance used to write
	 * the document into file.
	 *
	 */
	virtual void writeVLNV(QXmlStreamWriter& writer);

protected:

	/*! \brief Write the kactus2 attributes using the writer.
	 *
	 * \param writer The writer instance to use.
	 *
	*/
	virtual void writeKactus2Attributes(QXmlStreamWriter& writer);

	/*! \brief Parse the Kactus2 attributes from the xml.
	 *
	 * \param attributeNode The node that contains the attributes.
	 *
	*/
	virtual void parseKactus2Attributes(QDomNode& attributeNode);

	/*! \brief Set the attributes that define the used xml namespaces.
	 *
	 * \param attributes QMap containing the name-value attribute pairs.
	 *
	*/
	void setXMLNameSpaceAttributes(QMap<QString, QString>& attributes);

	/*!
	 * spirit:versionedIdentifier
	 * MANDATORY
	 */
	QSharedPointer<VLNV> vlnv_;

	/*!
	 * spirit:description
	 * OPTIONAL
	 */
	QString description_;

	//! \brief Contains the kactus2 attributes for the IP-Xact object.
	QMap<QString, QString> kactus2Attributes_;
};

#endif /* LIBRARYCOMPONENT_H_ */
