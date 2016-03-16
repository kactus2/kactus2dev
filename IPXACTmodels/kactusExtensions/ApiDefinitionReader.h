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

#include <QObject>
#include <QDomNode>
#include <QSharedPointer>

#include <IPXACTmodels/common/DocumentReader.h>
#include <IPXACTmodels/ipxactmodels_global.h>

class ApiDefinition;

//-----------------------------------------------------------------------------
//! XML reader class for IP-XACT Parameter element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ApiDefinitionReader : public DocumentReader
{
    Q_OBJECT
public:

	//! The constructor.
	ApiDefinitionReader(QObject* parent = 0);

	//! The destructor.
	~ApiDefinitionReader();

    /*!
     *  Creates a Api definition from XML description.
     *
     *      @param [in] document   The XML document to create the Api definition from.
     *
     *      @return The created Api definition.
     */
    QSharedPointer<ApiDefinition> createApiDefinitionFrom(QDomNode const& document) const;
 
private:

    /*!
     *  Parses all found data types from the given XML node.
     *
     *      @param [in] node The source XML node.
     */
    void parseDataTypes(QDomNode& node, QSharedPointer<ApiDefinition> apiDefinition) const;

    /*!
     *  Parses all found API functions from the given XML node.
     *
     *      @param [in] node The source XML node.
     */
    void parseFunctions(QDomNode& node, QSharedPointer<ApiDefinition> apiDefinition) const;

	//! Disable copying.
	ApiDefinitionReader(ApiDefinitionReader const& rhs);
	ApiDefinitionReader& operator=(ApiDefinitionReader const& rhs);
};

#endif // ApiDEFINITIONREADER_H