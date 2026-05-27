//-----------------------------------------------------------------------------
// File: ExecutableImagesWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 27.5.2026
//
// Description:
// Writer for IP-XACT executableImage elements.
//-----------------------------------------------------------------------------

#include "ExecutableImagesWriter.h"

#include <IPXACTmodels/common/NameGroupWriter.h>
#include <IPXACTmodels/common/ParameterWriter.h>
#include <IPXACTmodels/common/FileBuilderWriter.h>
#include <IPXACTmodels/common/CommonItemsWriter.h>

//-----------------------------------------------------------------------------
// Function: ExecutableImagesWriter::writeExecutableImages()
//-----------------------------------------------------------------------------
void ExecutableImagesWriter::writeExecutableImages(QXmlStreamWriter& writer,
    QSharedPointer<QList<QSharedPointer<ExecutableImage> > > images, Document::Revision docRevision)
{    
    for (auto const& executableImage : *images)
    {
        Details::writeExecutableImage(writer, executableImage, docRevision);
    }
}

void ExecutableImagesWriter::Details::writeExecutableImage(QXmlStreamWriter& writer, QSharedPointer<ExecutableImage> image, Document::Revision docRevision)
{
    writer.writeStartElement(QStringLiteral("ipxact:executableImage"));
    writer.writeAttribute(QStringLiteral("imageId"), image->getImageId());
    CommonItemsWriter::writeNonEmptyAttribute(writer, QStringLiteral("imageType"), image->getImageType());

    NameGroupWriter::writeNameGroup(writer, image, docRevision);

    CommonItemsWriter::writeParameters(writer, image->getParameters(), docRevision);

    Details::writeLanguageTools(writer, image->getLanguageTools());

    writer.writeEndElement(); // ipxact:executableImage
}

//-----------------------------------------------------------------------------
// Function: ExecutableImagesWriter::Details::writeLanguageTools()
//-----------------------------------------------------------------------------
void ExecutableImagesWriter::Details::writeLanguageTools(QXmlStreamWriter& writer,
    QSharedPointer<LanguageTools> languageTools)
{
    if (languageTools.isNull())
    {
        return;
    }

    writer.writeStartElement(QStringLiteral("ipxact:languageTools"));

    for (auto const& fileBuilder : *languageTools->getFileBuilders())
    {
        FileBuilderWriter::writeFileBuilder(writer, fileBuilder, Document::Revision::Std22);
    }

    writer.writeTextElement(QStringLiteral("ipxact:linker"), languageTools->getLinker());

    CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:linkerFlags"),
        languageTools->getLinkerFlags());

    auto linkerCommandFile = languageTools->getLinkerCommandFile();
    if (linkerCommandFile->name_.isEmpty() == false)
    {
        writer.writeStartElement(QStringLiteral("ipxact:linkerCommandFile"));

        writer.writeTextElement(QStringLiteral("ipxact:name"), linkerCommandFile->name_);

        writer.writeTextElement(QStringLiteral("ipxact:commandLineSwitch"), linkerCommandFile->commandLineSwitch_);

        writer.writeTextElement(QStringLiteral("ipxact:enable"), linkerCommandFile->enable_);

        for (auto const& generatorRef : linkerCommandFile->generatorRefs_)
        {
            writer.writeTextElement(QStringLiteral("ipxact:generatorRef"), generatorRef);
        }

        CommonItemsWriter::writeVendorExtensions(writer, linkerCommandFile);

        writer.writeEndElement(); // ipxact:linkerCommandFile
    }

    writer.writeEndElement(); // ipxact:languageTools
}
