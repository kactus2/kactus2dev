//-----------------------------------------------------------------------------
// File: ApiDefinitionWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 11.12.2015
//
// Description:
// Writer class for IP-XACT ApiDefinition element.
//-----------------------------------------------------------------------------

#ifndef ApiDefinitionWriter_H
#define ApiDefinitionWriter_H

#include "ApiDefinition.h"

#include <IPXACTmodels/common/DocumentWriter.h>

#include <QXmlStreamWriter>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT ApiDefinition element.
//-----------------------------------------------------------------------------
namespace ApiDefinitionWriter
{

    /*!
     *  Write a view to an XML file.
     *
     *    @param [in] writer  The used xml writer.
     *    @param [in] ApiDefinition		The ApiDefinition to be written.
     */
    IPXACTMODELS_EXPORT void writeApiDefinition(QXmlStreamWriter& writer, 
        QSharedPointer<ApiDefinition> apiDefinition);

};

#endif // ApiDefinitionWriter_H
