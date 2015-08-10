//-----------------------------------------------------------------------------
// File: Document.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 05.08.2015
//
// Description:
// Base class for IP-XACT documents e.g. component and design.
//-----------------------------------------------------------------------------

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "Assertion.h"
#include "Parameter.h"

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/vlnv.h>

#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Value.h>

#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QStringList>
#include <QXmlStreamWriter>
#include <QFile>

class VendorExtension;

//-----------------------------------------------------------------------------
//! Base class for IP-XACT documents e.g. component and design.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Document
{
public:

    //! The default constructor.
	Document();

	/*!
	 *  The constructor.
	 *
	 *      @param [in] vlnv   The VLNV for the document.
	 */
	Document(VLNV const& vlnv);

    //! Copy constructor.
	Document(const Document &other);

    //! Assignment operator
	Document &operator=(const Document &other);

    //! The destructor.
	virtual ~Document();

    /*!
	 *  Creates a perfect copy of the document.
	 *
	 *      @return Copy of the document.
	 */
	virtual QSharedPointer<Document> clone() const = 0;

	/*! Get the vlnv of the element described in the document.
	 *
	 *      @return The vlnv element
	 */
	VLNV getVlnv() const;
    
	/*! Set the vlnv of the document.
	 *
	 *      @param [in] vlnv The vlnv to set.
	*/
	virtual void setVlnv(const VLNV& vlnv);
    	
	/*! Get the description of the document
	 *
	 *      @return QString containing the description of the document.
	 */
	QString getDescription() const;

	/*! Set the description of this document.
	 *
	 *      @return QString QString containing the description
	 */
	void setDescription(QString const& description);
    
    /*!
     *  Get the parameters for the document.
     *
     *      @return The parameters for the document.
     */
    QSharedPointer<QList<QSharedPointer<Parameter> > > getParameters() const;

    /*!
     *  Get the assertions.
     *
     *      @return The assertions for the document.
     */
    QSharedPointer<QList<QSharedPointer<Assertion> > > getAssertions() const;

    /*!
     *  Gets the vendor extensions.
     *     
     *      @return The vendor extensions for the document.
     */
    QSharedPointer<QList<QSharedPointer<VendorExtension> > > getVendorExtensions() const;
    
    /*!
     *  Sets the comments to display in the beginning of the XML file.
     *
     *      @param [in] comment   The comments to set.
     */
    void setTopComments(const QString& comment);

    /*!
     *  Sets the comments to display in the beginning of the XML file.
     *
     *      @param [in] comment   The comments to set.
     */
    void setTopComments(QStringList const& comments);

	/*!
	 *   Gets the comments in the beginning of the XML file.
	 *
	 *      @return The comments if any.
	 */
	QStringList getTopComments() const;

	/*! A pure virtual function to be implemented by subclasses.
	 *
	 *      @return QStringList containing the file dependencies of the document.
	 */
	virtual QStringList getDependentFiles() const = 0;

	/*! Get the dependent directories of the object. Default returns empty list.
	 *
	 *      @return empty list, re-implement in sub-classes.
	*/
	virtual QStringList getDependentDirs() const;

	/*! A pure virtual function to be implemented by subclasses.
	 *
	 *      @return The VLNVs that are needed by this document.
	 */
	virtual QList<VLNV> getDependentVLNVs()const = 0;

    /*!
     *  Set the version number kactus2 during creation of this component.
     *
     *      @param [in] versionNumber   Version of Kactus2.
     */
    void setVersion(QString versionNumber);

protected:

    bool hasKactusAttributes() const;

    bool hasImplementation() const;

    /*!
     *  Sets the implementation type for the document.
     *
     *      @param [in] implementation   The implementation type to set.
     */
    void setImplementation(KactusAttribute::Implementation implementation);

    /*!
     *  Gets the implementation type for the document.
     *
     *      @return The implementation type for the document.
     */
    KactusAttribute::Implementation getImplementation() const;

    bool hasProductHierarchy() const;

    void setHierarchy(KactusAttribute::ProductHierarchy productHierarchy);
  
    KactusAttribute::ProductHierarchy getHierarchy() const;

    bool hasFirmness() const;

    KactusAttribute::Firmness getFirmness() const;

    void setFirmness(KactusAttribute::Firmness firmness);

private:
    
    /*!
     *  Copies parameters from another document.
     *
     *      @param [in] other   The document to copy parameters from.
     */
    void copyParameters(Document const& other);
    
    /*!
     *  Copies assertions from another document.
     *
     *      @param [in] other   The document to copy assertions from.
     */
    void copyAssertions(Document const& other);

    /*!
     *  Copies vendor extensions from another document.
     *
     *      @param [in] other   The document to copy extensions from.
     */
    void copyVendorExtensions(const Document & other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    // The unique identifier for the document.
	VLNV vlnv_;

    //! The description for the document.
    QString description_;

	//! Contains the kactus2 attributes for the document.
	QMap<QString, QString> kactus2Attributes_;

	//! Contains the comment lines from the beginning of the document.
	QStringList topComments_;

    //! A list of parameters for the document.
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters_;

    //! A list of assertions for the document.
    QSharedPointer<QList<QSharedPointer<Assertion> > > assertions_;

    //! Any vendor extensions.
    QSharedPointer<QList<QSharedPointer<VendorExtension> > > vendorExtensions_;
};

#endif /* DOCUMENT_H */
