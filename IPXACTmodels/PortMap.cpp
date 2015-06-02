//-----------------------------------------------------------------------------
// File: PortMap.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.08.2014
//
// Description:
// Describes the mapping between the logical and physical ports.
//-----------------------------------------------------------------------------

#include "PortMap.h"

#include "vector.h"
#include "port.h"
#include "XmlUtils.h"

//-----------------------------------------------------------------------------
// Function: PortMap::PortMap()
//-----------------------------------------------------------------------------
PortMap::PortMap(): 
logicalPort_(),
physicalPort_(),
logicalVector_(QSharedPointer<Vector>(new Vector())), 
physicalVector_(QSharedPointer<Vector>(new Vector()))
{

}

//-----------------------------------------------------------------------------
// Function: PortMap::PortMap()
//-----------------------------------------------------------------------------
PortMap::PortMap(QDomNode &portMap): 
logicalPort_(),
physicalPort_(),
logicalVector_(),
physicalVector_()
{
	for (int i = 0; i < portMap.childNodes().count(); ++i) {

		QDomNode tempNode = portMap.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:logicalPort")) {

			// search childNodes for name and vector elements
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {

				if (tempNode.childNodes().at(j).nodeName() ==
						QString("spirit:name")) {

					// get the logical name and strip whitespace characters
					logicalPort_= tempNode.childNodes().at(j).childNodes().
							at(0).nodeValue();
					logicalPort_ = XmlUtils::removeWhiteSpace(logicalPort_);
				}

				else if (tempNode.childNodes().at(j).nodeName() ==
						QString("spirit:vector")) {
					QDomNode vectorNode = tempNode.childNodes().at(j);

					logicalVector_ = QSharedPointer<Vector>(
							new Vector(vectorNode));
				}
			}

		}
		else if (tempNode.nodeName() == QString("spirit:physicalPort")) {

			// search for a spirit:vector element
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {

				if (tempNode.childNodes().at(j).nodeName() ==
						QString("spirit:name")) {

					// get the physical name and strip the whitespace characters
					physicalPort_ = tempNode.childNodes().at(j).childNodes().
							at(0).nodeValue();
					physicalPort_ = XmlUtils::removeWhiteSpace(physicalPort_);
				}

				else if (tempNode.childNodes().at(j).nodeName() ==
						QString("spirit:vector")) {
					QDomNode vectorNode = tempNode.childNodes().at(j);

					physicalVector_ = QSharedPointer<Vector>(
							new Vector(vectorNode));
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Function: PortMap::PortMap()
//-----------------------------------------------------------------------------
PortMap::PortMap( const PortMap& other ): 
logicalPort_(other.logicalPort_),
physicalPort_(other.physicalPort_),
logicalVector_(),
physicalVector_() {

	if (other.logicalVector_)
		logicalVector_ = QSharedPointer<Vector>(new Vector(*other.logicalVector_));

	if (other.physicalVector_)
		physicalVector_ = QSharedPointer<Vector>(new Vector(*other.physicalVector_));
}

//-----------------------------------------------------------------------------
// Function: PortMap::~PortMap()
//-----------------------------------------------------------------------------
PortMap::~PortMap()
{

}

//-----------------------------------------------------------------------------
// Function: PortMap::operator=()
//-----------------------------------------------------------------------------
PortMap& PortMap::operator=( const PortMap& other )
{
	if (this != &other) {
		logicalPort_ = other.logicalPort_;
		if (other.logicalVector_)
			logicalVector_ = QSharedPointer<Vector>(new Vector(*other.logicalVector_.data()));
		else
			logicalVector_ = QSharedPointer<Vector>();

		physicalPort_ = other.physicalPort_;
		if (other.physicalVector_)
			physicalVector_ = QSharedPointer<Vector>(new Vector(*other.physicalVector_.data()));
		else
			physicalVector_ = QSharedPointer<Vector>();
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: PortMap::isValid()
//-----------------------------------------------------------------------------
bool PortMap::isValid( const QList<General::PortBounds>& physicalPorts, 
							   QStringList& errorList, 
							   const QString& parentIdentifier ) const
{
	bool valid = true;

	if (physicalPort_.isEmpty()) {
		errorList.append(QObject::tr("No physical port specified for port map within %1").arg(
			parentIdentifier));
		valid = false;
	}

	if (logicalPort_.isEmpty()) {
		errorList.append(QObject::tr("No logical port specified for port map within %1").arg(
			parentIdentifier));
		valid = false;
	}

	int physSize = 1;
	int logSize = 1;

	if (physicalVector_) {
		if (!physicalVector_->isValid(errorList, 
			QObject::tr("port map within %1").arg(parentIdentifier))) {
				valid = false;
		}

		physSize = physicalVector_->getSize();
	}
	if (logicalVector_) {
		if (!logicalVector_->isValid(errorList,
			QObject::tr("port map within %1").arg(parentIdentifier))) {
				valid = false;
		}

		logSize = logicalVector_->getSize();
	}

	// if the sizes of the ports don't match
	if (physSize != logSize) {
		errorList.append(QObject::tr("The sizes of the vectors don't match in"
			" port map within %1").arg(parentIdentifier));
		valid = false;
	}

	// if there is a physical port specified
	if (!physicalPort_.isEmpty()) {
		
		bool foundPhysPort = false;
		foreach (General::PortBounds port, physicalPorts) {

			// if the referenced physical port was found
			if (port.portName_ == physicalPort_) {
				foundPhysPort = true;

				// calculate the size of the actual physical port.
				int actualPortSize = port.left_ - port.right_ + 1;

				// if the actual port size is smaller than the referenced vector in 
				// the port map
				if (actualPortSize < physSize) {
					errorList.append(QObject::tr("The port map within %1 is larger"
						" than the actual size of the port %2.").arg(
						parentIdentifier).arg(physicalPort_));
					valid = false;
				}

				break;
			}
		}
		// if the referenced port was not found within the component
		if (!foundPhysPort) {
			errorList.append(QObject::tr("The port map within %1 contained reference to"
				" physical port %2 which is not found in the component.").arg(
				parentIdentifier).arg(physicalPort_));
			valid = false;
		}
	}

	return valid;
}

//-----------------------------------------------------------------------------
// Function: PortMap::isValid()
//-----------------------------------------------------------------------------
bool PortMap::isValid( const QList<General::PortBounds>& physicalPorts ) const
{
	if (physicalPort_.isEmpty()) {
		return false;
	}

	if (logicalPort_.isEmpty()) {
		return false;
	}

	int physSize = 1;
	int logSize = 1;

	if (physicalVector_) {
		if (!physicalVector_->isValid()) {
				return false;
		}

		physSize = physicalVector_->getSize();
	}
	if (logicalVector_) {
		if (!logicalVector_->isValid()) {
				return false;
		}

		logSize = logicalVector_->getSize();
	}

	// if the sizes of the ports don't match
	if (physSize != logSize) {
		return false;
	}

	// if there is a physical port specified
	if (!physicalPort_.isEmpty()) {

		bool foundPhysPort = false;
		foreach (General::PortBounds port, physicalPorts) {

			// if the referenced physical port was found
			if (port.portName_ == physicalPort_) {
				foundPhysPort = true;

				// calculate the size of the actual physical port.
				int actualPortSize = abs(port.left_ - port.right_) + 1;

				// if the actual port size is smaller than the referenced vector in 
				// the port map
				if (actualPortSize < physSize) {
					return false;
				}

				break;
			}
		}
		// if the referenced port was not found within the component
		if (!foundPhysPort) {
			return false;
		}
	}
	return true;
}

//-----------------------------------------------------------------------------
// Function: PortMap::write()
//-----------------------------------------------------------------------------
void PortMap::write(QXmlStreamWriter& writer) const
{
    writer.writeStartElement("spirit:portMap");

    writer.writeStartElement("spirit:logicalPort");
    writer.writeTextElement("spirit:name", logicalPort_);

    // if a logical vector is defined
    if (logicalVector_) {
        logicalVector_->write(writer);
    }
    writer.writeEndElement(); // spirit:logicalPort

    writer.writeStartElement("spirit:physicalPort");
    writer.writeTextElement("spirit:name", physicalPort_);

    // if a physical vector is defined
    if (physicalVector_) {
        physicalVector_->write(writer);
    }

    writer.writeEndElement(); // spirit:physicalPort

    writer.writeEndElement(); // spirit:portMap
}

//-----------------------------------------------------------------------------
// Function: PortMap::getPhysicalRange()
//-----------------------------------------------------------------------------
General::PortBounds PortMap::getPhysicalRange(QSharedPointer<Port> referencedPhysicalPort) const
{
    General::PortBounds physicalBounds(physicalPort_);
    // IP-XACT Mapping rule B.1.8.b) If vector subelement exists, the range shall be according 
    // to its left and right. If no vector subelement is present, the range is according to the
    // referenced wire model port.
    if (!physicalVector_.isNull())
    {
        physicalBounds.left_ = physicalVector_->getLeft();
        physicalBounds.right_ = physicalVector_->getRight();        
    }
    else if(!referencedPhysicalPort.isNull() &&
            referencedPhysicalPort->getPortType() == General::WIRE)
    {
        physicalBounds.left_ = referencedPhysicalPort->getLeftBound();
        physicalBounds.right_ = referencedPhysicalPort->getRightBound();
    }
    else
    {
        Q_ASSERT_X(false, "Resolving port map boundaries", "No vector element or wire port defined");        
    }

    return physicalBounds;
}

//-----------------------------------------------------------------------------
// Function: PortMap::getLogicalRange()
//-----------------------------------------------------------------------------
General::PortBounds PortMap::getLogicalRange(QSharedPointer<Port> referencedPhysicalPort) const
{
    General::PortBounds logicalBounds(logicalPort_);
    // IP-XACT Mapping rule B.1.8.c) If vector subelement exists, the range shall be according 
    // to its left and right. If no vector subelement is present, the range shall be taken as
    // [abs(physical.left – physical.right): 0].
    if (!logicalVector_.isNull())
    {
        logicalBounds.left_ = logicalVector_->getLeft();
        logicalBounds.right_ = logicalVector_->getRight();        
    }
    else
    {
        // Set logical boundaries based physical mapping.
        General::PortBounds physicalBounds = getPhysicalRange(referencedPhysicalPort);    

        // Select higher boundary according to physical mapping.           
        if (physicalBounds.left_ >= physicalBounds.right_)
        {
            logicalBounds.left_ = abs(physicalBounds.left_ - physicalBounds.right_);
            logicalBounds.right_ = 0;
        }
        else
        {
            logicalBounds.right_ = abs(physicalBounds.left_ - physicalBounds.right_);
            logicalBounds.left_ = 0;
        }
    }

    return logicalBounds;
}
//-----------------------------------------------------------------------------
// Function: PortMap::logicalPort()
//-----------------------------------------------------------------------------
QString PortMap::logicalPort() const
{
    return logicalPort_;
}

//-----------------------------------------------------------------------------
// Function: PortMap::setLogicalPort()
//-----------------------------------------------------------------------------
void PortMap::setLogicalPort(QString const& logicalPort)
{
    logicalPort_ = logicalPort;
}

//-----------------------------------------------------------------------------
// Function: PortMap::logicalVector()
//-----------------------------------------------------------------------------
QSharedPointer<Vector> PortMap::logicalVector() const
{
    return logicalVector_;
}

//-----------------------------------------------------------------------------
// Function: PortMap::getLogicalLeft()
//-----------------------------------------------------------------------------
int PortMap::getLogicalLeft() const
{
    if (logicalVector_)
    {
        return logicalVector_->getLeft();
    }
    return -1;
}

//-----------------------------------------------------------------------------
// Function: PortMap::setLogicalLeft()
//-----------------------------------------------------------------------------
void PortMap::setLogicalLeft(int left)
{
    if (!logicalVector_)
    {
        logicalVector_ = QSharedPointer<Vector>(new Vector());
    }

    logicalVector_->setLeft(left);
}

//-----------------------------------------------------------------------------
// Function: PortMap::getLogicalRight()
//-----------------------------------------------------------------------------
int PortMap::getLogicalRight() const
{
    if (logicalVector_)
    {
        return logicalVector_->getRight();
    }
    return -1;
}

//-----------------------------------------------------------------------------
// Function: PortMap::setLogicalRight()
//-----------------------------------------------------------------------------
void PortMap::setLogicalRight(int right)
{
    if (!logicalVector_)
    {
        logicalVector_ = QSharedPointer<Vector>(new Vector());
    }

    logicalVector_->setRight(right);
}

//-----------------------------------------------------------------------------
// Function: PortMap::physicalPort()
//-----------------------------------------------------------------------------
QString PortMap::physicalPort() const
{
    return physicalPort_;
}

//-----------------------------------------------------------------------------
// Function: PortMap::setPhysicalPort()
//-----------------------------------------------------------------------------
void PortMap::setPhysicalPort(QString const& physicalPort)
{
    physicalPort_ = physicalPort;
}

//-----------------------------------------------------------------------------
// Function: PortMap::physicalVector()
//-----------------------------------------------------------------------------
QSharedPointer<Vector> PortMap::physicalVector() const
{
    return physicalVector_;
}

//-----------------------------------------------------------------------------
// Function: PortMap::getPhysicalLeft()
//-----------------------------------------------------------------------------
int PortMap::getPhysicalLeft() const
{
    if (physicalVector_)
    {
        return physicalVector_->getLeft();
    }
    return -1;
}

//-----------------------------------------------------------------------------
// Function: PortMap::setPhysicalLeft()
//-----------------------------------------------------------------------------
void PortMap::setPhysicalLeft(int left)
{
    if (!physicalVector_)
    {
        physicalVector_ = QSharedPointer<Vector>(new Vector());
    }

    physicalVector_->setLeft(left);
}

//-----------------------------------------------------------------------------
// Function: PortMap::getPhysicalRight()
//-----------------------------------------------------------------------------
int PortMap::getPhysicalRight() const
{
    if (physicalVector_)
    {
        return physicalVector_->getRight();
    }
    return -1;
}

//-----------------------------------------------------------------------------
// Function: PortMap::setPhysicalRight()
//-----------------------------------------------------------------------------
void PortMap::setPhysicalRight(int right)
{
    if (!physicalVector_)
    {
        physicalVector_ = QSharedPointer<Vector>(new Vector());
    }

    physicalVector_->setRight(right);
}
