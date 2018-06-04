//-----------------------------------------------------------------------------
// File: ComDefinitionReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 28.01.2016
//
// Description:
// XML reader class for Kactus2 Com definition.
//-----------------------------------------------------------------------------

#ifndef COMDEFINITIONREADER_H
#define COMDEFINITIONREADER_H

#include <IPXACTmodels/common/DocumentReader.h>
#include <IPXACTmodels/ipxactmodels_global.h>

#include <QDomNode>
#include <QSharedPointer>

class ComDefinition;

//-----------------------------------------------------------------------------
//! XML reader class for IP-XACT Parameter element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ComDefinitionReader : public DocumentReader
{
public:

	//! The constructor.
	ComDefinitionReader();

	//! The destructor.
	virtual ~ComDefinitionReader() = default;

    //! Disable copying.
    ComDefinitionReader(ComDefinitionReader const& rhs) = delete;
    ComDefinitionReader& operator=(ComDefinitionReader const& rhs) = delete;

    /*!
     *  Creates a Com definition from XML description.
     *
     *      @param [in] document   The XML document to create the Com definition from.
     *
     *      @return The created Com definition.
     */
    QSharedPointer<ComDefinition> createComDefinitionFrom(QDomNode const& document) const;
 
private:

    /*!
     *  Parses all found transfer types from the given XML node.
     *
     *      @param [in] node The source XML node.
     */
    void parseTransferTypes(QDomNode& node, QSharedPointer<ComDefinition> comDefinition) const;

    /*!
     *  Parses all found properties from the given XML node.
     *
     *      @param [in] node The source XML node.
     */
    void parseProperties(QDomNode& node, QSharedPointer<ComDefinition> comDefinition) const;

};

#endif // COMDEFINITIONREADER_H