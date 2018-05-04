//-----------------------------------------------------------------------------
// File: CatalogReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 07.08.2015
//
// Description:
// XML reader class for IP-XACT Parameter element.
//-----------------------------------------------------------------------------

#include "CatalogReader.h"

#include "Catalog.h"
#include "IpxactFile.h"


//-----------------------------------------------------------------------------
// Function: CatalogReader::CatalogReader()
//-----------------------------------------------------------------------------
CatalogReader::CatalogReader(): DocumentReader()
{

}

//-----------------------------------------------------------------------------
// Function: CatalogReader::~CatalogReader()
//-----------------------------------------------------------------------------
CatalogReader::~CatalogReader()
{

}

//-----------------------------------------------------------------------------
// Function: CatalogReader::createCatalogFrom()
//-----------------------------------------------------------------------------
QSharedPointer<Catalog> CatalogReader::createCatalogFrom(QDomNode const& document) const
{
    QSharedPointer<Catalog> catalog(new Catalog());

    parseTopComments(document, catalog);

    parseXMLProcessingInstructions(document, catalog);

    QDomNode catalogNode = document.firstChildElement();
    parseNamespaceDeclarations(catalogNode, catalog);

    parseVLNVElements(catalogNode, catalog, VLNV::CATALOG);

    parseDescription(catalogNode, catalog);
  
    parseCatalogs(catalogNode, catalog);

    parseBusDefinitions(catalogNode, catalog);

    parseAbstractionDefinitions(catalogNode, catalog);

    parseComponents(catalogNode, catalog);

    parseAbstractors(catalogNode, catalog);

    parseDesigns(catalogNode, catalog);
    
    parseDesignCongfigurations(catalogNode, catalog);

    parseGeneratorChains(catalogNode, catalog);

    parseKactusAndVendorExtensions(catalogNode, catalog);

    return catalog;
}

//-----------------------------------------------------------------------------
// Function: CatalogReader::parseCatalogs()
//-----------------------------------------------------------------------------
void CatalogReader::parseCatalogs(QDomNode const& catalogNode, QSharedPointer<Catalog> catalog) const
{   
    QDomElement catalogsNode = catalogNode.firstChildElement(QStringLiteral("ipxact:catalogs"));
    
    catalog->getCatalogs()->append(parseIpxactFileList(catalogsNode));
}

//-----------------------------------------------------------------------------
// Function: CatalogReader::parseBusDefinitions()
//-----------------------------------------------------------------------------
void CatalogReader::parseBusDefinitions(QDomNode const& catalogNode, QSharedPointer<Catalog> catalog) const
{   
    QDomElement busDefinitionsNode = catalogNode.firstChildElement(QStringLiteral("ipxact:busDefinitions"));

    catalog->getBusDefinitions()->append(parseIpxactFileList(busDefinitionsNode));
}

//-----------------------------------------------------------------------------
// Function: CatalogReader::parseAbstractionDefinitions()
//-----------------------------------------------------------------------------
void CatalogReader::parseAbstractionDefinitions(QDomNode const& catalogNode, QSharedPointer<Catalog> catalog) const
{   
    QDomElement abstractionsNode = catalogNode.firstChildElement(QStringLiteral("ipxact:abstractionDefinitions"));

    catalog->getAbstractionDefinitions()->append(parseIpxactFileList(abstractionsNode));
}

//-----------------------------------------------------------------------------
// Function: CatalogReader::parseComponents()
//-----------------------------------------------------------------------------
void CatalogReader::parseComponents(QDomNode const& catalogNode, QSharedPointer<Catalog> catalog) const
{   
    QDomElement componentsNode = catalogNode.firstChildElement(QStringLiteral("ipxact:components"));

    catalog->getComponents()->append(parseIpxactFileList(componentsNode));
}

//-----------------------------------------------------------------------------
// Function: CatalogReader::parseAbstractors()
//-----------------------------------------------------------------------------
void CatalogReader::parseAbstractors(QDomNode const& catalogNode, QSharedPointer<Catalog> catalog) const
{   
    QDomElement abstractorsNode = catalogNode.firstChildElement(QStringLiteral("ipxact:abstractors"));

    catalog->getAbstractors()->append(parseIpxactFileList(abstractorsNode));
}

//-----------------------------------------------------------------------------
// Function: CatalogReader::parseDesigns()
//-----------------------------------------------------------------------------
void CatalogReader::parseDesigns(QDomNode const& catalogNode, QSharedPointer<Catalog> catalog) const
{   
    QDomElement designsNode = catalogNode.firstChildElement(QStringLiteral("ipxact:designs"));

    catalog->getDesigns()->append(parseIpxactFileList(designsNode));
}

//-----------------------------------------------------------------------------
// Function: CatalogReader::parseDesignCongfigurations()
//-----------------------------------------------------------------------------
void CatalogReader::parseDesignCongfigurations(QDomNode const& catalogNode, QSharedPointer<Catalog> catalog) const
{   
    QDomElement configurationsNode = catalogNode.firstChildElement(QStringLiteral("ipxact:designConfigurations"));

    catalog->getDesignConfigurations()->append(parseIpxactFileList(configurationsNode));
}

//-----------------------------------------------------------------------------
// Function: CatalogReader::parseGeneratorChains()
//-----------------------------------------------------------------------------
void CatalogReader::parseGeneratorChains(QDomNode const& catalogNode, QSharedPointer<Catalog> catalog) const
{   
    QDomElement chainsNode = catalogNode.firstChildElement(QStringLiteral("ipxact:generatorChains"));

    catalog->getGeneratorChains()->append(parseIpxactFileList(chainsNode));
}

//-----------------------------------------------------------------------------
// Function: CatalogReader::parseIpxactFileList()
//-----------------------------------------------------------------------------
QList<QSharedPointer<IpxactFile> > CatalogReader::parseIpxactFileList(QDomElement & filesNode) const
{
    QList<QSharedPointer<IpxactFile> > files;

    if (!filesNode.isNull())
    {
        QDomNodeList fileNodeList = filesNode.elementsByTagName(QStringLiteral("ipxact:ipxactFile"));
        for (int fileIndex = 0; fileIndex < fileNodeList.count(); fileIndex++)
        {
            files.append(createIpxactFileFrom(fileNodeList.at(fileIndex)));
        }
    }

    return files;
}

//-----------------------------------------------------------------------------
// Function: CatalogReader::createIpxactFileFrom()
//-----------------------------------------------------------------------------
QSharedPointer<IpxactFile> CatalogReader::createIpxactFileFrom(QDomNode const& fileNode) const
{
    QSharedPointer<IpxactFile> ipxactFile(new IpxactFile());

    QDomNode vlnvNode = fileNode.firstChildElement(QStringLiteral("ipxact:vlnv"));
    ipxactFile->setVlnv(CommonItemsReader::parseVLNVAttributes(vlnvNode, VLNV::CATALOG));

    ipxactFile->setName(fileNode.firstChildElement(QStringLiteral("ipxact:name")).firstChild().nodeValue());

    QDomNode descriptionNode = fileNode.firstChildElement(QStringLiteral("ipxact:description"));
    ipxactFile->setDescription(descriptionNode.firstChild().nodeValue());

    CommonItemsReader::parseVendorExtensions(fileNode, ipxactFile);

    return ipxactFile;    
}
