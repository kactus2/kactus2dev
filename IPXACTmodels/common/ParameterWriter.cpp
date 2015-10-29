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
#include <IPXACTmodels/common/NameGroupWriter.h>
#include <IPXACTmodels/common/VendorExtension.h>

#include <QSharedPointer>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
// Function: ParameterWriter::ParameterWriter()
//-----------------------------------------------------------------------------
ParameterWriter::ParameterWriter(QObject* parent) : QObject(parent)
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
    writer.writeStartElement("ipxact:parameter");
    writeAttributes(writer, parameter);
    
    writeNameGroup(writer, parameter);

    writeVectors(writer, parameter);

    writeArrays(writer, parameter);

    writeValue(writer, parameter);

    writeVendorExtensions(writer, parameter);

    writer.writeEndElement(); // ipxact:parameter
}

//-----------------------------------------------------------------------------
// Function: ParameterWriter::writeAttributes()
//-----------------------------------------------------------------------------
void ParameterWriter::writeAttributes(QXmlStreamWriter& writer, QSharedPointer<Parameter> parameter) const
{
    foreach (QString attribute, parameter->getAttributeNames())
    {
        writer.writeAttribute(attribute, parameter->getAttribute(attribute));
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterWriter::writeNameGroup()
//-----------------------------------------------------------------------------
void ParameterWriter::writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<Parameter> parameter) const
{
    NameGroupWriter nameWriter;
    nameWriter.writeNameGroup(writer, parameter);
}

//-----------------------------------------------------------------------------
// Function: ParameterWriter::writeArrays()
//-----------------------------------------------------------------------------
void ParameterWriter::writeArrays(QXmlStreamWriter& writer, QSharedPointer<Parameter> parameter) const
{
    QSharedPointer<QList<QSharedPointer<Array> > > arrays = parameter->getArrays();
    if (!arrays->isEmpty())
    {
        writer.writeStartElement("ipxact:arrays");
        foreach (QSharedPointer<Array> array, *arrays)
        {
            writer.writeStartElement("ipxact:array");
            writer.writeTextElement("ipxact:left", array->getLeft());
            writer.writeTextElement("ipxact:right", array->getRight());
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
        writer.writeStartElement("ipxact:vectors");
        foreach (QSharedPointer<Vector> vector, *vectors)
        {
            writer.writeStartElement("ipxact:vector");
            writer.writeTextElement("ipxact:left", vector->getLeft());
            writer.writeTextElement("ipxact:right", vector->getRight());
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
    writer.writeStartElement("ipxact:value");
    
    foreach (QString attribute, parameter->getValueAttributeNames())
    {
        writer.writeAttribute(attribute, parameter->getValueAttribute(attribute));
    }

    writer.writeCharacters(parameter->getValue());
    writer.writeEndElement();
}

//-----------------------------------------------------------------------------
// Function: ParameterWriter::writeVendorExtensions()
//-----------------------------------------------------------------------------
void ParameterWriter::writeVendorExtensions(QXmlStreamWriter& writer, QSharedPointer<Parameter> parameter) const
{
    if (!parameter->getVendorExtensions()->isEmpty())
    {
        writer.writeStartElement("ipxact:vendorExtensions");
        foreach (QSharedPointer<VendorExtension> extension, *parameter->getVendorExtensions())
        {
            extension->write(writer);
        }
        writer.writeEndElement();
    }
}
