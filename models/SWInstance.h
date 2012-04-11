//-----------------------------------------------------------------------------
// File: SWInstance.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 5.4.2012
//
// Description:
// SW instance class for extending IP-XACT designs.
//-----------------------------------------------------------------------------

#ifndef SWINSTANCE_H
#define SWINSTANCE_H

#include <LibraryManager/vlnv.h>

#include <QString>
#include <QPointF>
#include <QList>
#include <QMap>
#include <QSharedPointer>
#include <QDomNode>
#include <QXmlStreamWriter>

class ComInterface;

//-----------------------------------------------------------------------------
//! SW instance class for extending IP-XACT designs.
//-----------------------------------------------------------------------------
class SWInstance
{
public:
    /*!
     *  Default constructor.
     */
    SWInstance();

    /*!
     *  Copy constructor.
     */
    SWInstance(SWInstance const& rhs);

    /*!
     *  Constructor which reads the SW instance from an XML node.
     *
     *      @param [in] node The source XML node.
     */
    SWInstance(QDomNode& node);

    /*!
     *  Destructor.
     */
    ~SWInstance();

    /*!
     *  Writes the contents to an XML stream.
     *
     *      @param [in] writer The XML stream writer.
     */
    void write(QXmlStreamWriter& writer) const;

    /*!
     *  Sets the name of the SW instance.
     *
     *      @param [in] name The name to set.
     */
    void setInstanceName(QString const& name);

    /*!
     *  Sets the display name of the SW instance.
     *
     *      @param [in] displayName The display name to set.
     */
    void setDisplayName(QString const& displayName);

    /*!
     *  Sets the description of the SW instance.
     *
     *      @param [in] description The description to set.
     */
    void setDescription(QString const& description);

    /*!
     *  Sets the component reference.
     *
     *      @param [in] vlnv The referenced component VLNV.
     */
    void setComponentRef(VLNV const& vlnv);

    /*!
     *  Sets the file set reference.
     *
     *      @param [in] fileSetName The name of the referenced file set in the top-level component.
     */
    void setFileSetRef(QString const& fileSetName);

    /*!
     *  Sets the HW mapping information for the SW instance.
     *
     *      @param [in] hwRef The name of the HW component instance to which the SW instance is mapped.
     */
    void setMapping(QString const& hwRef);

    /*!
     *  Sets the global position of the SW instance.
     *
     *      @param [in] pos The global position in the design, in pixels.
     */
    void setPosition(QPointF const& pos);

    /*!
     *  Returns the name of the SW instance.
     */
    QString const& getInstanceName() const;
    
    /*!
     *  Returns the display name of the SW instance.
     */
    QString const& getDisplayName() const;

    /*!
     *  Returns the description of the SW instance.
     */
    QString const& getDescription() const;

    /*!
     *  Returns the component reference.
     */
    VLNV const& getComponentRef() const;

    /*!
     *  Returns the file set reference (i.e. the name of the referenced file set).
     */
    QString const& getFileSetRef() const;

    /*!
     *  Returns the name of the referenced HW component instance onto which the SW instance is mapped.
     */
    QString const& getMapping() const;

    /*!
     *  Returns the global position of the SW instance in the design.
     */
    QPointF const& getPosition() const;

    /*!
     *  Assignment operator.
     */
    SWInstance& operator=(SWInstance const& rhs);

private:
    /*!
     *  Parses communication interfaces from the given XML node.
     *
     *      @param [in] node The source XML node.
     */
    void parseComInterfaces(QDomNode& node);

    /*!
     *  Parses property values from the given XML node.
     *
     *      @param [in] node The source XML node.
     */
    void parsePropertyValues(QDomNode& node);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The instance name.
    QString instanceName_;

    //! The display name.
    QString displayName_;

    //! The description of the SW instance.
    QString desc_;

    //! The VLNV reference to an SW component.
    VLNV componentRef_;

    //! The file set reference.
    QString fileSetRef_;
    
    //! HW reference for mapping.
    QString hwRef_;

    //! The global position of the SW instance in the design.
    QPointF pos_;

    //! The "ad-hoc" communication interfaces.
    QList< QSharedPointer<ComInterface> > comInterfaces_;

    //! The set property values.
    QMap<QString, QString> propertyValues_;
};

//-----------------------------------------------------------------------------

#endif // SWINSTANCE_H
