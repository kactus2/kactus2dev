//-----------------------------------------------------------------------------
// File: CatalogWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.01.2017
//
// Description:
// Writer class for IP-XACT Catalog element.
//-----------------------------------------------------------------------------

#include "CatalogWriter.h"

#include <IPXACTmodels/Catalog/Catalog.h>
#include <IPXACTmodels/Catalog/IpxactFile.h>

//-----------------------------------------------------------------------------
// Function: CatalogWriter::CatalogWriter()
//-----------------------------------------------------------------------------
CatalogWriter::CatalogWriter(): DocumentWriter()
{

}

//-----------------------------------------------------------------------------
// Function: CatalogWriter::~CatalogWriter()
//-----------------------------------------------------------------------------
CatalogWriter::~CatalogWriter()
{

}

//-----------------------------------------------------------------------------
// Function: CatalogWriter::writeCatalog()
//-----------------------------------------------------------------------------
void CatalogWriter::writeCatalog(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog) const
{
    writer.writeStartDocument();
    
    writeTopComments(writer, catalog);

    writeXmlProcessingInstructions(writer, catalog);

    writer.writeStartElement(QStringLiteral("ipxact:catalog"));
    writeNamespaceDeclarations(writer, catalog);

    writeVLNVElements(writer, catalog->getVlnv());

    writeDescription(writer, catalog);

    writeCatalogs(writer, catalog);

    writeBusDefinitions(writer, catalog);

    writeAbstractionDefinitions(writer, catalog);

    writeComponents(writer, catalog);

    writeAbstractors(writer, catalog);

    writeDesigns(writer, catalog);

    writeDesignConfigurations(writer, catalog);

    writeGeneratorChains(writer, catalog);

    writeVendorExtensions(writer, catalog);

    writer.writeEndElement(); // "ipxact:catalog"
    writer.writeEndDocument();
}

//-----------------------------------------------------------------------------
// Function: CatalogWriter::writeCatalogs()
//-----------------------------------------------------------------------------
void CatalogWriter::writeCatalogs(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog) const
{
    writeIpxactFileGroup(writer, catalog->getCatalogs(), QStringLiteral("ipxact:catalogs"));
}

//-----------------------------------------------------------------------------
// Function: CatalogWriter::writeBusDefinitions()
//-----------------------------------------------------------------------------
void CatalogWriter::writeBusDefinitions(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog) const
{
    writeIpxactFileGroup(writer, catalog->getBusDefinitions(), QStringLiteral("ipxact:busDefinitions"));
}

//-----------------------------------------------------------------------------
// Function: CatalogWriter::writeAbstractionDefinitions()
//-----------------------------------------------------------------------------
void CatalogWriter::writeAbstractionDefinitions(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog) const
{
    writeIpxactFileGroup(writer, catalog->getAbstractionDefinitions(), QStringLiteral("ipxact:abstractionDefinitions"));
}

//-----------------------------------------------------------------------------
// Function: CatalogWriter::writeComponents()
//-----------------------------------------------------------------------------
void CatalogWriter::writeComponents(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog) const
{
    writeIpxactFileGroup(writer, catalog->getComponents(), QStringLiteral("ipxact:components"));
}

//-----------------------------------------------------------------------------
// Function: CatalogWriter::writeAbstractors()
//-----------------------------------------------------------------------------
void CatalogWriter::writeAbstractors(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog) const
{
    writeIpxactFileGroup(writer, catalog->getAbstractors(), QStringLiteral("ipxact:abstractors"));
}

//-----------------------------------------------------------------------------
// Function: CatalogWriter::writeDesigns()
//-----------------------------------------------------------------------------
void CatalogWriter::writeDesigns(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog) const
{
    writeIpxactFileGroup(writer, catalog->getDesigns(), QStringLiteral("ipxact:designs"));
}

//-----------------------------------------------------------------------------
// Function: CatalogWriter::writeDesignConfigurations()
//-----------------------------------------------------------------------------
void CatalogWriter::writeDesignConfigurations(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog) const
{
    writeIpxactFileGroup(writer, catalog->getDesignConfigurations(), QStringLiteral("ipxact:designConfigurations"));
}

//-----------------------------------------------------------------------------
// Function: CatalogWriter::writeGeneratorChains()
//-----------------------------------------------------------------------------
void CatalogWriter::writeGeneratorChains(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog) const
{
    writeIpxactFileGroup(writer, catalog->getGeneratorChains(), QStringLiteral("ipxact:generatorChains"));
}

//-----------------------------------------------------------------------------
// Function: CatalogWriter::writeIpxactFiles()
//-----------------------------------------------------------------------------
void CatalogWriter::writeIpxactFileGroup(QXmlStreamWriter& writer, 
    QSharedPointer<QList<QSharedPointer<IpxactFile> > > files, QString const& elementName) const
{
    if (files->isEmpty())
    {
        return;
    }

    writer.writeStartElement(elementName);

    foreach (QSharedPointer<IpxactFile> file, *files)
    {
        writeIpxactFile(writer, file);
    }

    writer.writeEndElement();
}

//-----------------------------------------------------------------------------
// Function: CatalogWriter::writeIpxactFile()
//-----------------------------------------------------------------------------
void CatalogWriter::writeIpxactFile(QXmlStreamWriter& writer, QSharedPointer<IpxactFile> ipxactFile) const
{
    writer.writeStartElement(QStringLiteral("ipxact:ipxactFile"));

    writer.writeEmptyElement(QStringLiteral("ipxact:vlnv"));
    writeVLNVAttributes(writer, ipxactFile->getVlnv());

    writer.writeStartElement(QStringLiteral("ipxact:name"));
    writer.writeCharacters(ipxactFile->getName());
    writer.writeEndElement(); // ipxact:name

    if (!ipxactFile->getDescription().isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:description"));
        writer.writeCharacters(ipxactFile->getDescription());
        writer.writeEndElement(); // ipxact:description
    }

    writeVendorExtensions(writer, ipxactFile);

    writer.writeEndElement(); // ipxact:ipxactFile
}
