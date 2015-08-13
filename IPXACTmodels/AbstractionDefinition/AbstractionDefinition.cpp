//-----------------------------------------------------------------------------
// File: AbstractionDefinition.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.08.2015
//
// Description:
// Implementation for ipxact:busDefinition element.
//-----------------------------------------------------------------------------

#include "AbstractionDefinition.h"

#include "PortAbstraction.h"

#include <IPXACTmodels/XmlUtils.h>

#include <QDomDocument>
#include <QString>
#include <QDomNamedNodeMap>
#include <QDomNode>
#include <QDomNodeList>
#include <QObject>
#include <QSharedPointer>
#include <QStringList>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::AbstractionDefinition()
//-----------------------------------------------------------------------------
AbstractionDefinition::AbstractionDefinition(): 
Document(), 
    busType_(),
    extends_(),
    logicalPorts_(new QList<QSharedPointer<PortAbstraction> >())
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::AbstractionDefinition()
//-----------------------------------------------------------------------------
AbstractionDefinition::AbstractionDefinition(AbstractionDefinition const& other):
Document(other),
    busType_(other.busType_),
    extends_(other.extends_),
    logicalPorts_(new QList<QSharedPointer<PortAbstraction> >())
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::operator=()
//-----------------------------------------------------------------------------
AbstractionDefinition & AbstractionDefinition::operator=(AbstractionDefinition const& other)
{
	if (this != &other)
    {
		Document::operator=(other);
        busType_ = other.busType_;
		extends_ = other.extends_;

        //copy ports
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::~AbstractionDefinition()
//-----------------------------------------------------------------------------
AbstractionDefinition::~AbstractionDefinition()
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::clone()
//-----------------------------------------------------------------------------
QSharedPointer<Document> AbstractionDefinition::clone() const
{
	return QSharedPointer<Document>(new AbstractionDefinition(*this));
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::setVlnv()
//-----------------------------------------------------------------------------
void AbstractionDefinition::setVlnv(VLNV const& vlnv)
{
    VLNV abstractionDefinitionVLNV(vlnv);
    abstractionDefinitionVLNV.setType(VLNV::ABSTRACTIONDEFINITION);
    Document::setVlnv(abstractionDefinitionVLNV);
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::setBusType()
//-----------------------------------------------------------------------------
void AbstractionDefinition::setBusType(VLNV const& targetBus)
{
    busType_ = targetBus;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::setBusType()
//-----------------------------------------------------------------------------
VLNV AbstractionDefinition::getBusType() const
{
    return busType_;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::getExtends()
//-----------------------------------------------------------------------------
VLNV AbstractionDefinition::getExtends() const
{
    return extends_;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::setExtends()
//-----------------------------------------------------------------------------
void AbstractionDefinition::setExtends(VLNV const& extends)
{
    extends_ = extends;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::getDependentVLNVs()
//-----------------------------------------------------------------------------
QList<VLNV> AbstractionDefinition::getDependentVLNVs() const 
{
    QList<VLNV> vlnvList;

    if (busType_.isValid())
    {
        vlnvList.append(busType_);
    }

    if (extends_.isValid())
    {
        vlnvList.append(extends_);
    }

    return vlnvList;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::getDependentFiles()
//-----------------------------------------------------------------------------
QStringList AbstractionDefinition::getDependentFiles() const
{
    return QStringList();
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::getLogicalPorts()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<PortAbstraction> > > AbstractionDefinition::getLogicalPorts() const
{
    return logicalPorts_;
}
