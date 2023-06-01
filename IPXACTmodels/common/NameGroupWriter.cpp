//-----------------------------------------------------------------------------
// File: NameGroupWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 05.08.2015
//
// Description:
// Writer for namegroup element.
//-----------------------------------------------------------------------------

#include "NameGroupWriter.h"

//-----------------------------------------------------------------------------
// Function: NameGroupWriter::writeNameGroup()
//-----------------------------------------------------------------------------
void NameGroupWriter::writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<NameGroup> nameGroup)
{
    writer.writeTextElement(QStringLiteral("ipxact:name"), nameGroup->name());

    if (!nameGroup->displayName().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:displayName"), nameGroup->displayName());
    }
    if (!nameGroup->shortDescription().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:shortDescription"), nameGroup->shortDescription());
    }
    if (!nameGroup->description().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:description"), nameGroup->description());
    }
}