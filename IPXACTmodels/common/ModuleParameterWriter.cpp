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

//-----------------------------------------------------------------------------
// Function: ModuleParameterWriter::ModuleParameterWriter()
//-----------------------------------------------------------------------------
ModuleParameterWriter::ModuleParameterWriter(QObject* parent /* = 0 */):
ParameterWriter(parent)
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
    QSharedPointer<ModuleParameter> moduleParameter) const
{
    writer.writeStartElement("ipxact:moduleParameter");

    writeAttributes(writer, moduleParameter);

    writeNameGroup(writer, moduleParameter);

    writeVectors(writer, moduleParameter);

    writeArrays(writer, moduleParameter);

    writeValue(writer, moduleParameter);

    writeVendorExtensions(writer, moduleParameter);

    writeIsPresent(writer, moduleParameter);

    writer.writeEndElement(); // ipxact:moduleParameter
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterWriter::writeIsPresent()
//-----------------------------------------------------------------------------
void ModuleParameterWriter::writeIsPresent(QXmlStreamWriter& writer,
    QSharedPointer<ModuleParameter> moduleParameter) const
{
    if (!moduleParameter->getIsPresent().isEmpty())
    {
        writer.writeTextElement("ipxact:isPresent", moduleParameter->getIsPresent());
    }
}
