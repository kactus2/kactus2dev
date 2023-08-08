//-----------------------------------------------------------------------------
// File: ModeWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 07.08.2023
//
// Description:
// Writer class for IP-XACT mode element.
//-----------------------------------------------------------------------------

#include "ModeWriter.h"
#include "Mode.h"
#include "RemapPort.h"

#include <IPXACTmodels/common/NameGroupWriter.h>


//-----------------------------------------------------------------------------
// Function: ModeWriter::writeMode()
//-----------------------------------------------------------------------------
void ModeWriter::writeMode(QXmlStreamWriter& writer, QSharedPointer<Mode> mode)
{
    writer.writeStartElement(QStringLiteral("ipxact:mode"));

    NameGroupWriter::writeNameGroup(writer, mode, Document::Revision::Std22);

    writer.writeEndElement(); // ipxact:mode
}
