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
namespace  ComDefinitionReader
{

    /*!
     *  Creates a Com definition from XML description.
     *
     *      @param [in] document   The XML document to create the Com definition from.
     *
     *      @return The created Com definition.
     */
    IPXACTMODELS_EXPORT QSharedPointer<ComDefinition> createComDefinitionFrom(QDomNode const& document);
 
    namespace Details
    {
        /*!
         *  Parses all found transfer types from the given XML node.
         *
         *      @param [in] node The source XML node.
         */
        void parseTransferTypes(QDomNode& node, QSharedPointer<ComDefinition> comDefinition);

        /*!
         *  Parses all found properties from the given XML node.
         *
         *      @param [in] node The source XML node.
         */
        void parseProperties(QDomNode& node, QSharedPointer<ComDefinition> comDefinition);
    }
};

#endif // COMDEFINITIONREADER_H