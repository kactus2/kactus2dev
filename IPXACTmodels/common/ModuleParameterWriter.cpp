//-----------------------------------------------------------------------------
// File: ModuleParameterWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 08.09.2015
//
// Description:
// Writer for IP-XACT Module parameter element.
//-----------------------------------------------------------------------------

#include "ModuleParameterWriter.h"
#include "CommonItemsWriter.h"

//-----------------------------------------------------------------------------
// Function: ModuleParameterWriter::writeModuleParameter()
//-----------------------------------------------------------------------------
void ModuleParameterWriter::writeModuleParameter(QXmlStreamWriter& writer,
    QSharedPointer<ModuleParameter> moduleParameter, Document::Revision docRevision)
{
    writer.writeStartElement(QStringLiteral("ipxact:moduleParameter"));

    ParameterWriter::Details::writeAttributes(writer, moduleParameter);

    ParameterWriter::Details::writeNameGroup(writer, moduleParameter, docRevision);

    ParameterWriter::Details::writeVectors(writer, moduleParameter, docRevision);

    ParameterWriter::Details::writeArrays(writer, moduleParameter, docRevision);

    ParameterWriter::Details::writeValue(writer, moduleParameter);

    CommonItemsWriter::writeVendorExtensions(writer, moduleParameter);

    if (docRevision == Document::Revision::Std14)
    {
        CommonItemsWriter::writeIsPresent(writer, moduleParameter->getIsPresent());
    }

    writer.writeEndElement(); // ipxact:moduleParameter
}
