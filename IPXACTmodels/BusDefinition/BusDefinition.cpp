//-----------------------------------------------------------------------------
// File: BusDefinition.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 05.08.2015
//
// Description:
// Implementation for ipxact:busDefinition element.
//-----------------------------------------------------------------------------

#include "BusDefinition.h"
  
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
// Function: BusDefinition::BusDefinition()
//-----------------------------------------------------------------------------
BusDefinition::BusDefinition(): 
Document()
{

}

//-----------------------------------------------------------------------------
// Function: BusDefinition::BusDefinition()
//-----------------------------------------------------------------------------
BusDefinition::BusDefinition(VLNV vlnv, Document::Revision revision):
Document(revision)
{
	setVlnv(vlnv);
}

//-----------------------------------------------------------------------------
// Function: BusDefinition::BusDefinition()
//-----------------------------------------------------------------------------
BusDefinition::BusDefinition(BusDefinition const& other):
Document(other),
directConnection_(other.directConnection_),
broadcast_(other.broadcast_),
isAddressable_(other.isAddressable_),
maxMasters_(other.maxMasters_),
maxSlaves_(other.maxSlaves_),
choices_(other.choices_),
systemGroupNames_(other.systemGroupNames_),
extends_(other.extends_)
{

}

//-----------------------------------------------------------------------------
// Function: BusDefinition::operator=()
//-----------------------------------------------------------------------------
BusDefinition & BusDefinition::operator=(BusDefinition const& other)
{
	if (this != &other)
    {
		Document::operator=(other);
		directConnection_ = other.directConnection_;
        broadcast_ = other.broadcast_;
		isAddressable_ = other.isAddressable_;
		maxMasters_ = other.maxMasters_;
		maxSlaves_ = other.maxSlaves_;
		choices_ = other.choices_;
		systemGroupNames_ = other.systemGroupNames_;
		extends_ = other.extends_;
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: BusDefinition::~BusDefinition()
//-----------------------------------------------------------------------------
BusDefinition::~BusDefinition()
{

}

//-----------------------------------------------------------------------------
// Function: BusDefinition::clone()
//-----------------------------------------------------------------------------
QSharedPointer<Document> BusDefinition::clone() const
{
	return QSharedPointer<Document>(new BusDefinition(*this));
}

//-----------------------------------------------------------------------------
// Function: BusDefinition::setVlnv()
//-----------------------------------------------------------------------------
void BusDefinition::setVlnv(VLNV const& vlnv)
{
    VLNV busDefinitionVLNV(vlnv);
    busDefinitionVLNV.setType(VLNV::BUSDEFINITION);
    Document::setVlnv(busDefinitionVLNV);
}

//-----------------------------------------------------------------------------
// Function: BusDefinition::setDirectConnection()
//-----------------------------------------------------------------------------
void BusDefinition::setDirectConnection(bool directConnection)
{
    directConnection_ = directConnection;
}

//-----------------------------------------------------------------------------
// Function: BusDefinition::getDirectConnection()
//-----------------------------------------------------------------------------
bool BusDefinition::getDirectConnection() const
{
    return directConnection_;
}

//-----------------------------------------------------------------------------
// Function: BusDefinition::setBroadcast()
//-----------------------------------------------------------------------------
void BusDefinition::setBroadcast(bool broadcast)
{
    broadcast_.setValue(broadcast);
}

//-----------------------------------------------------------------------------
// Function: BusDefinition::getBroadcast()
//-----------------------------------------------------------------------------
BooleanValue BusDefinition::getBroadcast() const
{
    return broadcast_;
}

//-----------------------------------------------------------------------------
// Function: BusDefinition::setIsAddressable()
//-----------------------------------------------------------------------------
void BusDefinition::setIsAddressable(bool isAddressable)
{
    isAddressable_ = isAddressable;
}

//-----------------------------------------------------------------------------
// Function: BusDefinition::getIsAddressable()
//-----------------------------------------------------------------------------
bool BusDefinition::getIsAddressable() const
{
    return isAddressable_;
}

//-----------------------------------------------------------------------------
// Function: BusDefinition::getExtends()
//-----------------------------------------------------------------------------
VLNV BusDefinition::getExtends() const
{
    return extends_;
}

//-----------------------------------------------------------------------------
// Function: BusDefinition::setExtends()
//-----------------------------------------------------------------------------
void BusDefinition::setExtends(VLNV const& extends)
{
    extends_ = extends;
}

//-----------------------------------------------------------------------------
// Function: BusDefinition::setMaxMasters()
//-----------------------------------------------------------------------------
void BusDefinition::setMaxMasters(QString const& maxMasters)
{
	setMaxInitiators(maxMasters);
}

//-----------------------------------------------------------------------------
// Function: BusDefinition::getMaxMasters()
//-----------------------------------------------------------------------------
QString BusDefinition::getMaxMasters() const
{
	return getMaxInitiators();
}

//-----------------------------------------------------------------------------
// Function: BusDefinition::setMaxSlaves()
//-----------------------------------------------------------------------------
void BusDefinition::setMaxSlaves(QString const& maxSlaves)
{
	setMaxTargets(maxSlaves);
}

//-----------------------------------------------------------------------------
// Function: BusDefinition::getMaxSlaves()
//-----------------------------------------------------------------------------
QString BusDefinition::getMaxSlaves() const
{
	return getMaxTargets();
}

//-----------------------------------------------------------------------------
// Function: BusDefinition::setMaxInitiators()
//-----------------------------------------------------------------------------
void BusDefinition::setMaxInitiators(QString const& maxInitiators)
{
	maxMasters_ = maxInitiators;
}

//-----------------------------------------------------------------------------
// Function: BusDefinition::getMaxInitiators()
//-----------------------------------------------------------------------------
QString BusDefinition::getMaxInitiators() const
{
	return maxMasters_;
}

//-----------------------------------------------------------------------------
// Function: BusDefinition::setMaxTargets()
//-----------------------------------------------------------------------------
void BusDefinition::setMaxTargets(QString const& maxTargets)
{
	maxSlaves_ = maxTargets;
}

//-----------------------------------------------------------------------------
// Function: BusDefinition::getMaxTargets()
//-----------------------------------------------------------------------------
QString BusDefinition::getMaxTargets() const
{
	return maxSlaves_;
}

//-----------------------------------------------------------------------------
// Function: BusDefinition::setSystemGroupNames()
//-----------------------------------------------------------------------------
void BusDefinition::setSystemGroupNames(QStringList const& systemGroupNames)
{
	systemGroupNames_ = systemGroupNames;
}

//-----------------------------------------------------------------------------
// Function: BusDefinition::getSystemGroupNames()
//-----------------------------------------------------------------------------
QStringList BusDefinition::getSystemGroupNames() const
{
	return systemGroupNames_;
}

//-----------------------------------------------------------------------------
// Function: BusDefinition::setChoices()
//-----------------------------------------------------------------------------
void BusDefinition::setChoices(QSharedPointer<QList<QSharedPointer<Choice> > > choices)
{
	choices_ = choices;
}

//-----------------------------------------------------------------------------
// Function: BusDefinition::getChoices()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Choice> > > BusDefinition::getChoices() const
{
	return choices_;
}

//-----------------------------------------------------------------------------
// Function: BusDefinition::getDependentVLNVs()
//-----------------------------------------------------------------------------
QList<VLNV> BusDefinition::getDependentVLNVs() const 
{
	QList<VLNV> vlnvList;
	if (extends_.isValid())
    {
		vlnvList.append(extends_);
	}

	return vlnvList;
}

//-----------------------------------------------------------------------------
// Function: BusDefinition::getDependentFiles()
//-----------------------------------------------------------------------------
QStringList BusDefinition::getDependentFiles() const
{
    return QStringList();
}
