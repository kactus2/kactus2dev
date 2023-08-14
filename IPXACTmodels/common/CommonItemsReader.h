//-----------------------------------------------------------------------------
// File: CommonItemsReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 07.09.2015
//
// Description:
// Reader class for common IP-XACT elements: VLNV, parameters, assertions, vendor extensions and presence.
//-----------------------------------------------------------------------------

#ifndef COMMONITEMSREADER_H
#define COMMONITEMSREADER_H

#include <IPXACTmodels/common/VLNV.h>

#include <IPXACTmodels/common/Assertion.h>
#include <IPXACTmodels/common/ConfigurableElementValue.h>
#include <IPXACTmodels/common/ConfigurableVLNVReference.h>
#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/PartSelect.h>
#include <IPXACTmodels/common/Qualifier.h>

#include <IPXACTmodels/Component/Choice.h>

#include <QPointF>
#include <QSharedPointer>
#include <QDomNode>

//-----------------------------------------------------------------------------
//! Reader class for common IP-XACT elements: VLNV, parameters, assertions, vendor extensions and presence.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT CommonItemsReader
{
public:

    /*!
     *  The constructor.
     */
    CommonItemsReader();

    /*!
     *  The destructor.
     */
    virtual ~CommonItemsReader() = default;

    // Disable copying.
    CommonItemsReader(CommonItemsReader const& rhs) = delete;
    CommonItemsReader& operator=(CommonItemsReader const& rhs) = delete;

    /*!
    *  Parse and create VLNV from XML file.
    *
    *      @param [in] vlnvNode    XML description of the VLNV.
    *      @param [in] type        The type of the VLNV.
    *
    *      @return The new VLNV.
    */
    static VLNV createVLNVFrom(QDomNode const& vlnvNode, VLNV::IPXactType type);

    /*!
    *  Parses the VLNV from XML to attributes of an element.
    *
    *      @param [in] vlnvNode    The XML description of the VLNV.
    *      @param [in] vlnvType    The type of the VLNV.
    */
    static VLNV parseVLNVAttributes(QDomNode const& vlnvNode, VLNV::IPXactType vlnvType);

    /*!
     *  Parses and creates parameters from XML.
     *
     *      @param [in] itemNode    XML description of item containing the parameters.
     *      @param [in] revision    The standard revision of the XML document being read.
     *      @return A list of new parameters.
     */
    static QSharedPointer<QList<QSharedPointer<Parameter> > > parseAndCreateParameters
        (QDomNode const& itemNode, Document::Revision revision = Document::Revision::Std14);

    /*!
     *  Parses the vendor extensions from XML to a document.
     *
     *      @param [in] itemNode    The XML description of the document.
     *      @param [in] element     The object in which the parsed vendor extensions are stored.
     */
    static void parseVendorExtensions(QDomNode const& itemNode, QSharedPointer<Extendable> element);

    /*!
     *  Parses a configurable VLNV.
     *
     *      @param [in] configurableVLNVNode    The XML description of the configurable VLNV.
     *      @param [in] type                    The type of the configurable VLNV.
     *
     *      @return A VLNV with configurable element values.
     */
    static QSharedPointer<ConfigurableVLNVReference> parseConfigurableVLNVReference(
        QDomNode const& configurableVLNVNode, VLNV::IPXactType type);

    /*!
     *  Parses a single configurable element value.
     *
     *      @param [in] configurableElementNode     The XML description of the configurable element value.
     *
     *      @return A configurable element value.
     */
    static QSharedPointer<ConfigurableElementValue> parseConfigurableElementValue(
        QDomNode const& configurableElementNode);

    /*!
     *  Parse attributes of an element.
     *
     *      @param [in] attributedNode The XML node containing the attributes.
     *
     *      @return The parsed attributes.
     */
    static QMap<QString, QString> parseAttributes(QDomNode const& attributedNode);

    /*!
     *  Parses the isPresent element.
     *
     *      @param [in] isPresentElement    The is present element.
     *
     *      @return A QString containing the isPresent element for the item.
     */
    static QString parseIsPresent(QDomElement const& isPresentElement);

    static QPointF parsePoint(QDomNode const& node);
    /*!
     *  Parses choices.
     *
     *      @param [in] itemNode        XML description of the IP-XACT element.
     *      @return A list of choices.
     */
    static QSharedPointer<QList<QSharedPointer<Choice> > > parseChoices(QDomNode const& itemNode);
    
    /*!
     *  Parses a qualifier.
     *
     *      @param [in] qualifierNode       XML description of the IP-XACT element.
     *      @param [in] qualifier           The qualifier to read to.
     *      @param [in] revision            The standard revision of the XML document being read.

     */
    static void parseQualifier(QDomNode const& qualifierNode, QSharedPointer<Qualifier> qualifier,
        Document::Revision revision);

    static QSharedPointer<PartSelect> parsePartSelect(QDomNode const& partSelectNode);
};

#endif // DOCUMENTREADER_H