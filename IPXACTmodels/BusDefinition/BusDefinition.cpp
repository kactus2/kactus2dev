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
BusDefinition::BusDefinition(VLNV const& vlnv, Document::Revision revision):
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
systemGroupNames_(other.systemGroupNames_),
extends_(other.extends_)
{
	Utilities::copyList(choices_, other.choices_);
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
		systemGroupNames_ = other.systemGroupNames_;
		extends_ = other.extends_;

        Utilities::copyList(choices_, other.choices_);
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
void BusDefinition::setMaxMasters(std::string const& maxMasters)
{
	setMaxInitiators(maxMasters);
}

//-----------------------------------------------------------------------------
// Function: BusDefinition::getMaxMasters()
//-----------------------------------------------------------------------------
std::string BusDefinition::getMaxMasters() const
{
	return getMaxInitiators();
}

//-----------------------------------------------------------------------------
// Function: BusDefinition::setMaxSlaves()
//-----------------------------------------------------------------------------
void BusDefinition::setMaxSlaves(std::string const& maxSlaves)
{
	setMaxTargets(maxSlaves);
}

//-----------------------------------------------------------------------------
// Function: BusDefinition::getMaxSlaves()
//-----------------------------------------------------------------------------
std::string BusDefinition::getMaxSlaves() const
{
	return getMaxTargets();
}

//-----------------------------------------------------------------------------
// Function: BusDefinition::setMaxInitiators()
//-----------------------------------------------------------------------------
void BusDefinition::setMaxInitiators(std::string const& maxInitiators)
{
	maxMasters_ = maxInitiators;
}

//-----------------------------------------------------------------------------
// Function: BusDefinition::getMaxInitiators()
//-----------------------------------------------------------------------------
std::string BusDefinition::getMaxInitiators() const
{
	return maxMasters_;
}

//-----------------------------------------------------------------------------
// Function: BusDefinition::setMaxTargets()
//-----------------------------------------------------------------------------
void BusDefinition::setMaxTargets(std::string const& maxTargets)
{
	maxSlaves_ = maxTargets;
}

//-----------------------------------------------------------------------------
// Function: BusDefinition::getMaxTargets()
//-----------------------------------------------------------------------------
std::string BusDefinition::getMaxTargets() const
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
