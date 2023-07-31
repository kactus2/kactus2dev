//-----------------------------------------------------------------------------
// File: ViewWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.09.2015
//
// Description:
// Writer for IP-XACT view element.
//-----------------------------------------------------------------------------

#include "ViewWriter.h"

#include <IPXACTmodels/common/NameGroupWriter.h>
#include <IPXACTmodels/common/CommonItemsWriter.h>

//-----------------------------------------------------------------------------
// Function: ViewWriter::writeView()
//-----------------------------------------------------------------------------
void ViewWriter::writeView(QXmlStreamWriter& writer, QSharedPointer<View> view, Document::Revision docRevision)
{
    writer.writeStartElement(QStringLiteral("ipxact:view"));

    NameGroupWriter::writeNameGroup(writer, view, docRevision);

    if (docRevision == Document::Revision::Std14)
    {
        Details::writeIsPresent(writer, view);
    }

    Details::writeEnvIdentifiers(writer, view);

    Details::writeInstantiationReferences(writer, view);

    if (docRevision == Document::Revision::Std22)
    {
        CommonItemsWriter::writeVendorExtensions(writer, view);
    }

    writer.writeEndElement(); // ipxact:view
}

//-----------------------------------------------------------------------------
// Function: ViewWriter::Details::writeIsPresent()
//-----------------------------------------------------------------------------
void ViewWriter::Details::writeIsPresent(QXmlStreamWriter& writer, QSharedPointer<View> view)
{
    if (!view->getIsPresent().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:isPresent"), view->getIsPresent());
    }
}

//-----------------------------------------------------------------------------
// Function: ViewWriter::Details::writeEnvIdentifiers()
//-----------------------------------------------------------------------------
void ViewWriter::Details::writeEnvIdentifiers(QXmlStreamWriter& writer, QSharedPointer<View> view)
{
    for (auto const& identifier : *view->getEnvIdentifiers())
    {
        writer.writeTextElement(QStringLiteral("ipxact:envIdentifier"), identifier->toString());
    }
}

//-----------------------------------------------------------------------------
// Function: ViewWriter::Details::writeInstantiationReferences()
//-----------------------------------------------------------------------------
void ViewWriter::Details::writeInstantiationReferences(QXmlStreamWriter& writer, QSharedPointer<View> view)
{
    if (!view->getComponentInstantiationRef().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:componentInstantiationRef"), view->getComponentInstantiationRef());
    }

    if (!view->getDesignInstantiationRef().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:designInstantiationRef"), view->getDesignInstantiationRef());
    }

    if (!view->getDesignConfigurationInstantiationRef().isEmpty())
    {
        writer.writeTextElement(
            QStringLiteral("ipxact:designConfigurationInstantiationRef"), view->getDesignConfigurationInstantiationRef());
    }
}