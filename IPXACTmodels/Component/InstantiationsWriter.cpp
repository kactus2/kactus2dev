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

    writeReference(writer, instantiation->getDesignReference(), "ipxact:designRef");

    writeVendorExtensions(writer, instantiation);

    writer.writeEndElement(); // ipxact:designInstantiation
}

//-----------------------------------------------------------------------------
// Function: InstantiationsWriter::writeReference()
//-----------------------------------------------------------------------------
void InstantiationsWriter::writeReference(QXmlStreamWriter& writer,
    QSharedPointer<ConfigurableVLNVReference> reference, QString const& elementName) const
{
    if (reference)
    {
        writer.writeStartElement(elementName);

        writeVLNVAttributes(writer, *reference);
        writeConfigurableElementValues(writer, reference->getConfigurableElementValues());

        writer.writeEndElement(); // elementName
    }
}

//-----------------------------------------------------------------------------
// Function: InstantiationsWriter::writeDesignConfigurationInstantiation()
//-----------------------------------------------------------------------------
void InstantiationsWriter::writeDesignConfigurationInstantiation(QXmlStreamWriter& writer,
    QSharedPointer<DesignConfigurationInstantiation> instantiation) const
{
    writer.writeStartElement("ipxact:designConfigurationInstantiation");

    NameGroupWriter nameWriter;
    nameWriter.writeNameGroup(writer, instantiation);

    writeLanguage(writer, instantiation->getLanguage(), instantiation->isLangugageStrict());

    writeReference(writer, instantiation->getDesignConfigurationReference(), "ipxact:designConfigurationRef");

    writeParameters(writer, instantiation->getParameters());

    writeVendorExtensions(writer, instantiation);

    writer.writeEndElement(); // ipxact:designConfigurationInstantiation
}

//-----------------------------------------------------------------------------
// Function: InstantiationsWriter::writeLanguage()
//-----------------------------------------------------------------------------
void InstantiationsWriter::writeLanguage(QXmlStreamWriter& writer, QString const& language,
    bool languageStrictness) const
{
    if (!language.isEmpty())
    {
        writer.writeStartElement("ipxact:language");

        if (languageStrictness)
        {
            writer.writeAttribute("strict", "true");
        }
        writer.writeCharacters(language);

        writer.writeEndElement(); // ipxact:language
    }
}