//-----------------------------------------------------------------------------
// File: NameGroupWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 05.08.2015
//
// Description:
// Writer class for namegroup element.
//-----------------------------------------------------------------------------

#include "NameGroupWriter.h"

//-----------------------------------------------------------------------------
// Function: NameGroupWriter::NameGroupWriter()
//-----------------------------------------------------------------------------
NameGroupWriter::NameGroupWriter(QObject* parent /* = 0 */):
QObject(parent)
{

}

//-----------------------------------------------------------------------------
// Function: NameGroupWriter::~NameGroupWriter()
//-----------------------------------------------------------------------------
NameGroupWriter::~NameGroupWriter()
{

}

//-----------------------------------------------------------------------------
// Function: NameGroupWriter::writeNameGroup()
//-----------------------------------------------------------------------------
void NameGroupWriter::writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<NameGroup> nameGroup) const
{
    writer.writeTextElement("ipxact:name", nameGroup->name());

    if (!nameGroup->displayName().isEmpty())
    {
        writer.writeTextElement("ipxact:displayName", nameGroup->displayName());
    }

    if (!nameGroup->description().isEmpty())
    {
        writer.writeTextElement("ipxact:description", nameGroup->description());
    }
}