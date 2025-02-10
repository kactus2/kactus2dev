//-----------------------------------------------------------------------------
// File: CatalogWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.01.2017
//
// Description:
// Writer for IP-XACT Catalog element.
//-----------------------------------------------------------------------------

#include "CatalogWriter.h"

#include <IPXACTmodels/Catalog/Catalog.h>
#include <IPXACTmodels/Catalog/IpxactFile.h>

//-----------------------------------------------------------------------------
// Function: CatalogWriter::writeCatalog()
//-----------------------------------------------------------------------------
void CatalogWriter::writeCatalog(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog)
{
    writer.writeStartDocument();
    
    DocumentWriter::writeTopComments(writer, catalog);

    DocumentWriter::writeXmlProcessingInstructions(writer, catalog);

    writer.writeStartElement(QStringLiteral("ipxact:catalog"));
    DocumentWriter::writeNamespaceDeclarations(writer, catalog);

    DocumentWriter::writeDocumentNameGroup(writer, catalog);

    if (catalog->getRevision() != Document::Revision::Std22)
    {
        DocumentWriter::writeDescription(writer, catalog->getDescription());
    }

    Details::writeCatalogs(writer, catalog);

    Details::writeBusDefinitions(writer, catalog);

    Details::writeAbstractionDefinitions(writer, catalog);

    Details::writeComponents(writer, catalog);

    Details::writeAbstractors(writer, catalog);

    Details::writeDesigns(writer, catalog);

    Details::writeDesignConfigurations(writer, catalog);

    Details::writeGeneratorChains(writer, catalog);

    Details::writeTypeDefinitions(writer, catalog);

    CommonItemsWriter::writeVendorExtensions(writer, catalog);

    writer.writeEndElement(); // "ipxact:catalog"
    writer.writeEndDocument();
}

//-----------------------------------------------------------------------------
// Function: CatalogWriter::Details::writeCatalogs()
//-----------------------------------------------------------------------------
void CatalogWriter::Details::writeCatalogs(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog)
{
    writeIpxactFileGroup(writer, catalog->getCatalogs(), QStringLiteral("ipxact:catalogs"));
}

//-----------------------------------------------------------------------------
// Function: CatalogWriter::Details::writeBusDefinitions()
//-----------------------------------------------------------------------------
void CatalogWriter::Details::writeBusDefinitions(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog)
{
    writeIpxactFileGroup(writer, catalog->getBusDefinitions(), QStringLiteral("ipxact:busDefinitions"));
}

//-----------------------------------------------------------------------------
// Function: CatalogWriter::Details::writeAbstractionDefinitions()
//-----------------------------------------------------------------------------
void CatalogWriter::Details::writeAbstractionDefinitions(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog)
{
    writeIpxactFileGroup(writer, catalog->getAbstractionDefinitions(), QStringLiteral("ipxact:abstractionDefinitions"));
}

//-----------------------------------------------------------------------------
// Function: CatalogWriter::Details::writeComponents()
//-----------------------------------------------------------------------------
void CatalogWriter::Details::writeComponents(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog)
{
    writeIpxactFileGroup(writer, catalog->getComponents(), QStringLiteral("ipxact:components"));
}

//-----------------------------------------------------------------------------
// Function: CatalogWriter::Details::writeAbstractors()
//-----------------------------------------------------------------------------
void CatalogWriter::Details::writeAbstractors(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog)
{
    writeIpxactFileGroup(writer, catalog->getAbstractors(), QStringLiteral("ipxact:abstractors"));
}

//-----------------------------------------------------------------------------
// Function: CatalogWriter::Details::writeDesigns()
//-----------------------------------------------------------------------------
void CatalogWriter::Details::writeDesigns(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog)
{
    writeIpxactFileGroup(writer, catalog->getDesigns(), QStringLiteral("ipxact:designs"));
}

//-----------------------------------------------------------------------------
// Function: CatalogWriter::Details::writeDesignConfigurations()
//-----------------------------------------------------------------------------
void CatalogWriter::Details::writeDesignConfigurations(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog)
{
    writeIpxactFileGroup(writer, catalog->getDesignConfigurations(), QStringLiteral("ipxact:designConfigurations"));
}

//-----------------------------------------------------------------------------
// Function: CatalogWriter::Details::writeGeneratorChains()
//-----------------------------------------------------------------------------
void CatalogWriter::Details::writeGeneratorChains(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog)
{
    writeIpxactFileGroup(writer, catalog->getGeneratorChains(), QStringLiteral("ipxact:generatorChains"));
}

//-----------------------------------------------------------------------------
// Function: CatalogWriter::Details::writeTypeDefinitions()
//-----------------------------------------------------------------------------
void CatalogWriter::Details::writeTypeDefinitions(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog)
{
    if (catalog->getRevision() == Document::Revision::Std22)
    {
        writeIpxactFileGroup(writer, catalog->getTypeDefinitions(), QStringLiteral("ipxact:typeDefinitions"));    
    }
}

//-----------------------------------------------------------------------------
// Function: CatalogWriter::Details::writeIpxactFiles()
//-----------------------------------------------------------------------------
void CatalogWriter::Details::writeIpxactFileGroup(QXmlStreamWriter& writer, 
    QSharedPointer<QList<QSharedPointer<IpxactFile> > > files, QString const& elementName)
{
    if (files->isEmpty())
    {
        return;
    }

    writer.writeStartElement(elementName);

    for (auto const& file : *files)
    {
        writeIpxactFile(writer, file);
    }

    writer.writeEndElement();
}

//-----------------------------------------------------------------------------
// Function: CatalogWriter::Details::writeIpxactFile()
//-----------------------------------------------------------------------------
void CatalogWriter::Details::writeIpxactFile(QXmlStreamWriter& writer, QSharedPointer<IpxactFile> ipxactFile)
{
    writer.writeStartElement(QStringLiteral("ipxact:ipxactFile"));

    writer.writeEmptyElement(QStringLiteral("ipxact:vlnv"));
    CommonItemsWriter::writeVLNVAttributes(writer, ipxactFile->getVlnv());

    writer.writeStartElement(QStringLiteral("ipxact:name"));
    writer.writeCharacters(ipxactFile->getName());
    writer.writeEndElement(); // ipxact:name

    if (!ipxactFile->getDescription().isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:description"));
        writer.writeCharacters(ipxactFile->getDescription());
        writer.writeEndElement(); // ipxact:description
    }

    CommonItemsWriter::writeVendorExtensions(writer, ipxactFile);

    writer.writeEndElement(); // ipxact:ipxactFile
}
