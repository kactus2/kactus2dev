//-----------------------------------------------------------------------------
// File: InstantiationsWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.09.2015
//
// Description:
// Writer class for IP-XACT instantiations element.
//-----------------------------------------------------------------------------

#include "InstantiationsWriter.h"

#include <IPXACTmodels/common/NameGroupWriter.h>

//-----------------------------------------------------------------------------
// Function: InstantiationsWriter::InstantiationsWriter()
//-----------------------------------------------------------------------------
InstantiationsWriter::InstantiationsWriter(QObject* parent /* = 0 */) :
CommonItemsWriter(parent)
{

}

//-----------------------------------------------------------------------------
// Function: InstantiationsWriter::~InstantiationsWriter()
//-----------------------------------------------------------------------------
InstantiationsWriter::~InstantiationsWriter()
{

}

//-----------------------------------------------------------------------------
// Function: InstantiationsWriter::writeDesignInstantiation()
//-----------------------------------------------------------------------------
void InstantiationsWriter::writeDesignInstantiation(QXmlStreamWriter& writer,
    QSharedPointer<DesignInstantiation> instantiation) const
{
    writer.writeStartElement("ipxact:designInstantiation");

    NameGroupWriter nameWriter;
    nameWriter.writeNameGroup(writer, instantiation);

    if (instantiation->getDesignReference())
    {
        writer.writeStartElement("ipxact:designRef");
            //writeConfigurableVLNVReference(writer, instantiation->getDesignReference());

        QSharedPointer<ConfigurableVLNVReference> designReference = instantiation->getDesignReference();

        writeVLNVAttributes(writer, *designReference);
        writeConfigurableElementValues(writer, designReference->getConfigurableElementValues());

        writer.writeEndElement(); // ipxact:designRef
    }

    writeVendorExtensions(writer, instantiation);

    writer.writeEndElement(); // ipxact:designInstantiation
}
