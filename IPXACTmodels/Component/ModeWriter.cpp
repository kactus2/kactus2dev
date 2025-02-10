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

#include <IPXACTmodels/common/CommonItemsWriter.h>
#include <IPXACTmodels/common/NameGroupWriter.h>
#include <IPXACTmodels/Component/FieldReferenceWriter.h>

//-----------------------------------------------------------------------------
// Function: ModeWriter::writeMode()
//-----------------------------------------------------------------------------
void ModeWriter::writeMode(QXmlStreamWriter& writer, QSharedPointer<Mode> mode)
{
    writer.writeStartElement(QStringLiteral("ipxact:mode"));

    NameGroupWriter::writeNameGroup(writer, mode, Document::Revision::Std22);

    Details::writeCondition(writer, mode);

    Details::writePortSlices(writer, mode);

    Details::writeFieldSlices(writer, mode);

    writer.writeEndElement(); // ipxact:mode
}

//-----------------------------------------------------------------------------
// Function: ModeWriter::Details::writeCondition()
//-----------------------------------------------------------------------------
void ModeWriter::Details::writeCondition(QXmlStreamWriter& writer, QSharedPointer<Mode> mode)
{
    CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:condition"), mode->getCondition());
}

//-----------------------------------------------------------------------------
// Function: ModeWriter::Details::writePortSlices()
//-----------------------------------------------------------------------------
void ModeWriter::Details::writePortSlices(QXmlStreamWriter& writer, QSharedPointer<Mode> mode)
{
    for (auto slice : *mode->getPortSlices())
    {
        writer.writeStartElement(QStringLiteral("ipxact:portSlice"));

        NameGroupWriter::writeNameGroup(writer, slice, Document::Revision::Std22);

        writer.writeStartElement(QStringLiteral("ipxact:portRef"));
        writer.writeAttribute(QStringLiteral("portRef"), slice->getPortRef());
        writer.writeEndElement(); // ipxact:portRef

        CommonItemsWriter::writePartSelect(writer, slice->getPartSelect());

        writer.writeEndElement(); // ipxact:portSlice
    }
}

//-----------------------------------------------------------------------------
// Function: ModeWriter::Details::writeFieldSlices()
//-----------------------------------------------------------------------------
void ModeWriter::Details::writeFieldSlices(QXmlStreamWriter& writer, QSharedPointer<Mode> mode)
{
    for (auto slice : *mode->getFieldSlices())
    {
        writer.writeStartElement(QStringLiteral("ipxact:fieldSlice"));

        NameGroupWriter::writeNameGroup(writer, slice, Document::Revision::Std22);

        FieldReferenceWriter::writeFieldReference(writer, slice);

        // Write range of the field slice.
        if (slice->getLeft().isEmpty() == false || slice->getRight().isEmpty() == false)
        {
            writer.writeStartElement(QStringLiteral("ipxact:range"));
            writer.writeTextElement(QStringLiteral("ipxact:left"), slice->getLeft());
            writer.writeTextElement(QStringLiteral("ipxact:right"), slice->getRight());
            writer.writeEndElement(); //! ipxact:range
        }

        writer.writeEndElement(); // ipxact:fieldSlice
    }
}
