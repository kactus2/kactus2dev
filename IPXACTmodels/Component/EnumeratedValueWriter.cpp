//-----------------------------------------------------------------------------
// File: EnumeratedValueWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.09.2015
//
// Description:
// Writer class for IP-XACT enumerated value element.
//-----------------------------------------------------------------------------

#include "EnumeratedValueWriter.h"
#include "EnumeratedValue.h"

#include <IPXACTmodels/common/NameGroupWriter.h>

//-----------------------------------------------------------------------------
// Function: EnumeratedValueWriter::EnumeratedValueWriter()
//-----------------------------------------------------------------------------
EnumeratedValueWriter::EnumeratedValueWriter(QObject* parent /* = 0 */) :
CommonItemsWriter(parent)
{

}

//-----------------------------------------------------------------------------
// Function: EnumeratedValueWriter::~EnumeratedValueWriter()
//-----------------------------------------------------------------------------
EnumeratedValueWriter::~EnumeratedValueWriter()
{

}

//-----------------------------------------------------------------------------
// Function: EnumeratedValueWriter::writeEnumeratedValue()
//-----------------------------------------------------------------------------
void EnumeratedValueWriter::writeEnumeratedValue(QXmlStreamWriter& writer,
    QSharedPointer<EnumeratedValue> enumeratedValue) const
{
    writer.writeStartElement("ipxact:enumeratedValue");

    writeUsage(writer, enumeratedValue);

    writeNameGroup(writer, enumeratedValue);

    writeValue(writer, enumeratedValue);

    writeVendorExtensions(writer, enumeratedValue);

    writer.writeEndElement(); // ipxact:enumeratedValue
}

//-----------------------------------------------------------------------------
// Function: EnumeratedValueWriter::writeUsage()
//-----------------------------------------------------------------------------
void EnumeratedValueWriter::writeUsage(QXmlStreamWriter& writer, QSharedPointer<EnumeratedValue> enumeratedValue)
    const
{
    EnumeratedValue::EnumeratedUsage usage = enumeratedValue->getUsage();
    if (usage != EnumeratedValue::UNKNOWNUSAGE)
    {
        writer.writeAttribute("usage", EnumeratedValue::usage2Str(usage));
    }
}

//-----------------------------------------------------------------------------
// Function: EnumeratedValueWriter::writeNameGroup()
//-----------------------------------------------------------------------------
void EnumeratedValueWriter::writeNameGroup(QXmlStreamWriter& writer,
    QSharedPointer<EnumeratedValue> enumeratedValue) const
{
    NameGroupWriter namegroupWriter;
    namegroupWriter.writeNameGroup(writer, enumeratedValue);
}

//-----------------------------------------------------------------------------
// Function: EnumeratedValueWriter::writeValue()
//-----------------------------------------------------------------------------
void EnumeratedValueWriter::writeValue(QXmlStreamWriter& writer, QSharedPointer<EnumeratedValue> enumeratedValue)
    const
{
    writer.writeTextElement("ipxact:value", enumeratedValue->getValue());
}