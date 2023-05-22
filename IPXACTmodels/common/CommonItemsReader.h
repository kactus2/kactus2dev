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

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/Assertion.h>
#include <IPXACTmodels/common/ConfigurableVLNVReference.h>
#include <IPXACTmodels/common/ConfigurableElementValue.h>

#include <IPXACTmodels/Component/Choice.h>

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
     *
     *      @return A list of new parameters.
     */
    static QSharedPointer<QList<QSharedPointer<Parameter> > > parseAndCreateParameters
        (QDomNode const& itemNode);

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
     *  Parses the isPresent element.
     *
     *      @param [in] isPresentElement    The is present element.
     *
     *      @return A QString containing the isPresent element for the item.
     */
    QString parseIsPresent(QDomElement const& isPresentElement) const;

    /*!
     *  Parses choices.
     *
     *      @param [in] itemNode        XML description of the IP-XACT element.
     *      @return [out]               A list of choices.
     */
    static QSharedPointer<QList<QSharedPointer<Choice> > > parseChoices(QDomNode const& itemNode);

private:

	// Disable copying.
    CommonItemsReader(CommonItemsReader const& rhs);
    CommonItemsReader& operator=(CommonItemsReader const& rhs);
};

#endif // DOCUMENTREADER_H