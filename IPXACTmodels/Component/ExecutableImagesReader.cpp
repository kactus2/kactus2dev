//-----------------------------------------------------------------------------
// File: ExecutableImagesReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 27.5.2026
//
// Description:
// Reader for IP-XACT executableImage elements.
//-----------------------------------------------------------------------------

#include "ExecutableImagesReader.h"

#include <IPXACTmodels/common/NameGroupReader.h>
#include <IPXACTmodels/common/FileBuilderReader.h>
#include <IPXACTmodels/common/CommonItemsReader.h>

QSharedPointer<QList<QSharedPointer<ExecutableImage> > > ExecutableImagesReader::parseAndCreateExecutableImages(
    QDomNode const& parentNode, Document::Revision docRevision)
{
    auto executableImageNodes = parentNode.toElement().elementsByTagName(QStringLiteral("ipxact:executableImage"));
    QSharedPointer<QList<QSharedPointer<ExecutableImage> > > executableImages(new QList<QSharedPointer<ExecutableImage> >());

    for (int i = 0; i < executableImageNodes.count(); ++i)
    {
        QDomNode imageNode = executableImageNodes.at(i);
        if (imageNode.isElement())
        {
            executableImages->append(Details::parseAndCreateExecutableImage(imageNode, docRevision));
        }
    }

    return executableImages;
}

QSharedPointer<ExecutableImage> ExecutableImagesReader::Details::parseAndCreateExecutableImage(QDomNode const& parentNode, Document::Revision docRevision)
{
    QSharedPointer<ExecutableImage> image(new ExecutableImage);

    NameGroupReader::parseNameGroup(parentNode, image);

    auto imageElement = parentNode.toElement();
    auto id = imageElement.attribute(QStringLiteral("imageId"));
    image->setImageId(id);

    auto type = imageElement.attribute(QStringLiteral("imageType"));
    image->setImageType(type);
    image->setParameters(CommonItemsReader::parseAndCreateParameters(parentNode, docRevision));

    auto languageToolsElement = parentNode.firstChildElement(QStringLiteral("ipxact:languageTools"));
    if (!languageToolsElement.isNull())
    {
        Details::parseLanguageTools(languageToolsElement, image);
    }

    return image;
}

void ExecutableImagesReader::Details::parseLanguageTools(QDomElement const& languageToolsElement,
    QSharedPointer<ExecutableImage> image)
{
    QDomNodeList fileBuilderNodes = languageToolsElement.elementsByTagName(QStringLiteral("ipxact:fileBuilder"));

    QSharedPointer<LanguageTools> languageTools(new LanguageTools);
    
    const int BUILDER_COUNT = fileBuilderNodes.count();
    for (int i = 0; i < BUILDER_COUNT; ++i)
    {
        QDomNode builderElement = fileBuilderNodes.at(i);

        auto fileBuilder = FileBuilderReader::createFileBuilderFrom(builderElement, Document::Revision::Std22);
        languageTools->getFileBuilders()->append(fileBuilder);
    }

    auto linkerElement = languageToolsElement.firstChildElement(QStringLiteral("ipxact:linker"));
    languageTools->setLinker(linkerElement.firstChild().nodeValue());

    auto linkerFlagsElement = languageToolsElement.firstChildElement(QStringLiteral("ipxact:linkerFlags"));
    languageTools->setLinkerFlags(linkerFlagsElement.firstChild().nodeValue());

    auto linkerCommandFileElement = languageToolsElement.firstChildElement(QStringLiteral("ipxact:linkerCommandFile"));

    Details::parseLinkerCommandFile(linkerCommandFileElement, languageTools);

    image->setLanguageTools(languageTools);
}

void ExecutableImagesReader::Details::parseLinkerCommandFile(QDomElement const& linkerCommandFileElement,
    QSharedPointer<LanguageTools> languageTools)
{
    auto linkerCommandFile = languageTools->getLinkerCommandFile();

    auto nameElement = linkerCommandFileElement.firstChildElement(QStringLiteral("ipxact:name"));
    linkerCommandFile->name_ = nameElement.firstChild().nodeValue();

    auto switchElement = linkerCommandFileElement.firstChildElement(QStringLiteral("ipxact:commandLineSwitch"));
    linkerCommandFile->commandLineSwitch_ = switchElement.firstChild().nodeValue();

    auto enableElement = linkerCommandFileElement.firstChildElement(QStringLiteral("ipxact:enable"));
    linkerCommandFile->enable_ = enableElement.firstChild().nodeValue();

    auto generatorRefs = linkerCommandFileElement.elementsByTagName(QStringLiteral("ipxact:generatorRef"));
    const int GENERATOR_COUNT = generatorRefs.count();
    for (int i = 0; i < GENERATOR_COUNT; ++i)
    {
        QDomNode generatorNode = generatorRefs.at(i);
        linkerCommandFile->generatorRefs_.append(generatorNode.firstChild().nodeValue());
    }

    CommonItemsReader::parseVendorExtensions(linkerCommandFileElement, linkerCommandFile);
}
