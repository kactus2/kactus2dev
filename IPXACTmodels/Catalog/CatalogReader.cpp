//-----------------------------------------------------------------------------
// File: CatalogReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 07.08.2015
//
// Description:
// XML reader for IP-XACT Parameter element.
//-----------------------------------------------------------------------------

#include "CatalogReader.h"

#include "Catalog.h"
#include "IpxactFile.h"


//-----------------------------------------------------------------------------
// Function: CatalogReader::createCatalogFrom()
//-----------------------------------------------------------------------------
QSharedPointer<Catalog> CatalogReader::createCatalogFrom(QDomNode const& document)
{
    QDomNode catalogNode = document.firstChildElement();
    
    Document::Revision docRevision = DocumentReader::getXMLDocumentRevision(catalogNode);

    VLNV vlnv = CommonItemsReader::createVLNVFrom(catalogNode, VLNV::CATALOG);

    QSharedPointer<Catalog> catalog(new Catalog(vlnv, docRevision));

    DocumentReader::parseTopComments(document, catalog);

    DocumentReader::parseXMLProcessingInstructions(document, catalog);

    DocumentReader::parseNamespaceDeclarations(catalogNode, catalog);

    DocumentReader::parseDocumentNameGroup(catalogNode, catalog);

    if (catalog->getRevision() != Document::Revision::Std22)
    {
        DocumentReader::parseDescription(catalogNode, catalog);
    }

    Details::parseCatalogs(catalogNode, catalog);

    Details::parseBusDefinitions(catalogNode, catalog);

    Details::parseAbstractionDefinitions(catalogNode, catalog);

    Details::parseComponents(catalogNode, catalog);

    Details::parseAbstractors(catalogNode, catalog);

    Details::parseDesigns(catalogNode, catalog);
    
    Details::parseDesignCongfigurations(catalogNode, catalog);

    Details::parseGeneratorChains(catalogNode, catalog);

    Details::parseTypeDefinitions(catalogNode, catalog);

    DocumentReader::parseKactusAndVendorExtensions(catalogNode, catalog);

    return catalog;
}

//-----------------------------------------------------------------------------
// Function: CatalogReader::Details::parseCatalogs()
//-----------------------------------------------------------------------------
void CatalogReader::Details::parseCatalogs(QDomNode const& catalogNode, QSharedPointer<Catalog> catalog)
{   
    QDomElement catalogsNode = catalogNode.firstChildElement(QStringLiteral("ipxact:catalogs"));
    
    catalog->getCatalogs()->append(parseIpxactFileList(catalogsNode, VLNV::CATALOG));
}

//-----------------------------------------------------------------------------
// Function: CatalogReader::Details::parseBusDefinitions()
//-----------------------------------------------------------------------------
void CatalogReader::Details::parseBusDefinitions(QDomNode const& catalogNode, QSharedPointer<Catalog> catalog)
{   
    QDomElement busDefinitionsNode = catalogNode.firstChildElement(QStringLiteral("ipxact:busDefinitions"));

    catalog->getBusDefinitions()->append(parseIpxactFileList(busDefinitionsNode, VLNV::BUSDEFINITION));
}

//-----------------------------------------------------------------------------
// Function: CatalogReader::Details::parseAbstractionDefinitions()
//-----------------------------------------------------------------------------
void CatalogReader::Details::parseAbstractionDefinitions(QDomNode const& catalogNode, QSharedPointer<Catalog> catalog)
{   
    QDomElement abstractionsNode = catalogNode.firstChildElement(QStringLiteral("ipxact:abstractionDefinitions"));

    catalog->getAbstractionDefinitions()->append(parseIpxactFileList(abstractionsNode, VLNV::ABSTRACTIONDEFINITION));
}

//-----------------------------------------------------------------------------
// Function: CatalogReader::Details::parseComponents()
//-----------------------------------------------------------------------------
void CatalogReader::Details::parseComponents(QDomNode const& catalogNode, QSharedPointer<Catalog> catalog)
{   
    QDomElement componentsNode = catalogNode.firstChildElement(QStringLiteral("ipxact:components"));

    catalog->getComponents()->append(parseIpxactFileList(componentsNode, VLNV::COMPONENT));
}

//-----------------------------------------------------------------------------
// Function: CatalogReader::Details::parseAbstractors()
//-----------------------------------------------------------------------------
void CatalogReader::Details::parseAbstractors(QDomNode const& catalogNode, QSharedPointer<Catalog> catalog)
{   
    QDomElement abstractorsNode = catalogNode.firstChildElement(QStringLiteral("ipxact:abstractors"));

    catalog->getAbstractors()->append(parseIpxactFileList(abstractorsNode, VLNV::ABSTRACTOR));
}

//-----------------------------------------------------------------------------
// Function: CatalogReader::Details::parseDesigns()
//-----------------------------------------------------------------------------
void CatalogReader::Details::parseDesigns(QDomNode const& catalogNode, QSharedPointer<Catalog> catalog)
{   
    QDomElement designsNode = catalogNode.firstChildElement(QStringLiteral("ipxact:designs"));

    catalog->getDesigns()->append(parseIpxactFileList(designsNode, VLNV::DESIGN));
}

//-----------------------------------------------------------------------------
// Function: CatalogReader::Details::parseDesignCongfigurations()
//-----------------------------------------------------------------------------
void CatalogReader::Details::parseDesignCongfigurations(QDomNode const& catalogNode, QSharedPointer<Catalog> catalog)
{   
    QDomElement configurationsNode = catalogNode.firstChildElement(QStringLiteral("ipxact:designConfigurations"));

    catalog->getDesignConfigurations()->append(parseIpxactFileList(configurationsNode, VLNV::DESIGNCONFIGURATION));
}

//-----------------------------------------------------------------------------
// Function: CatalogReader::Details::parseGeneratorChains()
//-----------------------------------------------------------------------------
void CatalogReader::Details::parseGeneratorChains(QDomNode const& catalogNode, QSharedPointer<Catalog> catalog)
{   
    QDomElement chainsNode = catalogNode.firstChildElement(QStringLiteral("ipxact:generatorChains"));

    catalog->getGeneratorChains()->append(parseIpxactFileList(chainsNode, VLNV::GENERATORCHAIN));
}

//-----------------------------------------------------------------------------
// Function: CatalogReader::Details::parseTypeDefinitions()
//-----------------------------------------------------------------------------
void CatalogReader::Details::parseTypeDefinitions(QDomNode const& catalogNode, QSharedPointer<Catalog> catalog)
{
    if (catalog->getRevision() != Document::Revision::Std22)
    {
        return;
    }
    
    QDomElement typeDefsNode = catalogNode.firstChildElement(QStringLiteral("ipxact:typeDefinitions"));
    
    catalog->getTypeDefinitions()->append(parseIpxactFileList(typeDefsNode, VLNV::TYPEDEFINITION));
}

//-----------------------------------------------------------------------------
// Function: CatalogReader::Details::parseIpxactFileList()
//-----------------------------------------------------------------------------
QList<QSharedPointer<IpxactFile> > CatalogReader::Details::parseIpxactFileList(QDomElement & filesNode, 
    VLNV::IPXactType documentType)
{
    QList<QSharedPointer<IpxactFile> > files;

    if (!filesNode.isNull())
    {
        QDomNodeList fileNodeList = filesNode.elementsByTagName(QStringLiteral("ipxact:ipxactFile"));
        for (int fileIndex = 0; fileIndex < fileNodeList.count(); fileIndex++)
        {
            files.append(createIpxactFileFrom(fileNodeList.at(fileIndex), documentType));
        }
    }

    return files;
}

//-----------------------------------------------------------------------------
// Function: CatalogReader::Details::createIpxactFileFrom()
//-----------------------------------------------------------------------------
QSharedPointer<IpxactFile> CatalogReader::Details::createIpxactFileFrom(QDomNode const& fileNode, 
    VLNV::IPXactType documentType)
{
    QSharedPointer<IpxactFile> ipxactFile(new IpxactFile());

    QDomNode vlnvNode = fileNode.firstChildElement(QStringLiteral("ipxact:vlnv"));
    ipxactFile->setVlnv(CommonItemsReader::parseVLNVAttributes(vlnvNode, documentType));

    ipxactFile->setName(fileNode.firstChildElement(QStringLiteral("ipxact:name")).firstChild().nodeValue());

    QDomNode descriptionNode = fileNode.firstChildElement(QStringLiteral("ipxact:description"));
    ipxactFile->setDescription(descriptionNode.firstChild().nodeValue());

    CommonItemsReader::parseVendorExtensions(fileNode, ipxactFile);

    return ipxactFile;    
}
