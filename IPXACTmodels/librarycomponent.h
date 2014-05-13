/* 
 *
 *  Created on: 14.7.2010
 *      Author: Antti Kamppi
 */

#ifndef LIBRARYCOMPONENT_H_
#define LIBRARYCOMPONENT_H_

#include <library/LibraryManager/vlnv.h>
#include "generaldeclarations.h"

#include <common/Global.h>

#include <QString>
#include <QList>
#include <QDomDocument>
#include <QSharedPointer>
#include <QStringList>
#include <QXmlStreamWriter>
#include <QFile>

class VendorExtension;

/*! \brief LibraryComponent is a base class for the 7 IP-Xact document types
 *
 * This class is meant to be subclassed to create classes that represent models
 * of the components, designs, busdefinitions, etc.
 *
 */
class KACTUS2_API LibraryComponent {

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

	/*! \brief Check the validity of the library component.
	 * 
	 * This function should be used to check if the component is in valid state
	 * before writing the xml to the disk.
	 * 
	 * This is a pure virtual function and is implemented in all sub classes of 
	 * library component so they can check themselves. This function can not 
	 * check the validity of the references to objects outside this item. For 
	 * example it can not check if the vlnv reference is to an object that exists.
	 * 
	 * \param errorList The error messages of the detected errors are appended to
	 * this list.
	 *
	 * \return bool True if the library component was valid.
	*/
	virtual bool isValid(QStringList& errorList) const = 0;

	/*! \brief Check the validity of the library component.
	 * 
	 * This function should be used to check if the component is in valid state
	 * before writing the xml to the disk.
	 * 
	 * This is a pure virtual function and is implemented in all sub classes of 
	 * library component so they can check themselves. This function can not 
	 * check the validity of the references to objects outside this item. For 
	 * example it can not check if the vlnv reference is to an object that exists.
	 * 
	 * \return bool True if the state is valid and writing is possible.
	*/
	virtual bool isValid() const = 0;

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

	/*! \brief Get the dependent directories of the object. Default returns empty list.
	 *
	 * Method: 		getDependentDirs
	 * Full name:	LibraryComponent::getDependentDirs
	 * Access:		virtual public 
	 *
	 *
	 * \return empty list, reimplement in sub-classes.
	*/
	virtual const QStringList getDependentDirs() const;

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

	/*! \brief Get the comments from the beginning of the XML file.
	 *
	 * Method: 		getTopComments
	 * Full name:	LibraryComponent::getTopComments
	 * Access:		virtual public 
	 *
	 *
	 * \return QStringList containing the XML header comment lines.
	*/
	virtual QStringList getTopComments() const;

	/*! \brief Set the top header comments for the XML file.
	 *
	 * Method: 		setTopComments
	 * Full name:	LibraryComponent::setTopComments
	 * Access:		virtual public 
	 *
	 * \param comment QString containing the comments, if string contains line breaks it chopped to separate strings.
	 *
	*/
	virtual void setTopComments(const QString& comment);

	/*! \brief Set the top header comments for the XML file.
	 *
	 * Method: 		setTopComments
	 * Full name:	LibraryComponent::setTopComments
	 * Access:		virtual public 
	 *
	 * \param comments QStringList which contains the comment lines of the XML header.
	 *
	*/
	virtual void setTopComments(const QStringList& comments);

    /*!
     *  Sets the vendor extensions in the library component.
     *
     *      @param [in] vendorExtensions   The vendor extensions to set.
     */
    void setVendorExtensions(QList<QSharedPointer<VendorExtension> > vendorExtensions);

    /*!
     *  Gets the vendor extensions.
     *     
     *      @return The vendor extensions of the library component.
     */
    QList<QSharedPointer<VendorExtension> > getVendorExtensions() const;

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
     *  Writes the component vendor extensions to XML.
     *
     *      @param [in] writer   The XML writer used in writing.     
     */
    void writeVendorExtensions(QXmlStreamWriter& writer) const;

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

	//! \brief Contains the comment lines from the beginning of the document.
	QStringList topComments_;

    //! Component vendor extensions.
    QList<QSharedPointer<VendorExtension> > vendorExtensions_;
};

#endif /* LIBRARYCOMPONENT_H_ */
