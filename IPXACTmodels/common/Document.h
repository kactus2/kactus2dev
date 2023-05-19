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
#include <IPXACTmodels/common/TagData.h>

#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Value.h>

#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QStringList>
#include <QXmlStreamWriter>
#include <QFile>

class VendorExtension;
class Kactus2Group;

//-----------------------------------------------------------------------------
//! Base class for IP-XACT documents e.g. component and design.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Document : public Extendable
{
public:

    //! IP-XACT standard revision information.
    enum class Revision
    {
        Std14, Std22, Unknown
    };

    //! The default constructor.
	explicit Document(Revision revision = Revision::Unknown);

	/*!
	 *  The constructor.
	 *
	 *      @param [in] vlnv   The VLNV for the document.
	 */
	Document(VLNV const& vlnv, Revision revision = Revision::Unknown);

    //! Copy constructor.
	Document(const Document &other);

    //! Assignment operator
	Document &operator=(const Document &other);

    //! The destructor.
	virtual ~Document() = default;

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
    
    /*! Get the display name of the element described in the document.
     *
     *      @return The display name of the document.
     */
    QString getDisplayName() const;

    /*! Set the display name of the document.
     *
     *      @param [in] displayName The display name to set.
    */
    void setDisplayName(const QString& displayName);

    /*! Get the short description of the element described in the document.
     *
     *      @return The short description of the document.
     */
    QString getShortDescription() const;

    /*! Set the short description of the document.
     *
     *      @param [in] shortDescription The short description to set.
    */
    void setShortDescription(const QString& shortDescription);

	/*! Get the description of the document.
	 *
	 *      @return QString containing the description of the document.
	 */
	QString getDescription() const;

	/*! Set the description of this document.
	 *
	 *      @param [in] description The description to set.
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
        
    /*!
     *  Adds a new XML namespace for the document, if it does not already exist within the document.
     *
     *      @param [in] nameSpace  The name of the namespace.
     *      @param [in] uri        The uri of the namespace.
     */
    void addXmlNameSpace(QString const& nameSpace, QString const& uri);
    
    /*!
     *  Gets the XML name spaces for the document.
     */
    QVector<QPair<QString, QString> > getXmlNameSpaces() const;

    /*!
     *  Set the XML schema location based on std revision.
     *
     *      @param [in] revision    The document std revision.
     */
    void setSchemaLocation(Document::Revision revision);

    /*!
     *  Get the XML schema location.
     *  
     *      @return The schema location as a string.
     */
    QString getSchemaLocation() const;

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
    
    /*!
     *  Get the document license.
     *
     *      @return The license of the document, if any.
     */
    QString getLicense() const;

    /*!
     *  Set a new license for the document.
     *
     *      @param [in] license     The new license.
     */
    void setLicense(QString const& license);

    /*!
     *  Get the document tags.
     *
     *      @return The document tags.
     */
    QVector<TagData> getTags() const;

    /*!
     *  Set new tags for the document.
     *
     *      @param [in] newTags     The new tags.
     */
    void setTags(QVector<TagData> newTags) const;

    /*!
     *  Get the IP-XACT document standard revision.
     *
     *      @return The document standard revision.
     */
    Document::Revision getRevision() const;

private:

    /*
     *  Add the default namespaces to the list of namespaces.
     */
    void addDefaultNameSpaces(Revision revision);
    
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

    /*!
     *  Get the group container for document tags.
     *
     *      @return The group container for document tags.
     */
    QSharedPointer<Kactus2Group> getTagGroup() const;

    /*!
     *  Get a tag container using a name.
     *
     *      @param [in] name        The name of the selected tag.
     *      @param [in] tagGroup    Tag group containing the document tags.
     *
     *      @return The selected tag container.
     */
    QSharedPointer<Kactus2Group> getTagByName(QString const& name, QSharedPointer<Kactus2Group> tagGroup) const;

    /*!
     *  Remove old non-existing tags.
     *
     *      @param [in] tagContainer    Extension container for tags.
     *      @param [in] newTags         The new tags.
     */
    void removeNonExistingTags(QSharedPointer<Kactus2Group> tagContainer, QVector<TagData> const& newTags) const;

    /*!
     *  Get the name of the selected tag.
     *
     *      @param [in] tagExtension    The selected tag.
     *
     *      @return Name of the selected tag.
     */
    QString getTagName(QSharedPointer<VendorExtension> tagExtension) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    struct DocumentNameGroup
    {
        // The unique identifier for the document.
	    VLNV vlnv;

        //! The document display name.
        QString displayName;

        //! The short description for the document.
        QString shortDescription;

        //! The description for the document.
        QString description;
    };

    //! The document name group
    DocumentNameGroup documentNameGroup_;

    //! The selected IP-XACT standard revision
	Revision revision_;	

	//! Contains the comment lines from the beginning of the document.
	QStringList topComments_;

    //! Contains the xml processing instructions as target-data pairs.
    QVector<QPair<QString, QString> > xmlProcessingInstructions_;

    //! The map of xml namespaces with namespace as key and uri as value.
    QVector<QPair<QString, QString> > xmlNameSpaces_;
    
    //! The map of xml attributes with attribute as key and uri as value.
    QString xmlSchemaLocation_;

    //! A list of parameters for the document.
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters_ =
        QSharedPointer<QList<QSharedPointer<Parameter> > >(new QList<QSharedPointer<Parameter> >());

    //! A list of assertions for the document.
    QSharedPointer<QList<QSharedPointer<Assertion> > > assertions_ =
        QSharedPointer<QList<QSharedPointer<Assertion> > >(new QList<QSharedPointer<Assertion> >());

};

#endif /* DOCUMENT_H */
