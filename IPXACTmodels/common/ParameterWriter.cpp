//-----------------------------------------------------------------------------
// File: ParameterWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 31.07.2015
//
// Description:
// Writer for IP-XACT Parameter element.
//-----------------------------------------------------------------------------

#include "ParameterWriter.h"

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/CommonItemsWriter.h>
#include <IPXACTmodels/common/NameGroupWriter.h>
#include <IPXACTmodels/common/VendorExtension.h>

#include <QSharedPointer>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
// Function: ParameterWriter::write()
//-----------------------------------------------------------------------------
void ParameterWriter::writeParameter(QXmlStreamWriter& writer, QSharedPointer<Parameter> parameter,
    Document::Revision docRevision)
{
    writer.writeStartElement(QStringLiteral("ipxact:parameter"));
    Details::writeAttributes(writer, parameter);
    
    Details::writeNameGroup(writer, parameter, docRevision);

    Details::writeVectors(writer, parameter, docRevision);

    Details::writeArrays(writer, parameter, docRevision);

    Details::writeValue(writer, parameter);

    CommonItemsWriter::writeVendorExtensions(writer, parameter);

    writer.writeEndElement(); // ipxact:parameter
}

//-----------------------------------------------------------------------------
// Function: ParameterWriter::Details::writeAttributes()
//-----------------------------------------------------------------------------
void ParameterWriter::Details::writeAttributes(QXmlStreamWriter& writer, QSharedPointer<Parameter> parameter)
{
    for (auto const& attribute : parameter->getAttributeNames())
    {
        writer.writeAttribute(attribute, parameter->getAttribute(attribute));
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterWriter::Details::writeNameGroup()
//-----------------------------------------------------------------------------
void ParameterWriter::Details::writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<Parameter> parameter, 
    Document::Revision docRevision)
{
    NameGroupWriter::writeNameGroup(writer, parameter, docRevision);
}

//-----------------------------------------------------------------------------
// Function: ParameterWriter::Details::writeArrays()
//-----------------------------------------------------------------------------
void ParameterWriter::Details::writeArrays(QXmlStreamWriter& writer, QSharedPointer<Parameter> parameter,
    Document::Revision docRevision)
{
    QSharedPointer<QList<QSharedPointer<Array> > > arrays = parameter->getArrays();
    if (!arrays->isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:arrays"));
        for (QSharedPointer<Array> array : *arrays)
        {
            writer.writeStartElement(QStringLiteral("ipxact:array"));
            if (docRevision == Document::Revision::Std22)
            {
                writer.writeAttribute(QStringLiteral("vectorId"), array->getId());
            }

            writer.writeTextElement(QStringLiteral("ipxact:left"), array->getLeft());
            writer.writeTextElement(QStringLiteral("ipxact:right"), array->getRight());
            writer.writeEndElement();
        }
        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterWriter::Details::writeVectors()
//-----------------------------------------------------------------------------
void ParameterWriter::Details::writeVectors(QXmlStreamWriter& writer, QSharedPointer<Parameter> parameter,
    Document::Revision docRevision)
{
    QSharedPointer<QList<QSharedPointer<Vector> > > vectors = parameter->getVectors();
    if (!vectors->isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:vectors"));
        for (QSharedPointer<Vector> vector : *vectors)
        {
            writer.writeStartElement(QStringLiteral("ipxact:vector"));

            if (docRevision == Document::Revision::Std22)
            {
                writer.writeAttribute(QStringLiteral("vectorId"), vector->getId());
            }

            writer.writeTextElement(QStringLiteral("ipxact:left"), vector->getLeft());
            writer.writeTextElement(QStringLiteral("ipxact:right"), vector->getRight());
            writer.writeEndElement();
        }
        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterWriter::Details::writeValue()
//-----------------------------------------------------------------------------
void ParameterWriter::Details::writeValue(QXmlStreamWriter& writer, QSharedPointer<Parameter> parameter)
{
    writer.writeStartElement(QStringLiteral("ipxact:value"));
    
    for (auto const& attribute : parameter->getValueAttributeNames())
    {
        writer.writeAttribute(attribute, parameter->getValueAttribute(attribute));
    }

    writer.writeCharacters(parameter->getValue());
    writer.writeEndElement();
}
