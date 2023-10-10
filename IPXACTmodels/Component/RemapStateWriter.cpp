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
// Function: RemapStateWriter::writeRemapState()
//-----------------------------------------------------------------------------
void RemapStateWriter::writeRemapState(QXmlStreamWriter& writer, QSharedPointer<RemapState> remapState)
{
    writer.writeStartElement(QStringLiteral("ipxact:remapState"));

    Details::writeNameGroup(writer, remapState);

    Details::writeRemapPorts(writer, remapState);

    writer.writeEndElement(); // ipxact:remapState
}

//-----------------------------------------------------------------------------
// Function: RemapStateWriter::Details::writeNameGroup()
//-----------------------------------------------------------------------------
void RemapStateWriter::Details::writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<RemapState> remapState)
{
    NameGroupWriter::writeNameGroup(writer, remapState);
}

//-----------------------------------------------------------------------------
// Function: RemapStateWriter::writeRemapPorts()
//-----------------------------------------------------------------------------
void RemapStateWriter::Details::writeRemapPorts(QXmlStreamWriter& writer, QSharedPointer<RemapState> remapState)
{
    if (!remapState->getRemapPorts()->isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:remapPorts"));

        for (auto const& remapPort : *remapState->getRemapPorts())
        {
            writeSingleRemapPort(writer, remapPort);
        }

        writer.writeEndElement(); // ipxact:remapPorts
    }
}

//-----------------------------------------------------------------------------
// Function: RemapStateWriter::Details::writeSingleRemapPort()
//-----------------------------------------------------------------------------
void RemapStateWriter::Details::writeSingleRemapPort(QXmlStreamWriter& writer, QSharedPointer<RemapPort> remapPort)
{
    writer.writeStartElement(QStringLiteral("ipxact:remapPort"));

    writer.writeAttribute(QStringLiteral("portRef"), remapPort->getPortNameRef());

    if (!remapPort->getPortIndex().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:portIndex"), remapPort->getPortIndex());
    }

    writer.writeTextElement(QStringLiteral("ipxact:value"), remapPort->getValue());

    writer.writeEndElement(); // ipxact:remapPort
}