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
#include <IPXACTmodels/common/VLNV.h>

#include <IPXACTmodels/common/Extendable.h>

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
class IPXACTMODELS_EXPORT Document : public Extendable
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
     *  Check if the document contains any parameters.
     *
     *      @return True, if parameters are found, false otherwise.
     */
    bool hasParameters() const;

    /*!
     *  Get the assertions.
     *
     *      @return The assertions for the document.
     */
    QSharedPointer<QList<QSharedPointer<Assertion> > > getAssertions() const;
    
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
        
    /*!
     *  Adds a new XML processing instruction for the document.
     *
     *      @param [in] target  The target for the instruction.
     *      @param [in] data    The data in the instruction.
     */
    void addXmlProcessingInstructions(QString const& target, QString const& data);

    /*!
     *  Gets the XML processing instructions for the document.
     *
     *      @return The XML processing instructions as target-data pairs.
     */
    QVector<QPair<QString, QString> > getXmlProcessingInstructions() const;

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
     *  Set the version number kactus2 during creation of this document.
     *
     *      @param [in] versionNumber   Version of Kactus2.
     */
    void setVersion(QString const& versionNumber);

    /*!
     *  Get the version number of Kactus2 used for creating this document.
     *
     *      @return  The used version of Kactus2.
     */
    QString getVersion() const;

    /*!
     *  Check if the document has Kactus attributes.
     *
     *      @return True, if the document has Kactus attributes, otherwise false.
     */
    bool hasKactusAttributes() const;

    /*!
     *  Check if the document contains an implementation Kactus attribute.
     *
     *      @return True, if an implementation attribute exists, otherwise false.
     */
    bool hasImplementation() const;

    /*!
     *  Sets the implementation type for the document.
     *
     *      @param [in] implementation   The new implementation type.
     */
    void setImplementation(KactusAttribute::Implementation implementation);

    /*!
     *  Gets the implementation type for the document.
     *
     *      @return The implementation type for the document.
     */
    KactusAttribute::Implementation getImplementation() const;

    /*!
     *  Check if the document has a determined product hierarchy.
     *
     *      @return True, if a product hierarchy exists, otherwise false.
     */
    bool hasProductHierarchy() const;

    /*!
     *  Set a new product hierarchy.
     *
     *      @param [in] productHierarchy    The new product hierarchy value.
     */
    void setHierarchy(KactusAttribute::ProductHierarchy productHierarchy);
  
    /*!
     *  Get the product hierarhcy of the document.
     *
     *      @return The product hierarchy of the document.
     */
    KactusAttribute::ProductHierarchy getHierarchy() const;

    /*!
     *  Check if the document has a firmness attribute.
     *
     *      @return True, if a firmness attribute exists, otherwise false.
     */
    bool hasFirmness() const;

    /*!
     *  Get the firmness attribute.
     *
     *      @return The firmness value of the document.
     */
    KactusAttribute::Firmness getFirmness() const;

    /*!
     *  Set the firmness attribute.
     *
     *      @param [in] firmness    The new firmness value.
     */
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

	//! Contains the comment lines from the beginning of the document.
	QStringList topComments_;

    //! Contains the xml processing instructions as target-data pairs.
    QVector<QPair<QString, QString> > xmlProcessingInstructions_;

    //! A list of parameters for the document.
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters_;

    //! A list of assertions for the document.
    QSharedPointer<QList<QSharedPointer<Assertion> > > assertions_;

};

#endif /* DOCUMENT_H */
