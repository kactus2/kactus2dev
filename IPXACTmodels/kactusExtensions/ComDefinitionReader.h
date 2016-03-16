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

#ifndef ComDEFINITIONREADER_H
#define ComDEFINITIONREADER_H

#include <QObject>
#include <QDomNode>
#include <QSharedPointer>

#include <IPXACTmodels/common/DocumentReader.h>
#include <IPXACTmodels/ipxactmodels_global.h>

class ComDefinition;

//-----------------------------------------------------------------------------
//! XML reader class for IP-XACT Parameter element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ComDefinitionReader : public DocumentReader
{
    Q_OBJECT
public:

	//! The constructor.
	ComDefinitionReader(QObject* parent = 0);

	//! The destructor.
	~ComDefinitionReader();

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

	//! Disable copying.
	ComDefinitionReader(ComDefinitionReader const& rhs);
	ComDefinitionReader& operator=(ComDefinitionReader const& rhs);
};

#endif // ComDEFINITIONREADER_H