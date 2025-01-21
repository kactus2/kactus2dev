//-----------------------------------------------------------------------------
// File: ApiDefinitionReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 28.01.2016
//
// Description:
// XML reader class for Kactus2 API definition.
//-----------------------------------------------------------------------------

#ifndef APIDEFINITIONREADER_H
#define APIDEFINITIONREADER_H

#include <IPXACTmodels/common/DocumentReader.h>
#include <IPXACTmodels/ipxactmodels_global.h>

#include <QDomNode>
#include <QSharedPointer>

class ApiDefinition;

//-----------------------------------------------------------------------------
//! XML reader class for IP-XACT Parameter element.
//-----------------------------------------------------------------------------
namespace ApiDefinitionReader
{
    /*!
     *  Creates a Api definition from XML description.
     *
     *    @param [in] document   The XML document to create the Api definition from.
     *
     *    @return The created Api definition.
     */
    IPXACTMODELS_EXPORT QSharedPointer<ApiDefinition> createApiDefinitionFrom(QDomNode const& document);


    namespace Details
    {

        /*!
         *  Parses all found data types from the given XML node.
         *
         *    @param [in] node The source XML node.
         */
        void parseDataTypes(QDomNode& node, QSharedPointer<ApiDefinition> apiDefinition);

        /*!
         *  Parses all found API functions from the given XML node.
         *
         *    @param [in] node The source XML node.
         */
        void parseFunctions(QDomNode& node, QSharedPointer<ApiDefinition> apiDefinition);
    }
};

#endif // ApiDEFINITIONREADER_H