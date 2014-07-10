/* 
 *
 *  Created on: 2.8.2010
 *      Author: Antti Kamppi
 */

#include "port.h"

#include "parameter.h"
#include "transactional.h"
#include "wire.h"
#include "GenericVendorExtension.h"
#include "XmlUtils.h"

#include <IPXACTmodels/kactusExtensions/Kactus2Placeholder.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Position.h>

#include <QDomNode>
#include <QString>
#include <QList>
#include <QDomNamedNodeMap>
#include <QObject>
#include <QSharedPointer>
#include <QXmlStreamWriter>

// the constructor
Port::Port(QDomNode &portNode): 
nameGroup_(portNode), 
portType_(General::WIRE),
wire_(),
transactional_(),
portAccessHandle_(), 
portAccessType_(),
adHocVisible_(),
defaultPos_(),
vendorExtensions_()
{
    createAdHocVisibleExtension();
    createPositionExtension();

	for (int i = 0; i < portNode.childNodes().count(); ++i) {
		QDomNode tempNode = portNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:wire")) 
        {
            portType_ = General::WIRE;
			wire_ = QSharedPointer<Wire>(new Wire(tempNode));
		}
		else if (tempNode.nodeName() == QString("spirit:transactional")) 
        {
            portType_ = General::TRANSACTIONAL;
			transactional_ = QSharedPointer<Transactional>(new Transactional(tempNode));
		}

		// get the portAccessHandle and portAccessType elements
		else if (tempNode.nodeName() == QString("spirit:access")) 
        {
			for (int j = 0; j < tempNode.childNodes().count(); ++j) 
            {
                QDomNode accessNode = tempNode.childNodes().at(j);
				if (accessNode.nodeName() == QString("spirit:portAccessHandle")) {
					portAccessHandle_ = accessNode.childNodes().at(0).nodeValue();
				}

				else if (accessNode.nodeName() == QString("spirit:portAccessType")) {
					portAccessType_ = accessNode.childNodes().at(0).nodeValue();
				}
			}
		}
        else if (tempNode.nodeName() == QString("spirit:vendorExtensions"))
        {
            parseVendorExtensions(tempNode);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Port()
//-----------------------------------------------------------------------------
Port::Port(QString const& name, Port const& other): 
nameGroup_(name), portType_(other.portType_),
wire_(), 
transactional_(other.transactional_),
portAccessHandle_(other.portAccessHandle_),
portAccessType_(other.portAccessType_),
adHocVisible_(),
defaultPos_(),
vendorExtensions_()
{	
    if (other.wire_ != 0) {
        wire_ = QSharedPointer<Wire>(new Wire(*other.wire_));
    }

    createAdHocVisibleExtension();
    createPositionExtension();
    copyVendorExtensions(other);
}

//-----------------------------------------------------------------------------
// Function: Port()
//-----------------------------------------------------------------------------
Port::Port(const Port &other): 
nameGroup_(other.nameGroup_), 
portType_(other.portType_),
wire_(),
transactional_(),
portAccessHandle_(other.portAccessHandle_),
portAccessType_(other.portAccessType_),
adHocVisible_(),
defaultPos_(),
vendorExtensions_()
{
	if (other.wire_) {
		wire_ = QSharedPointer<Wire>(new Wire(*other.wire_));
	}

	if (other.transactional_) {
		transactional_ = QSharedPointer<Transactional>(new Transactional(*other.transactional_.data()));
	}

    createAdHocVisibleExtension();
    createPositionExtension();
    copyVendorExtensions(other);
}

//-----------------------------------------------------------------------------
// Function: Port::operator=()
//-----------------------------------------------------------------------------
Port & Port::operator=( const Port &other ) {
	if (this != &other) {
		nameGroup_ = other.nameGroup_;
		portType_ = other.portType_;
		portAccessHandle_ = other.portAccessHandle_;
		portAccessType_ = other.portAccessType_;

		if (other.wire_) {
			wire_ = QSharedPointer<Wire>(new Wire(*other.wire_.data()));
		}
		else
			wire_ = QSharedPointer<Wire>();

		if (other.transactional_) 
        {
			transactional_ = QSharedPointer<Transactional>(new Transactional(*other.transactional_.data()));
		}
		else
        {
			transactional_ = QSharedPointer<Transactional>();
        }

        createAdHocVisibleExtension();
        createPositionExtension();
        copyVendorExtensions(other);
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: Port::Port()
//-----------------------------------------------------------------------------
Port::Port():
nameGroup_(), 
portType_(General::WIRE),
wire_(),
transactional_(),
portAccessHandle_(),
portAccessType_(),
adHocVisible_(),
defaultPos_(),
vendorExtensions_()
{
	wire_ = QSharedPointer<Wire>(new Wire());

    createAdHocVisibleExtension();
    createPositionExtension();
}

//-----------------------------------------------------------------------------
// Function: Port::Port()
//-----------------------------------------------------------------------------
Port::Port( const QString& name, 
		   General::Direction direction, 
		   int leftBound, 
		   int rightBound, 
		   const QString& defaultValue, 
		   bool allLogicalDirections ):
nameGroup_(name), 
portType_(General::WIRE),
wire_(), 
transactional_(),
portAccessHandle_(),
portAccessType_(),
adHocVisible_(),
defaultPos_(),
vendorExtensions_()
{
	wire_ = QSharedPointer<Wire>(new Wire(direction, leftBound, rightBound, defaultValue, allLogicalDirections));
    
    createAdHocVisibleExtension();
    createPositionExtension();
}

//-----------------------------------------------------------------------------
// Function: Port::Port()
//-----------------------------------------------------------------------------
Port::Port( const QString& name,
		   General::Direction direction, 
		   int leftBound,
		   int rightBound,
		   const QString& typeName, 
		   const QString& typeDefinition,
		   const QString& defaultValue,
		   const QString& description ):
nameGroup_(name, QString(), description),
portType_(General::WIRE),
wire_(),
transactional_(),
portAccessHandle_(),
portAccessType_(),
adHocVisible_(),
defaultPos_(),
vendorExtensions_()
{

	wire_ = QSharedPointer<Wire>(new Wire(direction, leftBound, rightBound, defaultValue, false));
	wire_->setTypeName(typeName);
	wire_->setTypeDefinition(typeName, typeDefinition);

    createAdHocVisibleExtension();
    createPositionExtension();
}

//-----------------------------------------------------------------------------
// Function: ~Port()
//-----------------------------------------------------------------------------
Port::~Port() 
{

}

//-----------------------------------------------------------------------------
// Function: Port::write()
//-----------------------------------------------------------------------------
void Port::write(QXmlStreamWriter& writer, const QStringList& viewNames)
{
	writer.writeStartElement("spirit:port");

    nameGroup_.write(writer);

	// write the port type element (spirit:wire or spirit:transactional)
	switch (portType_) {
	case General::WIRE: {
		if (wire_) {
			wire_->write(writer, viewNames);
		}
		break;
	}
	case General::TRANSACTIONAL: {
		if (transactional_) {
			transactional_->write(writer);
		}
		break;
	}
	default: {
        Q_ASSERT(false);
		break;
	}
	}

	// if either of the optional elements is defined
	if (!portAccessHandle_.isEmpty() || !portAccessType_.isEmpty()) {
		writer.writeStartElement("spirit:access");

		if (!portAccessType_.isEmpty()) {
			writer.writeTextElement("spirit:portAccessType", portAccessType_);
		}

		if (!portAccessHandle_.isEmpty()) {
			writer.writeTextElement("spirit:portAccessHandle", portAccessHandle_);
		}

		writer.writeEndElement(); // spirit:access
	}

    writer.writeStartElement("spirit:vendorExtensions");
    XmlUtils::writeVendorExtensions(writer, vendorExtensions_);
    writer.writeEndElement(); // spirit:vendorExtensions

	writer.writeEndElement(); // spirit:port
}

bool Port::isValid(bool hasViews) const {

	if (nameGroup_.name().isEmpty())
		return false;

	// if port is type wire but the element is not defined.
	if (portType_ == General::WIRE && !wire_)
		return false;

	// if port is type transactional but element is not defined.
	if (portType_ == General::TRANSACTIONAL && !transactional_)
		return false;

	if (wire_)
		return wire_->isValid(hasViews);

	return true;

}

bool Port::isValid( bool hasViews, 
				   QStringList& errorList, 
				   const QString& parentIdentifier ) const {

	bool valid = true;

	if (nameGroup_.name().isEmpty()) {
		errorList.append(QObject::tr("Port has no name within %1").arg(parentIdentifier));
		valid = false;
	}

	if (portType_ == General::WIRE && !wire_) {
		errorList.append(QObject::tr("Port is type wire but has no wire-element within %1").arg(
			parentIdentifier));
		valid = false;
	}

	if (portType_ == General::TRANSACTIONAL && !transactional_) {
		errorList.append(QObject::tr("Port is type transactional but has no "
			"transactional-element within %1").arg(parentIdentifier));
		valid = false;
	}

	if (wire_) {
		if (!wire_->isValid(hasViews, errorList, QObject::tr("Port %1").arg(nameGroup_.name()))) {
			valid = false;
		}
	}

	return valid;
}

void Port::setTransactional(Transactional *transactional) {	
	// delete the old wire if one exists
	if (wire_) {
		wire_.clear();
	}

    // delete the old transactional if one exists
	if (transactional_) {
		transactional_.clear();
	}

	// change the port type
    portType_ = General::TRANSACTIONAL;
	transactional_ = QSharedPointer<Transactional>(transactional);
}

Transactional *Port::getTransactional() const {
	return transactional_.data();
}

void Port::setName(const QString &name) {
	nameGroup_.setName(name);
}

QString Port::getName() const {
	return nameGroup_.name();
}

QString Port::getDisplayName() const {
	return nameGroup_.displayName();
}

void Port::setDisplayName( const QString& displayName ) {
	nameGroup_.setDisplayName(displayName);
}

QString Port::getDescription() const {
	return nameGroup_.description();
}

void Port::setDescription( const QString& description ) {
	nameGroup_.setDescription(description);
}

General::PortType Port::getPortType() const {
	return portType_;
}

Wire *Port::getWire() const {
	return wire_.data();
}

void Port::setWire(Wire *wire) {	
	// delete the old wire if one exists
	if (wire_) {
		wire_.clear();
	}

    // delete the old transactional if one exists
	if (transactional_) {
		transactional_.clear();
	}

	// change the port type
    portType_ = General::WIRE;
	wire_ = QSharedPointer<Wire>(wire);
}

QString Port::getPortAccessType() const {
	return portAccessType_;
}

void Port::setPortAccessType(const QString &portAccessType) {
	portAccessType_ = portAccessType;
}

int Port::getLeftBound() const {

	// if the port is not wire-type
	if (!wire_) {
		return 0;
	}

	// if the wire has a vector defined
	if (wire_->getVector()) {
		return wire_->getVector()->getLeft();
	}

	// no vector so bound is 0
	return 0;
}

int Port::getRightBound() const {

	// if the port is not wire-type
	if (!wire_) {
		return 0;
	}

	// if the wire has a vector defined
	if (wire_->getVector()) {
		return wire_->getVector()->getRight();
	}

	// no vector so bound is 0
	return 0;
}

int Port::getPortSize() const {
	return abs(getLeftBound() - getRightBound()) +1;
}

QString Port::getDefaultValue() const {
	if (!wire_) {
		return QString();
	}
	return wire_->getDefaultDriverValue();
}

void Port::setDefaultValue( const QString& defaultValue ) {

	if (wire_)
		wire_->setDefaultDriverValue(defaultValue);

	// transactional does not have default value
	else if (transactional_)
		return;

	else {
		wire_ = QSharedPointer<Wire>(new Wire());
		wire_->setDefaultDriverValue(defaultValue);
		portType_ = General::WIRE;
	}
}

General::Direction Port::getDirection() const {
	if (wire_) {
		return wire_->getDirection();
	}
	return General::DIRECTION_INVALID;
}

bool Port::allLogicalDirectionsAllowed() const {
	
	if (wire_)
		return wire_->getAllLogicalDirectionsAllowed();

	return false;
}

void Port::setDirection( General::Direction direction ) {

	// if wire has been specified
	if (wire_)
    {
		wire_->setDirection(direction);
    }
	// if neither is specified then create a new wire element
	else 
    {
		wire_ = QSharedPointer<Wire>(new Wire());
		portType_ = General::WIRE;
		wire_->setDirection(direction);
	}
}

void Port::setLeftBound( int leftBound ) {

	if (wire_)
		wire_->setLeftBound(leftBound);

	// if neither exists then create wire 
	else {
		wire_ = QSharedPointer<Wire>(new Wire());
		wire_->setLeftBound(leftBound);
		portType_ = General::WIRE;
	}
}

void Port::setRightBound( int rightBound ) {
	if (wire_)
		wire_->setRightBound(rightBound);

	// if neither exists then create wire 
	else {
		wire_ = QSharedPointer<Wire>(new Wire());
		wire_->setRightBound(rightBound);
		portType_ = General::WIRE;
	}
}

void Port::setPortSize( int size ) {
	setLeftBound(size-1);
	setRightBound(0);
}

void Port::setAllLogicalDirectionsAllowed( bool allowed ) {

	if (wire_)
    {
		wire_->setAllLogicalDirectionsAllowed(allowed);
    }
	else if (transactional_)
    {
		transactional_->setAllLogicalInitiativesAllowed(allowed);
    }
	else 
    {
		wire_ = QSharedPointer<Wire>(new Wire());
		wire_->setAllLogicalDirectionsAllowed(allowed);
		portType_ = General::WIRE;
	}
}

QString Port::getTypeName( const QString& viewName /*= QString("")*/ ) const {
	if (wire_) 
    {
		return wire_->getTypeName(viewName);
	}
	else 
    {
		return QString("");
	}
}

void Port::setTypeName( const QString& typeName, const QString& viewName /*= QString("")*/ ) {
	if (wire_) {
		wire_->setTypeName(typeName, viewName);
	}
	else {
		wire_ = QSharedPointer<Wire>(new Wire());
		portType_ = General::WIRE;
		wire_->setTypeName(typeName, viewName);
	}
}

QString Port::getTypeDefinition( const QString& typeName ) {
	if (wire_) {
		return wire_->getTypeDefinition(typeName);
	}
	else {
		return QString("");
	}
}

QStringList Port::getTypeDefinitions() const {
	if (wire_) {
		return wire_->getTypeDefinitions();
	}
	else {
		return QStringList();
	}
}

void Port::setTypeDefinition( const QString& typeName, const QString& typeDefinition ) {
	if (wire_) {
		wire_->setTypeDefinition(typeName, typeDefinition);
	}
	else {
		wire_ = QSharedPointer<Wire>(new Wire());
		portType_ = General::WIRE;
		wire_->setTypeDefinition(typeName, typeDefinition);
	}
}

bool Port::hasType( const QString& viewName /*= QString("")*/ ) const {
	if (wire_) {
		return wire_->hasType(viewName);
	}
	// if no wire then theres no type
	return false;
}

bool Port::hasTypeDefinitions() const {
	if (wire_) {
		return wire_->hasTypeDefinition();
	}
	// if wire don't exist then there is no type definitions
	else {
		return false;
	}
}

void Port::useDefaultVhdlTypes() {
	// if theres no wire definition
	if (!wire_) 
    {
		wire_ = QSharedPointer<Wire>(new Wire());
		portType_ = General::WIRE;
	}

	int size = getPortSize();
	QString typeName;

	// use std_logic for scalar ports
	if (size <= 1) {
		typeName = QString("std_logic");
	}
	// use std_logic_vector for vectored ports
	else {
		typeName = QString("std_logic_vector");
	}

	wire_->setTypeName(typeName);

	// set the default type definition
	wire_->setTypeDefinition(typeName, QString("IEEE.std_logic_1164.all"));
}

//-----------------------------------------------------------------------------
// Function: Port::setAdHocVisible()
//-----------------------------------------------------------------------------
void Port::setAdHocVisible(bool visible)
{
    if (visible)
    {
        if (!vendorExtensions_.contains(adHocVisible_))
        {
            vendorExtensions_.append(adHocVisible_);
        }
    }
    else
    {
        vendorExtensions_.removeAll(adHocVisible_);
    }
}

//-----------------------------------------------------------------------------
// Function: Port::isAdHocVisible()
//-----------------------------------------------------------------------------
bool Port::isAdHocVisible() const
{
    return vendorExtensions_.contains(adHocVisible_);
}

//-----------------------------------------------------------------------------
// Function: Port::setDefaultPos()
//-----------------------------------------------------------------------------
void Port::setDefaultPos(QPointF const& pos)
{
    defaultPos_->setPosition(pos);
    
    if (!vendorExtensions_.contains(defaultPos_))
    {
        vendorExtensions_.append(defaultPos_);
    }
}

//-----------------------------------------------------------------------------
// Function: Port::getDefaultPos()
//-----------------------------------------------------------------------------
QPointF Port::getDefaultPos() const
{
    return defaultPos_->position();
}

//-----------------------------------------------------------------------------
// Function: Port::parseVendorExtensions()
//-----------------------------------------------------------------------------
void Port::parseVendorExtensions(QDomNode const& extensionsNode)
{    
    int extensionCount = extensionsNode.childNodes().count();
    for (int i = 0; i < extensionCount; ++i) 
    {        
        QDomNode extensionNode = extensionsNode.childNodes().at(i);

        if (extensionNode.nodeName() == QString("kactus2:adHocVisible"))
        {          
            setAdHocVisible(true);
        }
        else if (extensionNode.nodeName() == "kactus2:position")
        {            
            QPointF position = XmlUtils::parsePoint(extensionNode);
            setDefaultPos(position);
        }
        else
        {                    
            vendorExtensions_.append(QSharedPointer<VendorExtension>(new GenericVendorExtension(extensionNode)));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Port::copyVendorExtensions()
//-----------------------------------------------------------------------------
void Port::copyVendorExtensions(Port const& other)
{
    foreach (QSharedPointer<VendorExtension> extension, other.vendorExtensions_) 
    {        
        if (extension->type() == "kactus2:adHocVisible")
        {
            setAdHocVisible(true);
        }
        else if (extension->type() == "kactus2:position")
        {
            setDefaultPos(other.defaultPos_->position());           
        }
        else
        {
            vendorExtensions_.append(QSharedPointer<VendorExtension>(extension->clone()));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Port::createAdHocVisibleExtension()
//-----------------------------------------------------------------------------
void Port::createAdHocVisibleExtension()
{
    adHocVisible_ = QSharedPointer<Kactus2Placeholder>(new Kactus2Placeholder("kactus2:adHocVisible"));
}

//-----------------------------------------------------------------------------
// Function: Port::createPositionExtension()
//-----------------------------------------------------------------------------
void Port::createPositionExtension()
{
    defaultPos_ = QSharedPointer<Kactus2Position>(new Kactus2Position(QPointF()));
}


