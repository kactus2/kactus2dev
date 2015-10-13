//-----------------------------------------------------------------------------
// File: ComponentGeneratorReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 05.10.2015
//
// Description:
// Reader class for IP-XACT ComponentGenerator element.
//-----------------------------------------------------------------------------

#ifndef ComponentGeneratorReader_H
#define ComponentGeneratorReader_H

#include "ComponentGenerator.h"

#include <IPXACTmodels/common/CommonItemsReader.h>

#include <QObject>
#include <QSharedPointer>
#include <QDomNode>

//-----------------------------------------------------------------------------
//! Reader class for IP-XACT ComponentGenerator element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ComponentGeneratorReader : public CommonItemsReader
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this reader.
     */
    ComponentGeneratorReader(QObject* parent = 0);

    //! The destructor.
    ~ComponentGeneratorReader();

    /*!
     *  Creates a new ComponentGenerator from a given ComponentGenerator node.
     *
     *      @param [in] ComponentGeneratorNode    XML description of the ComponentGenerator.
     *
     *      @return The component generator described in the XML.
     */
    QSharedPointer<ComponentGenerator> createComponentGeneratorFrom(QDomNode const& componentGeneratorNode) const;

private:

    //! No copying allowed.
    ComponentGeneratorReader(ComponentGeneratorReader const& rhs);
	ComponentGeneratorReader& operator=(ComponentGeneratorReader const& rhs);

	/*!
	 *  Extracts attributes from componentGeneratorNode.
	 *
	 *      @param [in] componentGeneratorNode		    The XML description of the component generator.
	 *      @param [in/out] newComponentGenerator		The created component generator.
	 */
	void readAttributes(QDomNode const& componentGeneratorNode,
		QSharedPointer<ComponentGenerator> newComponentGenerator) const;

	/*!
	 *  Extracts parameters from parameters XML description.
	 *
	 *      @param [in] parametersNode				    The XML description of the parameters.
	 *      @param [in/out] componentGenerator  		The created component generator.
	 */
	void readParameters(QDomNode const& parametersNode, 
        QSharedPointer<ComponentGenerator> componentGenerator) const;

	/*!
	 *  Extracts API type from XML description.
	 *
	 *      @param [in] apiTypeNode					    The XML description of the API type.
	 *      @param [in/out] newComponentGenerator		The created component generator.
	 */
	void readApiType(QDomNode const& apiTypeNode, QSharedPointer<ComponentGenerator> newComponentGenerator) const;
    
	/*!
	 *  Extracts the transport type methods from XML description.
	 *
	 *      @param [in] tempNode					    The XML description of transport methods.
	 *      @param [in/out] newComponentGenerator		The created component generator.
	 */
    void readTransportMethods(QDomElement const& methodsNode,
        QSharedPointer<ComponentGenerator> newComponentGenerator) const;

    /*!
	 *  Extracts the generator group names from XML description.
	 *
	 *      @param [in]     componentGeneratorNode		The XML description of component generator.
	 *      @param [in/out] newComponentGenerator		The created component generator.
	 */
    void parseGroups(QDomNode const& componentGeneratorNode, 
        QSharedPointer<ComponentGenerator> newComponentGenerator) const;
};

#endif // ComponentGeneratorReader_H