//-----------------------------------------------------------------------------
// File: Catalog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.01.2017
//
// Description:
// Implementation for ipxact:catalog element.
//-----------------------------------------------------------------------------

#include "Catalog.h"

#include "IpxactFile.h"

#include <QString>
#include <QObject>
#include <QSharedPointer>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: Catalog::Catalog()
//-----------------------------------------------------------------------------
Catalog::Catalog(VLNV const & vlnv, Document::Revision revision):
Document(vlnv, revision),
    catalogs_(new QList<QSharedPointer<IpxactFile> >()),
    busDefinitions_(new QList<QSharedPointer<IpxactFile> >()),
    abstractionDefinitions_(new QList<QSharedPointer<IpxactFile> >()),
    components_(new QList<QSharedPointer<IpxactFile> >()),
    abstractors_(new QList<QSharedPointer<IpxactFile> >()),
    designs_(new QList<QSharedPointer<IpxactFile> >()),
    designConfigurations_(new QList<QSharedPointer<IpxactFile> >()),
    generatorChains_(new QList<QSharedPointer<IpxactFile> >()),
    typeDefinitions_(new QList<QSharedPointer<IpxactFile> >())
{

}

//-----------------------------------------------------------------------------
// Function: Catalog::Catalog()
//-----------------------------------------------------------------------------
Catalog::Catalog(Catalog const& other):
Document(other),
    catalogs_(new QList<QSharedPointer<IpxactFile> >()),
    busDefinitions_(new QList<QSharedPointer<IpxactFile> >()),
    abstractionDefinitions_(new QList<QSharedPointer<IpxactFile> >()),
    components_(new QList<QSharedPointer<IpxactFile> >()),
    abstractors_(new QList<QSharedPointer<IpxactFile> >()),
    designs_(new QList<QSharedPointer<IpxactFile> >()),
    designConfigurations_(new QList<QSharedPointer<IpxactFile> >()),
    generatorChains_(new QList<QSharedPointer<IpxactFile> >()),
    typeDefinitions_(new QList<QSharedPointer<IpxactFile> >())
{
    copyIpxactFiles(other);
}

//-----------------------------------------------------------------------------
// Function: Catalog::operator=()
//-----------------------------------------------------------------------------
Catalog & Catalog::operator=(Catalog const& other)
{
    if (this != &other)
    {
        Document::operator=(other);
        
        catalogs_->clear();
        busDefinitions_->clear();
        abstractionDefinitions_->clear();
        components_->clear();
        abstractors_->clear();
        designs_->clear();
        designConfigurations_->clear();
        generatorChains_->clear();
        typeDefinitions_->clear();

        copyIpxactFiles(other);
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: Catalog::~Catalog()
//-----------------------------------------------------------------------------
Catalog::~Catalog()
{

}

//-----------------------------------------------------------------------------
// Function: Catalog::clone()
//-----------------------------------------------------------------------------
QSharedPointer<Document> Catalog::clone() const
{
    return QSharedPointer<Document>(new Catalog(*this));
}

//-----------------------------------------------------------------------------
// Function: Catalog::setVlnv()
//-----------------------------------------------------------------------------
void Catalog::setVlnv(VLNV const& vlnv)
{
    VLNV CatalogVLNV(vlnv);
    CatalogVLNV.setType(VLNV::CATALOG);

    Document::setVlnv(CatalogVLNV);
}

//-----------------------------------------------------------------------------
// Function: Catalog::getCatalogs()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<IpxactFile> > > Catalog::getCatalogs() const
{
    return catalogs_;
}

//-----------------------------------------------------------------------------
// Function: Catalog::getBusDefinitions()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<IpxactFile> > > Catalog::getBusDefinitions() const
{
    return busDefinitions_;
}

//-----------------------------------------------------------------------------
// Function: Catalog::getAbstractionDefinitions()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<IpxactFile> > > Catalog::getAbstractionDefinitions() const
{
    return abstractionDefinitions_;
}

//-----------------------------------------------------------------------------
// Function: Catalog::getComponents()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<IpxactFile> > > Catalog::getComponents() const
{
    return components_;
}

//-----------------------------------------------------------------------------
// Function: Catalog::getAbstractor()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<IpxactFile> > > Catalog::getAbstractors() const
{
    return abstractors_;
}

//-----------------------------------------------------------------------------
// Function: Catalog::getDesigns()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<IpxactFile> > > Catalog::getDesigns() const
{
    return designs_;
}

//-----------------------------------------------------------------------------
// Function: Catalog::getDesignConfigurations()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<IpxactFile> > > Catalog::getDesignConfigurations() const
{
    return designConfigurations_;
}

//-----------------------------------------------------------------------------
// Function: Catalog::getGeneratorChains()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<IpxactFile> > > Catalog::getGeneratorChains() const
{
    return generatorChains_;
}

//-----------------------------------------------------------------------------
// Function: Catalog::getTypeDefinitions()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<IpxactFile> > > Catalog::getTypeDefinitions() const
{
    return typeDefinitions_;
}

//-----------------------------------------------------------------------------
// Function: Catalog::getDependentVLNVs()
//-----------------------------------------------------------------------------
QList<VLNV> Catalog::getDependentVLNVs() const 
{
    QList<VLNV> vlnvs;

    for (auto const& file : getAllFiles())
    {
        vlnvs.append(file->getVlnv());
    }

    return vlnvs;
}

//-----------------------------------------------------------------------------
// Function: Catalog::getDependentFiles()
//-----------------------------------------------------------------------------
QStringList Catalog::getDependentFiles() const
{
    QStringList fileNames;

    for (auto const& file : getAllFiles())
    {
        fileNames.append(file->getName());
    }

    return fileNames;
}

//-----------------------------------------------------------------------------
// Function: Catalog::copyIpxactFiles()
//-----------------------------------------------------------------------------
void Catalog::copyIpxactFiles(Catalog const& other)
{
    for (auto const& catalog : *other.catalogs_)
    {
        catalogs_->append(QSharedPointer<IpxactFile>(new IpxactFile(*catalog)));
    }

    for (auto const& busDefinition : *other.busDefinitions_)
    {
        busDefinitions_->append(QSharedPointer<IpxactFile>(new IpxactFile(*busDefinition)));
    }

    for (auto const& abstractionDefinition : *other.abstractionDefinitions_)
    {
        abstractionDefinitions_->append(QSharedPointer<IpxactFile>(new IpxactFile(*abstractionDefinition)));
    }

    for (auto const& component : *other.components_)
    {
        components_->append(QSharedPointer<IpxactFile>(new IpxactFile(*component)));
    }

    for (auto const& abstractor : *other.abstractors_)
    {
        abstractors_->append(QSharedPointer<IpxactFile>(new IpxactFile(*abstractor)));
    }
    
    for (auto const& design : *other.designs_)
    {
        designs_->append(QSharedPointer<IpxactFile>(new IpxactFile(*design)));
    }

    for (auto const& configuration : *other.designConfigurations_)
    {
        designConfigurations_->append(QSharedPointer<IpxactFile>(new IpxactFile(*configuration)));
    }

    for (auto const& chain : *other.generatorChains_)
    {
        generatorChains_->append(QSharedPointer<IpxactFile>(new IpxactFile(*chain)));
    }
    
    for (auto const& typeDef : *other.typeDefinitions_)
    {
        typeDefinitions_->append(QSharedPointer<IpxactFile>(new IpxactFile(*typeDef)));
    }
}

//-----------------------------------------------------------------------------
// Function: Catalog::getAllFiles()
//-----------------------------------------------------------------------------
QList<QSharedPointer<IpxactFile> > Catalog::getAllFiles() const
{
    QList<QSharedPointer<IpxactFile> > files;

    files.append(*catalogs_);
    files.append(*busDefinitions_);
    files.append(*abstractionDefinitions_);
    files.append(*components_);
    files.append(*abstractors_);
    files.append(*designs_);
    files.append(*designConfigurations_);
    files.append(*generatorChains_);
    files.append(*typeDefinitions_);

    return files;
}
