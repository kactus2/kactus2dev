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

#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/VendorExtension.h>

#include <IPXACTmodels/kactusExtensions/ComInterface.h>

#include <IPXACTmodels/Design/ComponentInstance.h>
#include <IPXACTmodels/common/ConfigurableVLNVReference.h>
#include <IPXACTmodels/common/ConfigurableElementValue.h>

#include <QString>
#include <QPointF>
#include <QList>
#include <QMap>
#include <QSharedPointer>
#include <QDomNode>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
//! SW instance class for extending IP-XACT designs.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT SWInstance : public VendorExtension, public ComponentInstance
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
     *  Clone constructor.
     */
    virtual SWInstance* clone() const;

    /*!
     *  Get the type of the extension.
     *
     *      @return The type of the extension.
     */
    virtual QString type() const;

    /*!
     *  Writes the contents to an XML stream.
     *
     *      @param [in] writer The XML stream writer.
     */
    virtual void write(QXmlStreamWriter& writer) const;

    /*!
     *  Returns true if the SW instance is in a valid state.
     *
     *      @param [out] errorList      Error list which is appended with errors if found while validating.
     *      @param [in]  instanceNames  The list of HW UUIDs in the containing design.
     *      @param [in]  parentId       The identifier of the containing design.
     */
    //bool isValid(QStringList& errorList, QStringList const& hwUUIDs,
    //             QString const& parentId) const;

	/*! \brief Check if the software instance is in valid state.
	 *
	 * \param instanceNames The list of HW UUIDs in the containing design.
	 *
	 * \return bool True if the SW instance is in valid state.
	*/
	//bool isValid(const QStringList& hwUUIDs) const;

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
     *  Returns the file set reference (i.e. the name of the referenced file set).
     */
    QString const& getFileSetRef() const;

    /*!
     *  Returns the name of the referenced HW component instance onto which the SW instance is mapped.
     */
    QString const& getMapping() const;

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
    
    /*!
     *  Writes the given vlnv as attributes.
     *
     *      @param [in] writer      The used XML writer.
     *      @param [in] targetVLNV  The target VLNV.
     */
    void writeVLNVasAttributes(QXmlStreamWriter& writer, VLNV const& targetVLNV) const;

    /*!
     *  Write the mapped position.
     *
     *      @param [in] writer          The used writer.
     *      @param [in] positions       The positions.
     *      @param [in] identifier      The identifier of the positions.
     *      @param [in] refIdentifier   The reference identifier.
     */
    void writeMappedPosition(QXmlStreamWriter& writer, QMap<QString, QPointF> const& positions,
        QString const& identifier, QString const& refIdentifier) const;

    QMap<QString, QPointF> createMappedPositions(QDomNode& mapNode, QString const& referenceIdentifier) const;

    /*!
     *  Reads a single point.
     *
     *      @param [in] node    The node containing the point.
     */
    QPointF parseSinglePoint(QDomNode const& node);

    /*!
     *  Create a VLNV from a node referencing a component.
     *
     *      @param [in] node    The node containing the component reference.
     */
    QSharedPointer<ConfigurableVLNVReference> createComponentReference(const QDomNode& node) const;

    /*!
     *  Create a configurable element value from XML.
     *
     *      @param [in] configurableElementNode     XML description of the configurable element.
     */
    QSharedPointer<ConfigurableElementValue> parseConfigurableElement(QDomNode const& configurableElementNode) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The file set reference.
    QString fileSetRef_;
    
    //! HW reference for mapping.
    QString hwRef_;

    //! The "ad-hoc" communication interfaces.
    QList< QSharedPointer<ComInterface> > comInterfaces_;
};

//-----------------------------------------------------------------------------

#endif // SWINSTANCE_H
