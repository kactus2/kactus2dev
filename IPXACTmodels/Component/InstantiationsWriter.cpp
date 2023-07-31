//-----------------------------------------------------------------------------
// File: InstantiationsWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.09.2015
//
// Description:
// Writer for IP-XACT instantiations element.
//-----------------------------------------------------------------------------

#include "InstantiationsWriter.h"

#include <IPXACTmodels/common/FileBuilderWriter.h>
#include <IPXACTmodels/common/ModuleParameterWriter.h>
#include <IPXACTmodels/common/NameGroupWriter.h>

//-----------------------------------------------------------------------------
// Function: InstantiationsWriter::writeDesignInstantiation()
//-----------------------------------------------------------------------------
void InstantiationsWriter::writeDesignInstantiation(QXmlStreamWriter& writer,
    QSharedPointer<DesignInstantiation> instantiation, Document::Revision docRevision)
{
    writer.writeStartElement(QStringLiteral("ipxact:designInstantiation"));

    NameGroupWriter::writeNameGroup(writer, instantiation, docRevision);

    Details::writeReference(writer, instantiation->getDesignReference(), QStringLiteral("ipxact:designRef"));

    CommonItemsWriter::writeVendorExtensions(writer, instantiation);

    writer.writeEndElement(); // ipxact:designInstantiation
}

//-----------------------------------------------------------------------------
// Function: InstantiationsWriter::Details::writeReference()
//-----------------------------------------------------------------------------
void InstantiationsWriter::Details::writeReference(QXmlStreamWriter& writer,
    QSharedPointer<ConfigurableVLNVReference> reference, QString const& elementName)
{
    if (reference)
    {
        writer.writeStartElement(elementName);

        CommonItemsWriter::writeVLNVAttributes(writer, *reference);
        CommonItemsWriter::writeConfigurableElementValues(writer, reference->getConfigurableElementValues());

        writer.writeEndElement(); // elementName
    }
}

//-----------------------------------------------------------------------------
// Function: InstantiationsWriter::writeDesignConfigurationInstantiation()
//-----------------------------------------------------------------------------
void InstantiationsWriter::writeDesignConfigurationInstantiation(QXmlStreamWriter& writer,
    QSharedPointer<DesignConfigurationInstantiation> instantiation, Document::Revision docRevision)
{
    writer.writeStartElement(QStringLiteral("ipxact:designConfigurationInstantiation"));

    NameGroupWriter::writeNameGroup(writer, instantiation, docRevision);

    Details::writeLanguage(writer, instantiation->getLanguage(), instantiation->isLangugageStrict());

    Details::writeReference(writer, instantiation->getDesignConfigurationReference(), QStringLiteral("ipxact:designConfigurationRef"));

    CommonItemsWriter::writeParameters(writer, instantiation->getParameters(), docRevision);

    CommonItemsWriter::writeVendorExtensions(writer, instantiation);

    writer.writeEndElement(); // ipxact:designConfigurationInstantiation
}

//-----------------------------------------------------------------------------
// Function: InstantiationsWriter::Details::writeLanguage()
//-----------------------------------------------------------------------------
void InstantiationsWriter::Details::writeLanguage(QXmlStreamWriter& writer, QString const& language,
    bool languageStrictness)
{
    if (!language.isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:language"));

        if (languageStrictness)
        {
            writer.writeAttribute(QStringLiteral("strict"), QStringLiteral("true"));
        }
        writer.writeCharacters(language);

        writer.writeEndElement(); // ipxact:language
    }
}

//-----------------------------------------------------------------------------
// Function: InstantiationsWriter::writeComponentInstantiation()
//-----------------------------------------------------------------------------
void InstantiationsWriter::writeComponentInstantiation(QXmlStreamWriter& writer,
    QSharedPointer<ComponentInstantiation> instantiation, Document::Revision docRevision)
{
    writer.writeStartElement(QStringLiteral("ipxact:componentInstantiation"));

    NameGroupWriter::writeNameGroup(writer, instantiation, docRevision);

    Details::writeLanguage(writer, instantiation->getLanguage(), instantiation->isLanguageStrict());

    if (instantiation->isVirtual())
    {
        writer.writeTextElement(QStringLiteral("ipxact:isVirtual"), QStringLiteral("true"));
    }

    Details::writeNameReferences(writer, instantiation);

    Details::writeModuleParameters(writer, instantiation->getModuleParameters(), docRevision);

    Details::writeDefaultFileBuilders(writer, instantiation->getDefaultFileBuilders(), docRevision);

    Details::writeFileSetReferences(writer, instantiation->getFileSetReferences());

    CommonItemsWriter::writeParameters(writer, instantiation->getParameters(), docRevision);

    CommonItemsWriter::writeVendorExtensions(writer, instantiation);

    writer.writeEndElement(); // ipxact:componentInstantiation
}

//-----------------------------------------------------------------------------
// Function: InstantiationsWriter::Details::writeNameReferences()
//-----------------------------------------------------------------------------
void InstantiationsWriter::Details::writeNameReferences(QXmlStreamWriter& writer,
    QSharedPointer<ComponentInstantiation> instantiation)
{
    if (!instantiation->getLibraryName().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:libraryName"), instantiation->getLibraryName());
    }
    if (!instantiation->getPackageName().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:packageName"), instantiation->getPackageName());
    }
    if (!instantiation->getModuleName().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:moduleName"), instantiation->getModuleName());
    }
    if (!instantiation->getArchitectureName().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:architectureName"), instantiation->getArchitectureName());
    }
    if (!instantiation->getConfigurationName().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:configurationName"), instantiation->getConfigurationName());
    }
}

//-----------------------------------------------------------------------------
// Function: InstantiationsWriter::Details::writeModuleParameters()
//-----------------------------------------------------------------------------
void InstantiationsWriter::Details::writeModuleParameters(QXmlStreamWriter& writer,
    QSharedPointer<QList<QSharedPointer<ModuleParameter> > > moduleParameters,
    Document::Revision docRevision)
{
    if (!moduleParameters->isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:moduleParameters"));

        for (auto const& singleModuleParameter : *moduleParameters)
        {
            ModuleParameterWriter::writeModuleParameter(writer, singleModuleParameter, docRevision);
        }

        writer.writeEndElement(); // ipxact:moduleParameters
    }
}

//-----------------------------------------------------------------------------
// Function: InstantiationsWriter::Details::writeDefaultFileBuilders()
//-----------------------------------------------------------------------------
void InstantiationsWriter::Details::writeDefaultFileBuilders(QXmlStreamWriter& writer,
    QSharedPointer<QList<QSharedPointer<FileBuilder> > > defautlFileBuilders,
    Document::Revision docRevision)
{
    for (QSharedPointer<FileBuilder> fileBuilder : *defautlFileBuilders)
    {
        FileBuilderWriter::writeDefaultFileBuilder(writer, fileBuilder, docRevision);
    }
}

//-----------------------------------------------------------------------------
// Function: InstantiationsWriter::Details::writeFileSetReferences()
//-----------------------------------------------------------------------------
void InstantiationsWriter::Details::writeFileSetReferences(QXmlStreamWriter& writer, QSharedPointer<QStringList> references)
   
{
    if (!references->isEmpty())
    {
        for (auto const& fileSetReference : *references)
        {
            writer.writeStartElement(QStringLiteral("ipxact:fileSetRef"));

            writer.writeTextElement(QStringLiteral("ipxact:localName"), fileSetReference);

            writer.writeEndElement(); // ipxact:fileSetRef
        }
    }
}
