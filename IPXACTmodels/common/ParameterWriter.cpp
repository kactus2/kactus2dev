//-----------------------------------------------------------------------------
// File: ParameterWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 31.07.2015
//
// Description:
// Writer class for IP-XACT Parameter element.
//-----------------------------------------------------------------------------

#include "ParameterWriter.h"

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/CommonItemsWriter.h>
#include <IPXACTmodels/common/NameGroupWriter.h>
#include <IPXACTmodels/common/VendorExtension.h>

#include <QSharedPointer>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
// Function: ParameterWriter::ParameterWriter()
//-----------------------------------------------------------------------------
ParameterWriter::ParameterWriter()
{

}

//-----------------------------------------------------------------------------
// Function: ParameterWriter::~ParameterWriter()
//-----------------------------------------------------------------------------
ParameterWriter::~ParameterWriter()
{

}

//-----------------------------------------------------------------------------
// Function: ParameterWriter::write()
//-----------------------------------------------------------------------------
void ParameterWriter::writeParameter(QXmlStreamWriter& writer, QSharedPointer<Parameter> parameter) const
{
    writer.writeStartElement(QStringLiteral("ipxact:parameter"));
    writeAttributes(writer, parameter);
    
    writeNameGroup(writer, parameter);

    writeVectors(writer, parameter);

    writeArrays(writer, parameter);

    writeValue(writer, parameter);

    CommonItemsWriter::writeVendorExtensions(writer, parameter);

    writer.writeEndElement(); // ipxact:parameter
}

//-----------------------------------------------------------------------------
// Function: ParameterWriter::writeAttributes()
//-----------------------------------------------------------------------------
void ParameterWriter::writeAttributes(QXmlStreamWriter& writer, QSharedPointer<Parameter> parameter) const
{
    for (auto const& attribute : parameter->getAttributeNames())
    {
        writer.writeAttribute(attribute, parameter->getAttribute(attribute));
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterWriter::writeNameGroup()
//-----------------------------------------------------------------------------
void ParameterWriter::writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<Parameter> parameter) const
{
    NameGroupWriter::writeNameGroup(writer, parameter);
}

//-----------------------------------------------------------------------------
// Function: ParameterWriter::writeArrays()
//-----------------------------------------------------------------------------
void ParameterWriter::writeArrays(QXmlStreamWriter& writer, QSharedPointer<Parameter> parameter) const
{
    QSharedPointer<QList<QSharedPointer<Array> > > arrays = parameter->getArrays();
    if (!arrays->isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:arrays"));
        for (QSharedPointer<Array> array : *arrays)
        {
            writer.writeStartElement(QStringLiteral("ipxact:array"));
            writer.writeTextElement(QStringLiteral("ipxact:left"), array->getLeft());
            writer.writeTextElement(QStringLiteral("ipxact:right"), array->getRight());
            writer.writeEndElement();
        }
        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterWriter::writeVectors()
//-----------------------------------------------------------------------------
void ParameterWriter::writeVectors(QXmlStreamWriter& writer, QSharedPointer<Parameter> parameter) const
{
    QSharedPointer<QList<QSharedPointer<Vector> > > vectors = parameter->getVectors();
    if (!vectors->isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:vectors"));
        for (QSharedPointer<Vector> vector : *vectors)
        {
            writer.writeStartElement(QStringLiteral("ipxact:vector"));
            writer.writeTextElement(QStringLiteral("ipxact:left"), vector->getLeft());
            writer.writeTextElement(QStringLiteral("ipxact:right"), vector->getRight());
            writer.writeEndElement();
        }
        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterWriter::writeValue()
//-----------------------------------------------------------------------------
void ParameterWriter::writeValue(QXmlStreamWriter& writer, QSharedPointer<Parameter> parameter) const
{
    writer.writeStartElement(QStringLiteral("ipxact:value"));
    
    for (auto const& attribute : parameter->getValueAttributeNames())
    {
        writer.writeAttribute(attribute, parameter->getValueAttribute(attribute));
    }

    writer.writeCharacters(parameter->getValue());
    writer.writeEndElement();
}
