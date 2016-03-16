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

#include <IPXACTmodels/common/FileBuilderWriter.h>
#include <IPXACTmodels/common/ModuleParameterWriter.h>
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

//-----------------------------------------------------------------------------
// Function: InstantiationsWriter::writeComponentInstantiation()
//-----------------------------------------------------------------------------
void InstantiationsWriter::writeComponentInstantiation(QXmlStreamWriter& writer,
    QSharedPointer<ComponentInstantiation> instantiation) const
{
    writer.writeStartElement("ipxact:componentInstantiation");

    NameGroupWriter nameWriter;
    nameWriter.writeNameGroup(writer, instantiation);

    writeLanguage(writer, instantiation->getLanguage(), instantiation->isLanguageStrict());

    if (instantiation->isVirtual())
    {
        writer.writeTextElement("ipxact:isVirtual", "true");
    }

    writeNameReferences(writer, instantiation);

    writeModuleParameters(writer, instantiation->getModuleParameters());

    writeDefaultFileBuilders(writer, instantiation->getDefaultFileBuilders());

    writeFileSetReferences(writer, instantiation->getFileSetReferences());

    writeParameters(writer, instantiation->getParameters());

    writeVendorExtensions(writer, instantiation);

    writer.writeEndElement(); // ipxact:componentInstantiation
}

//-----------------------------------------------------------------------------
// Function: InstantiationsWriter::writeNameReferences()
//-----------------------------------------------------------------------------
void InstantiationsWriter::writeNameReferences(QXmlStreamWriter& writer,
    QSharedPointer<ComponentInstantiation> instantiation) const
{
    if (!instantiation->getLibraryName().isEmpty())
    {
        writer.writeTextElement("ipxact:libraryName", instantiation->getLibraryName());
    }
    if (!instantiation->getPackageName().isEmpty())
    {
        writer.writeTextElement("ipxact:packageName", instantiation->getPackageName());
    }
    if (!instantiation->getModuleName().isEmpty())
    {
        writer.writeTextElement("ipxact:moduleName", instantiation->getModuleName());
    }
    if (!instantiation->getArchitectureName().isEmpty())
    {
        writer.writeTextElement("ipxact:architectureName", instantiation->getArchitectureName());
    }
    if (!instantiation->getConfigurationName().isEmpty())
    {
        writer.writeTextElement("ipxact:configurationName", instantiation->getConfigurationName());
    }
}

//-----------------------------------------------------------------------------
// Function: InstantiationsWriter::writeModuleParameters()
//-----------------------------------------------------------------------------
void InstantiationsWriter::writeModuleParameters(QXmlStreamWriter& writer,
    QSharedPointer<QList<QSharedPointer<ModuleParameter> > > moduleParameters) const
{
    if (!moduleParameters->isEmpty())
    {
        writer.writeStartElement("ipxact:moduleParameters");

        ModuleParameterWriter moduleParameterWriter;
        foreach (QSharedPointer<ModuleParameter> singleModuleParameter, *moduleParameters)
        {
            moduleParameterWriter.writeModuleParameter(writer, singleModuleParameter);
        }

        writer.writeEndElement(); // ipxact:moduleParameters
    }
}

//-----------------------------------------------------------------------------
// Function: InstantiationsWriter::writeDefaultFileBuilders()
//-----------------------------------------------------------------------------
void InstantiationsWriter::writeDefaultFileBuilders(QXmlStreamWriter& writer,
    QSharedPointer<QList<QSharedPointer<FileBuilder> > > defautlFileBuilders) const
{
    if (!defautlFileBuilders->isEmpty())
    {
        FileBuilderWriter fileBuilderWriter;

        foreach (QSharedPointer<FileBuilder> fileBuilder, *defautlFileBuilders)
        {
            fileBuilderWriter.writeDefaultFileBuilder(writer, fileBuilder);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: InstantiationsWriter::writeFileSetReferences()
//-----------------------------------------------------------------------------
void InstantiationsWriter::writeFileSetReferences(QXmlStreamWriter& writer, QSharedPointer<QStringList> references)
    const
{
    if (!references->isEmpty())
    {
        foreach (QString fileSetReference, *references)
        {
            writer.writeStartElement("ipxact:fileSetRef");

            writer.writeTextElement("ipxact:localName", fileSetReference);

            writer.writeEndElement(); // ipxact:fileSetRef
        }
    }
}
