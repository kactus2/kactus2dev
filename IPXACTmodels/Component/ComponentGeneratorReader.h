//-----------------------------------------------------------------------------
// File: ComponentGeneratorReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 05.10.2015
//
// Description:
// Reader for IP-XACT ComponentGenerator element.
//-----------------------------------------------------------------------------

#ifndef ComponentGeneratorReader_H
#define ComponentGeneratorReader_H

#include "ComponentGenerator.h"

#include <IPXACTmodels/common/CommonItemsReader.h>
#include <IPXACTmodels/common/Document.h>

#include <QSharedPointer>
#include <QDomNode>

//-----------------------------------------------------------------------------
//! Reader for IP-XACT ComponentGenerator element.
//-----------------------------------------------------------------------------
namespace ComponentGeneratorReader
{

    /*!
     *  Creates a new ComponentGenerator from a given ComponentGenerator node.
     *
     *      @param [in] ComponentGeneratorNode    XML description of the ComponentGenerator.
     *      @param [in] docRevision				  The IP-XACT standard revision in use.

     *      @return The component generator described in the XML.
     */
	IPXACTMODELS_EXPORT QSharedPointer<ComponentGenerator> createComponentGeneratorFrom(
		QDomNode const& componentGeneratorNode, Document::Revision docRevision);

	namespace Details
	{

		/*!
		 *  Extracts attributes from componentGeneratorNode.
		 *
		 *      @param [in] componentGeneratorNode		    The XML description of the component generator.
		 *      @param [in/out] newComponentGenerator		The created component generator.
		 */
		void readAttributes(QDomNode const& componentGeneratorNode,
			QSharedPointer<ComponentGenerator> newComponentGenerator);

		/*!
		 *  Extracts parameters from parameters XML description.
		 *
		 *      @param [in] parametersNode				    The XML description of the parameters.
         *      @param [in/out] componentGenerator  		The created component generator.
		 *      @param [in] docRevision						The IP-XACT standard revision in use.
		 */
		void readParameters(QDomNode const& parametersNode,
			QSharedPointer<ComponentGenerator> componentGenerator,
			Document::Revision docRevision);

		/*!
		 *  Extracts API type from XML description.
		 *
		 *      @param [in] apiTypeNode					    The XML description of the API type.
		 *      @param [in/out] newComponentGenerator		The created component generator.
		 */
		void readApiType(QDomNode const& apiTypeNode, QSharedPointer<ComponentGenerator> newComponentGenerator);

		/*!
		 *	Extracts API service from XML description.
		 *  
         *      @param [in] apiServiceNode					The XML description of the API service.
         *      @param [in/out] newComponentGenerator		The created component generator.
		 */
		void readApiService(QDomNode const& apiServiceNode, QSharedPointer<ComponentGenerator> newComponentGenerator);

		/*!
		 *  Extracts the transport type methods from XML description.
		 *
		 *      @param [in] tempNode					    The XML description of transport methods.
		 *      @param [in/out] newComponentGenerator		The created component generator.
		 */
		void readTransportMethods(QDomElement const& methodsNode,
			QSharedPointer<ComponentGenerator> newComponentGenerator);

		/*!
		 *  Extracts the generator group names from XML description.
		 *
		 *      @param [in]     componentGeneratorNode		The XML description of component generator.
		 *      @param [in/out] newComponentGenerator		The created component generator.
		 */
		void parseGroups(QDomNode const& componentGeneratorNode,
			QSharedPointer<ComponentGenerator> newComponentGenerator);
	}
};

#endif // ComponentGeneratorReader_H