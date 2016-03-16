//-----------------------------------------------------------------------------
// File: RemapStateWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.09.2015
//
// Description:
// Writer class for IP-XACT remapState element.
//-----------------------------------------------------------------------------

#include "RemapStateWriter.h"
#include "RemapState.h"
#include "RemapPort.h"

#include <IPXACTmodels/common/NameGroupWriter.h>

//-----------------------------------------------------------------------------
// Function: RemapStateWriter::RemapStateWriter()
//-----------------------------------------------------------------------------
RemapStateWriter::RemapStateWriter(QObject* parent /* = 0 */):
QObject(parent)
{

}

//-----------------------------------------------------------------------------
// Function: RemapStateWriter::~RemapStateWriter()
//-----------------------------------------------------------------------------
RemapStateWriter::~RemapStateWriter()
{

}

//-----------------------------------------------------------------------------
// Function: RemapStateWriter::writeRemapState()
//-----------------------------------------------------------------------------
void RemapStateWriter::writeRemapState(QXmlStreamWriter& writer, QSharedPointer<RemapState> remapState) const
{
    writer.writeStartElement("ipxact:remapState");

    writeNameGroup(writer, remapState);

    writeRemapPorts(writer, remapState);

    writer.writeEndElement(); // ipxact:remapState
}

//-----------------------------------------------------------------------------
// Function: RemapStateWriter::writeNameGroup()
//-----------------------------------------------------------------------------
void RemapStateWriter::writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<RemapState> remapState) const
{
    NameGroupWriter nameGroupWriter;
    nameGroupWriter.writeNameGroup(writer, remapState);
}

//-----------------------------------------------------------------------------
// Function: RemapStateWriter::writeRemapPorts()
//-----------------------------------------------------------------------------
void RemapStateWriter::writeRemapPorts(QXmlStreamWriter& writer, QSharedPointer<RemapState> remapState) const
{
    if (!remapState->getRemapPorts()->isEmpty())
    {
        writer.writeStartElement("ipxact:remapPorts");

        foreach (QSharedPointer<RemapPort> remapPort, *remapState->getRemapPorts())
        {
            writeSingleRemapPort(writer, remapPort);
        }

        writer.writeEndElement(); // ipxact:remapPorts
    }
}

//-----------------------------------------------------------------------------
// Function: RemapStateWriter::writeSingleRemapPort()
//-----------------------------------------------------------------------------
void RemapStateWriter::writeSingleRemapPort(QXmlStreamWriter& writer, QSharedPointer<RemapPort> remapPort) const
{
    writer.writeStartElement("ipxact:remapPort");

    writer.writeAttribute("portRef", remapPort->getPortNameRef());

    if (!remapPort->getPortIndex().isEmpty())
    {
        writer.writeTextElement("ipxact:portIndex", remapPort->getPortIndex());
    }

    writer.writeTextElement("ipxact:value", remapPort->getValue());

    writer.writeEndElement(); // ipxact:remapPort
}