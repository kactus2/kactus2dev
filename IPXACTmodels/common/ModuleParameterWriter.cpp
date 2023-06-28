//-----------------------------------------------------------------------------
// File: ModuleParameterWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 08.09.2015
//
// Description:
// Writer class for IP-XACT Module parameter element.
//-----------------------------------------------------------------------------

#include "ModuleParameterWriter.h"
#include "CommonItemsWriter.h"

//-----------------------------------------------------------------------------
// Function: ModuleParameterWriter::ModuleParameterWriter()
//-----------------------------------------------------------------------------
ModuleParameterWriter::ModuleParameterWriter():
ParameterWriter()
{

}

//-----------------------------------------------------------------------------
// Function: ModuleParameterWriter::~ModuleParameterWriter()
//-----------------------------------------------------------------------------
ModuleParameterWriter::~ModuleParameterWriter()
{

}

//-----------------------------------------------------------------------------
// Function: ModuleParameterWriter::writeModuleParameter()
//-----------------------------------------------------------------------------
void ModuleParameterWriter::writeModuleParameter(QXmlStreamWriter& writer,
    QSharedPointer<ModuleParameter> moduleParameter, Document::Revision docRevision) const
{
    writer.writeStartElement(QStringLiteral("ipxact:moduleParameter"));

    writeAttributes(writer, moduleParameter);

    writeNameGroup(writer, moduleParameter, docRevision);

    writeVectors(writer, moduleParameter);

    writeArrays(writer, moduleParameter);

    writeValue(writer, moduleParameter);

    CommonItemsWriter::writeVendorExtensions(writer, moduleParameter);

    CommonItemsWriter::writeIsPresent(writer, moduleParameter->getIsPresent());

    writer.writeEndElement(); // ipxact:moduleParameter
}
