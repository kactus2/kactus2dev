//-----------------------------------------------------------------------------
// File: ViewWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.09.2015
//
// Description:
// Writer class for IP-XACT view element.
//-----------------------------------------------------------------------------

#include "ViewWriter.h"

#include <IPXACTmodels/common/NameGroupWriter.h>

//-----------------------------------------------------------------------------
// Function: ViewWriter::ViewWriter()
//-----------------------------------------------------------------------------
ViewWriter::ViewWriter(QObject* parent /* = 0 */):
QObject(parent)
{

}

//-----------------------------------------------------------------------------
// Function: ViewWriter::~ViewWriter()
//-----------------------------------------------------------------------------
ViewWriter::~ViewWriter()
{

}

//-----------------------------------------------------------------------------
// Function: ViewWriter::writeView()
//-----------------------------------------------------------------------------
void ViewWriter::writeView(QXmlStreamWriter& writer, QSharedPointer<View> view) const
{
    writer.writeStartElement("ipxact:view");

    writeNameGroup(writer, view);

    writeIsPresent(writer, view);

    writeEnvIdentifiers(writer, view);

    writeInstantiationReferences(writer, view);

    writer.writeEndElement(); // ipxact:view
}

//-----------------------------------------------------------------------------
// Function: ViewWriter::writeNameGroup()
//-----------------------------------------------------------------------------
void ViewWriter::writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<View> view) const
{
    NameGroupWriter nameGroupWriter;
    nameGroupWriter.writeNameGroup(writer, view);
}

//-----------------------------------------------------------------------------
// Function: ViewWriter::writeIsPresent()
//-----------------------------------------------------------------------------
void ViewWriter::writeIsPresent(QXmlStreamWriter& writer, QSharedPointer<View> view) const
{
    if (!view->getIsPresent().isEmpty())
    {
        writer.writeTextElement("ipxact:isPresent", view->getIsPresent());
    }
}

//-----------------------------------------------------------------------------
// Function: ViewWriter::writeEnvIdentifiers()
//-----------------------------------------------------------------------------
void ViewWriter::writeEnvIdentifiers(QXmlStreamWriter& writer, QSharedPointer<View> view) const
{
    if (!view->getEnvIdentifiers().isEmpty())
    {
        foreach (QString identifier, view->getEnvIdentifiers())
        {
            writer.writeTextElement("ipxact:envIdentifier", identifier);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ViewWriter::writeInstantiationReferences()
//-----------------------------------------------------------------------------
void ViewWriter::writeInstantiationReferences(QXmlStreamWriter& writer, QSharedPointer<View> view) const
{
    if (!view->getComponentInstantiationRef().isEmpty())
    {
        writer.writeTextElement("ipxact:componentInstantiationRef", view->getComponentInstantiationRef());
    }

    if (!view->getDesignInstantiationRef().isEmpty())
    {
        writer.writeTextElement("ipxact:designInstantiationRef", view->getDesignInstantiationRef());
    }

    if (!view->getDesignConfigurationInstantiationRef().isEmpty())
    {
        writer.writeTextElement(
            "ipxact:designConfigurationInstantiationRef", view->getDesignConfigurationInstantiationRef());
    }
}