/* 
 *
 *  Created on: 30.7.2010
 *      Author: Antti Kamppi
 */

#include "remapstate.h"
#include "remapport.h"

#include <IPXACTmodels/XmlUtils.h>

#include <QList>
#include <QString>
#include <QDomNode>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
// Function: remapstate::RemapState()
//-----------------------------------------------------------------------------
RemapState::RemapState(QDomNode &remapNode):
nameGroup_(remapNode),
remapPorts_(new QList<QSharedPointer<RemapPort> >())
{
	for (int i = 0; i < remapNode.childNodes().count(); ++i) {

		QDomNode tempNode = remapNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:remapPorts")) {

			// go through all the remapPorts
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QDomNode remapPortNode = tempNode.childNodes().at(j);
				remapPorts_->append(QSharedPointer<RemapPort>(
						new RemapPort(remapPortNode)));
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Function: remapstate::RemapState()
//-----------------------------------------------------------------------------
RemapState::RemapState():
nameGroup_(),
remapPorts_(new QList<QSharedPointer<RemapPort> >())
{

}

//-----------------------------------------------------------------------------
// Function: remapstate::RemapState()
//-----------------------------------------------------------------------------
RemapState::RemapState( const RemapState &other ):
nameGroup_(other.nameGroup_),
remapPorts_(new QList<QSharedPointer<RemapPort> > ())
{
	foreach (QSharedPointer<RemapPort> remapPort, *other.remapPorts_) {
		if (remapPort) {
			QSharedPointer<RemapPort> copy = QSharedPointer<RemapPort>(
				new RemapPort(*remapPort.data()));
			remapPorts_->append(copy);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: remapstate::RemapState()
//-----------------------------------------------------------------------------
RemapState & RemapState::operator=( const RemapState &other )
{
	if (this != &other)
    {
        nameGroup_ = other.nameGroup_;

		foreach (QSharedPointer<RemapPort> remapPort, *other.remapPorts_)
        {
			if (remapPort)
            {
				QSharedPointer<RemapPort> copy = QSharedPointer<RemapPort>(
					new RemapPort(*remapPort.data()));
				remapPorts_->append(copy);
			}
		}
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: remapstate::~RemapState()
//-----------------------------------------------------------------------------
RemapState::~RemapState()
{
	remapPorts_->clear();
}

//-----------------------------------------------------------------------------
// Function: remapstate::write()
//-----------------------------------------------------------------------------
void RemapState::write(QXmlStreamWriter& writer)
{
	writer.writeStartElement("spirit:remapState");

	nameGroup_.write(writer);

	if (remapPorts_->size() != 0)
    {
		writer.writeStartElement("spirit:remapPorts");

		for (int i = 0; i < remapPorts_->size(); ++i)
        {
			remapPorts_->at(i)->write(writer);
		}
		writer.writeEndElement(); // spirit:remapPorts
	}

	writer.writeEndElement(); // spirit:remapState
	return;
}

//-----------------------------------------------------------------------------
// Function: remapstate::isValid()
//-----------------------------------------------------------------------------
bool RemapState::isValid( const QStringList& portNames,
						 QStringList& errorList,
						 const QString& parentIdentifier ) const
{
	bool valid = true;

    if (nameGroup_.name().isEmpty())
    {
		errorList.append(QObject::tr("No name specified for a remap state within %1").arg(parentIdentifier));
		valid = false;
	}

	foreach (QSharedPointer<RemapPort> remapPort, *remapPorts_)
    {
		if (!remapPort->isValid(portNames, errorList, QObject::tr("remap port %1").arg(nameGroup_.name())))
        {
				valid = false;
		}
	}

	return valid;
}

//-----------------------------------------------------------------------------
// Function: remapstate::isValid()
//-----------------------------------------------------------------------------
bool RemapState::isValid( const QStringList& portNames ) const
{
    if (nameGroup_.name().isEmpty())
    {
		return false;
	}

	foreach (QSharedPointer<RemapPort> remapPort, *remapPorts_)
    {
		if (!remapPort->isValid(portNames))
        {
            return false;
		}
	}
	return true;
}

//-----------------------------------------------------------------------------
// Function: remapstate::getRemapPorts()
//-----------------------------------------------------------------------------
const QSharedPointer<QList<QSharedPointer<RemapPort> > > RemapState::getRemapPorts()
{
	return remapPorts_;
}

//-----------------------------------------------------------------------------
// Function: remapstate::setName()
//-----------------------------------------------------------------------------
void RemapState::setName(const QString &name)
{
    nameGroup_.setName(name);
}

//-----------------------------------------------------------------------------
// Function: remapstate::getName()
//-----------------------------------------------------------------------------
QString RemapState::getName() const
{
    return nameGroup_.name();
}

//-----------------------------------------------------------------------------
// Function: remapstate::getDisplayName()
//-----------------------------------------------------------------------------
QString RemapState::getDisplayName() const
{
    return nameGroup_.displayName();
}

//-----------------------------------------------------------------------------
// Function: remapstate::setDisplayName()
//-----------------------------------------------------------------------------
void RemapState::setDisplayName(QString const& newDisplayName)
{
    nameGroup_.setDisplayName(newDisplayName);
}

//-----------------------------------------------------------------------------
// Function: remapstate::getDescription()
//-----------------------------------------------------------------------------
QString RemapState::getDescription() const
{
    return nameGroup_.description();
}

//-----------------------------------------------------------------------------
// Function: remapstate::setDescription()
//-----------------------------------------------------------------------------
void RemapState::setDescription(QString const& newDescription)
{
    nameGroup_.setDescription(newDescription);
}

//-----------------------------------------------------------------------------
// Function: remapstate::getNamegroup()
//-----------------------------------------------------------------------------
const NameGroup& RemapState::getNamegroup() const
{
    return nameGroup_;
}

//-----------------------------------------------------------------------------
// Function: remapstate::getNamegroup()
//-----------------------------------------------------------------------------
NameGroup& RemapState::getNamegroup()
{
    return nameGroup_;
}