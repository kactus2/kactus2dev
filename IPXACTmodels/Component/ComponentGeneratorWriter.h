//-----------------------------------------------------------------------------
// File: ComponentGeneratorWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 05.10.2015
//
// Description:
// Writer for IP-XACT ComponentGenerator element.
//-----------------------------------------------------------------------------

#ifndef ComponentGeneratorWriter_H
#define ComponentGeneratorWriter_H

#include "ComponentGenerator.h"

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/Document.h>

#include <QXmlStreamWriter>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Writer for IP-XACT ComponentGenerator element.
//-----------------------------------------------------------------------------
namespace ComponentGeneratorWriter
{

    /*!
     *  Write a ComponentGenerator to an XML file.
     *
     *      @param [in] writer                  The used xml writer.
     *      @param [in] ComponentGenerator		The ComponentGenerator to be written.
     *      @param [in] docRevision				The IP-XACT standard revision in use.
     */
	IPXACTMODELS_EXPORT void writeComponentGenerator(QXmlStreamWriter& writer, 
		QSharedPointer<ComponentGenerator> componentGenerator, Document::Revision docRevision);

	namespace Details
	{

		/*!
		 *  Writes the component generator attributes to XML.
		 *
		 *      @param [in] writer					The used xml writer.
		 *      @param [in] componentGenerator		The component generator whose attributes to write.
		 */
		void writeAttributes(QXmlStreamWriter& writer, QSharedPointer<ComponentGenerator> componentGenerator);

		/*!
		 *  Writes the component generator API type to XML.
		 *
		 *      @param [in] writer					The used xml writer.
		 *      @param [in] componentGenerator		The component generator whose API type to write.
		 */
		void writeApiType(QXmlStreamWriter& writer, QSharedPointer<ComponentGenerator> componentGenerator, Document::Revision docRevision);
		
		/*!
		 *	Writes the component generator API service to XML.
		 *  
         *      @param [in] writer					The used xml writer.
         *      @param [in] componentGenerator		The component generator whose attributes to write.
		 *      @param [in] docRevision				The IP-XACT standard revision in use.
		 */
		void writeApiService(QXmlStreamWriter& writer, QSharedPointer<ComponentGenerator> componentGenerator, Document::Revision docRevision);

		/*!
		 *  Writes the component generator transport methods to XML.
		 *
		 *      @param [in] writer					The used xml writer.
		 *      @param [in] componentGenerator		The component generator whose transport mehtods to write.
		 */
		void writeTransportMethods(QXmlStreamWriter& writer,
			QSharedPointer<ComponentGenerator> componentGenerator);
	}
};

#endif // ComponentGeneratorWriter_H