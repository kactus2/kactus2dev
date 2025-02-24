//-----------------------------------------------------------------------------
// File: ComDefinitionWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 11.12.2015
//
// Description:
// Writer class for IP-XACT ComDefinition element.
//-----------------------------------------------------------------------------

#ifndef ComDefinitionWriter_H
#define ComDefinitionWriter_H

#include "ComDefinition.h"

#include <IPXACTmodels/common/DocumentWriter.h>

#include <QObject>
#include <QSharedPointer>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT ComDefinition element.
//-----------------------------------------------------------------------------
namespace ComDefinitionWriter 
{

    /*!
     *  Write a view to an XML file.
     *
     *    @param [in] writer  The used xml writer.
     *    @param [in] ComDefinition		The ComDefinition to be written.
     */
    IPXACTMODELS_EXPORT void writeComDefinition(QXmlStreamWriter& writer,
        QSharedPointer<ComDefinition> ComDefinition);
};

#endif // ComDefinitionWriter_H
