/* 
 *
 *  Created on: 5.8.2010
 *      Author: Antti Kamppi
 */

#include "wire.h"
#include "generaldeclarations.h"
#include "vector.h"

#include "../exceptions/parse_error.h"
#include "../exceptions/write_error.h"

#include <QString>
#include <QList>
#include <QDomNode>
#include <QObject>
#include <QDomNamedNodeMap>
#include <QSharedPointer>
#include <QXmlStreamWriter>

#include <QDebug>

Wire::WireTypeDef::WireTypeDef(QDomNode &wireTypeNode): typeName_(QString()),
constrained_(false), typeDefinitions_(), viewNameRefs_() {

	for (int i = 0; i < wireTypeNode.childNodes().count(); ++i) {
		QDomNode tempNode = wireTypeNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:typeName")) {
			Wire::WireTypeDef::typeName_ = tempNode.childNodes().at(0).
					nodeValue();

			// get the constrained attribute
			QDomNamedNodeMap attributeMap = tempNode.attributes();
			QString constrained = attributeMap.namedItem(
					QString("spirit:constrained")).nodeValue();
			Wire::WireTypeDef::constrained_ =
					General::str2Bool(constrained, false);
		}

		else if (tempNode.nodeName() == QString("spirit:typeDefinition")) {
			Wire::WireTypeDef::typeDefinitions_.append(
					tempNode.childNodes().at(0).nodeValue());
		}

		else if (tempNode.nodeName() == QString("spirit:viewNameRef")) {
			Wire::WireTypeDef::viewNameRefs_.append(
					tempNode.childNodes().at(0).nodeValue());
		}
	}

	// if mandatory elements are missing
	if (Wire::WireTypeDef::typeName_.isNull()) {
		throw Parse_error(QObject::tr("Mandatory element spirit:typeName missing"
				" in spirit:wireTypeDef"));
	}

	if (Wire::WireTypeDef::viewNameRefs_.size() < 1) {
		throw Parse_error(QObject::tr("Mandatory element spirit:viewNameRef "
				"missing in spirit:wireTypeDef"));
	}
}

Wire::WireTypeDef::WireTypeDef( const QString& typeName /*= QString()*/, 
							   const QString& viewNameRef /*= QString("")*/ ):
typeName_(typeName),
constrained_(false),
typeDefinitions_(),
viewNameRefs_() {

	viewNameRefs_.append(viewNameRef);
}

bool Wire::WireTypeDef::hasView( const QString& viewName ) {
	foreach (QString viewRef, viewNameRefs_) {
		if (viewRef == viewName)
			return true;

	}
	return false;
}

Wire::Wire(QDomNode &wireNode): direction_(General::DIRECTION_INVALID),
		allLogicalDirectionsAllowed_(false), vector_(), wireTypeDefs_(),
		defaultDriverValue_(), defaultValueAttributes_() {

	// get attribute spirit:allLogicalDirectionsAllowed
	QDomNamedNodeMap attributeMap = wireNode.attributes();
	QString allLogicalDirectionsAllowed = attributeMap.namedItem(
			QString("spirit:allLogicalDirectionsAllowed")).nodeValue();
	allLogicalDirectionsAllowed_ =
			General::str2Bool(allLogicalDirectionsAllowed, false);

	// go through the child nodes of spirit:wire
	for (int i = 0; i < wireNode.childNodes().count(); ++i) {
		QDomNode tempNode = wireNode.childNodes().at(i);

		if (tempNode.nodeName() ==	QString("spirit:direction")) {
			direction_ = General::str2Direction(
				General::removeWhiteSpace(tempNode.childNodes().at(0).
					nodeValue()),
					General::DIRECTION_INVALID);
		}

		// get spirit:vector node and parse left and right values
		else if (tempNode.nodeName() ==	QString("spirit:vector")) {
			vector_ = QSharedPointer<Vector>(
					new Vector(tempNode));
		}

		// get spirit:wireTypeDefs
		else if (tempNode.nodeName() ==	QString("spirit:wireTypeDefs")) {
			for (int j = 0;	j < tempNode.childNodes().count(); ++j) {

				QDomNode wireTypeNode =	tempNode.childNodes().at(j);
				wireTypeDefs_.append(QSharedPointer<WireTypeDef>(
						new WireTypeDef(wireTypeNode)));
			}
		}

		// get spirit:driver
		else if (tempNode.nodeName() == QString("spirit:driver")) {
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QDomNode driverNode = tempNode.childNodes().at(j);
				if (driverNode.nodeName() == QString("spirit:defaultValue")) {
					defaultDriverValue_ =
							driverNode.childNodes().at(0).nodeValue();

					General::parseAttributes(driverNode,
							defaultValueAttributes_);
				}
			}
		}
	}

	// if mandatory element spirit:direction was not successfully defined
	if (direction_ == General::DIRECTION_INVALID) {
		throw Parse_error(QObject::tr("Mandatory element direction invalid in"
			" spirit:wire"));
	}

	return;
}

//-----------------------------------------------------------------------------
// Function: Wire()
//-----------------------------------------------------------------------------
Wire::Wire(Wire const& other) : 
direction_(other.direction_),
allLogicalDirectionsAllowed_(other.allLogicalDirectionsAllowed_),
vector_(),
wireTypeDefs_(), 
defaultDriverValue_(other.defaultDriverValue_),
defaultValueAttributes_(other.defaultValueAttributes_) {
	
	if (other.vector_)	{
		vector_ = QSharedPointer<Vector>(new Vector(*other.vector_));
	}

	foreach (QSharedPointer<WireTypeDef> typeDef, other.wireTypeDefs_) {
		if (typeDef) {
			QSharedPointer<WireTypeDef> copy = QSharedPointer<WireTypeDef>(
				new WireTypeDef(*typeDef.data()));
			wireTypeDefs_.append(copy);
		}
	}
}


Wire & Wire::operator=( const Wire &other ) {
	if (this != &other) {
		direction_ = other.direction_;
		allLogicalDirectionsAllowed_ = other.allLogicalDirectionsAllowed_;
		defaultDriverValue_ = other.defaultDriverValue_;
		defaultValueAttributes_ = other.defaultValueAttributes_;

		if (other.vector_)	{
			vector_ = QSharedPointer<Vector>(new Vector(*other.vector_));
		}
		else
			vector_ = QSharedPointer<Vector>();

		wireTypeDefs_.clear();
		foreach (QSharedPointer<WireTypeDef> typeDef, other.wireTypeDefs_) {
			if (typeDef) {
				QSharedPointer<WireTypeDef> copy = QSharedPointer<WireTypeDef>(
					new WireTypeDef(*typeDef.data()));
				wireTypeDefs_.append(copy);
			}
		}
	}
	return *this;
}


Wire::Wire(): direction_(General::DIRECTION_INVALID),
allLogicalDirectionsAllowed_(false), vector_(), wireTypeDefs_(),
defaultDriverValue_(), defaultValueAttributes_() {

}

Wire::Wire( General::Direction direction, 
		   int leftBound, 
		   int rightBound, 
		   const QString& defaultValue, 
		   bool allLogicalDirections ):
direction_(direction),
allLogicalDirectionsAllowed_(allLogicalDirections), 
vector_(), 
wireTypeDefs_(),
defaultDriverValue_(defaultValue), 
defaultValueAttributes_() {

	vector_ = QSharedPointer<Vector>(new Vector(leftBound, rightBound));
}

// the destructor
Wire::~Wire() {
	wireTypeDefs_.clear();
}

void Wire::write(QXmlStreamWriter& writer) {

	writer.writeStartElement("spirit:wire");
	writer.writeAttribute("spirit:allLogicalDirectionsAllowed",
			General::bool2Str(allLogicalDirectionsAllowed_));

	if (direction_ == General::DIRECTION_INVALID) {
		throw Write_error(QObject::tr("Mandatory element directions invalid in"
				" spirit:wire"));
	}
	else {
		writer.writeTextElement("spirit:direction",
				General::direction2Str(direction_));
	}

	// if optional element spirit:vector is defined
	if (vector_) {
		vector_->write(writer);
	}

	if (wireTypeDefs_.size() != 0) {
		writer.writeStartElement("spirit:wireTypeDefs");

		// go through each wire type def
		for (int i = 0; i < wireTypeDefs_.size(); ++i) {
			writer.writeStartElement("spirit:wireTypeDef");

			// write typeName is its found
			if (wireTypeDefs_.at(i)->typeName_.isEmpty()) {
				throw Write_error(QObject::tr("Mandatory typeName missing in"
						" spirit:wireTypeDef"));
			}
			else {
				// start the spirit:typeName tag
				writer.writeStartElement("spirit:typeName");

				// write the attribute for type name
				writer.writeAttribute("spirit:constrained",
						General::bool2Str(wireTypeDefs_.at(i)->constrained_));

				// write the value of the element and close the tag
				writer.writeCharacters(wireTypeDefs_.at(i)->typeName_);
				writer.writeEndElement(); // spirit:typeName

				qDebug() << "wrote wireTypeDef" <<
						wireTypeDefs_.at(i)->typeName_;
			}

			// write all type definitions
			for (int j = 0; j < wireTypeDefs_.at(i)->typeDefinitions_.size();
					++j) {
				writer.writeTextElement("spirit:typeDefinition",
						wireTypeDefs_.at(i)->typeDefinitions_.at(j));
			}

			// if no viewNameRefs are defined
			if (wireTypeDefs_.at(i)->viewNameRefs_.size() == 0) {
				throw Write_error(QObject::tr("Mandatory element viewNameRef "
						"missing in spirit:wireTypeDef"));
			}

			// write all viewNameRefs
			for (int j = 0; j < wireTypeDefs_.at(i)->viewNameRefs_.size();
					++j) {
				writer.writeTextElement("spirit:viewNameRef",
						wireTypeDefs_.at(i)->viewNameRefs_.at(j));
			}

			writer.writeEndElement(); // spirit:wireTypeDef
		}

		writer.writeEndElement(); // spirit:wireTypeDefs
	}

	// if optional defaultDriverValue exists
	if (!defaultDriverValue_.isEmpty()) {

		// the default value is inside spirit:driver in IP-Xact
		writer.writeStartElement("spirit:driver");

		// start the spirit:defaultValue tag
		writer.writeStartElement("spirit:defaultValue");

		// write the attributes for the element
		General::writeAttributes(writer, defaultValueAttributes_);

		// write the value of the element and close the tag
		writer.writeCharacters(defaultDriverValue_);
		writer.writeEndElement(); // spirit:defaultValue

		writer.writeEndElement(); // spirit:driver
	}

	writer.writeEndElement(); // spirit:wire
}

General::Direction Wire::getDirection() const {
	return direction_;
}

const QList<QSharedPointer<Wire::WireTypeDef> >& Wire::getWireTypeDefs() {
	return wireTypeDefs_;
}

Vector* Wire::getVector() const {

	if (vector_)
		return vector_.data();

	return 0;
}

void Wire::setVector(Vector* vector) {
	// delete old vector element
	if (vector_) {
		vector_.clear();
	}
	vector_ = QSharedPointer<Vector>(vector);
}

void Wire::setDirection(General::Direction direction) {
	direction_ = direction;
}

bool Wire::getAllLogicalDirectionsAllowed() const {
	return allLogicalDirectionsAllowed_;
}

void Wire::setWireTypeDefs(const QList<QSharedPointer<WireTypeDef> > &wireTypeDefs) {
	// delete old wireTypeDef elements
	wireTypeDefs_.clear();

	// save the new wireTypeDefs
	wireTypeDefs_ = wireTypeDefs;
}

void Wire::setAllLogicalDirectionsAllowed(bool allLogicalDirectionsAllowed) {
	allLogicalDirectionsAllowed_ = allLogicalDirectionsAllowed;
}

void Wire::setDefaultDriverValue(const QString& defaultDriverValue) {
	defaultDriverValue_ = defaultDriverValue;
}

QString Wire::getDefaultDriverValue() const {
	return defaultDriverValue_;
}

bool Wire::isValid() const {
	
	// if direction is not specified
	if (direction_ == General::DIRECTION_INVALID)
		return false;

	// if vector exists but is not valid
	if (vector_ && !vector_->isValid())
		return false;

	// check all wireTypeDefs
	for (int i = 0; i < wireTypeDefs_.size(); ++i) {

		// if typeName or viewNameRef is not specified for wireTypeDef
		if (wireTypeDefs_.value(i)->typeName_.isEmpty() ||
			wireTypeDefs_.value(i)->viewNameRefs_.isEmpty())
			return false;
	}

	// everything ok
	return true;
}

void Wire::setLeftBound( int leftBound ) {
	if (vector_)
		vector_->setLeft(leftBound);

	// if vector is not specified then create a new vector
	else {
		vector_ = QSharedPointer<Vector>(new Vector(leftBound, 0));
	}
}

void Wire::setRightBound( int rightBound ) {

	if (vector_)
		vector_->setRight(rightBound);

	// if vector is not specified then create a new vector
	else {
		vector_ = QSharedPointer<Vector>(new Vector(0, rightBound));
	}
}

QString Wire::getTypeName( const QString& viewName /*= QString("")*/ ) const {
	
	foreach (QSharedPointer<Wire::WireTypeDef> wtypeDef, wireTypeDefs_) {
		if (wtypeDef->hasView(viewName)) {
			return wtypeDef->typeName_;
		}
	}
}

void Wire::setTypeName( const QString& typeName, const QString& viewName /*= QString("")*/ ) {
	
	bool added = false;
	for (QList<QSharedPointer<WireTypeDef> >::iterator i = wireTypeDefs_.begin();
		i != wireTypeDefs_.end(); ++i) {
			if ((*i)->hasView(viewName)) {
				(*i)->typeName_ = typeName;
				added = true;
			}
	}

	// if there were no matches then create new one
	if (!added) {
		QSharedPointer<Wire::WireTypeDef> wtypedef(new Wire::WireTypeDef(typeName, viewName));
		wireTypeDefs_.append(wtypedef);
	}
}

QString Wire::getTypeDefinition( const QString& typeName ) {
	foreach (QSharedPointer<Wire::WireTypeDef> wtypeDef, wireTypeDefs_) {
		// if the type def is for the type name that is wanted
		if (wtypeDef->typeName_ == typeName) {

			// return the first type definition, if there are none then return 
			// empty string
			return wtypeDef->typeDefinitions_.value(0, QString(""));
		}
	}

	// if none was found
	return QString("");
}

void Wire::setTypeDefinition( const QString& typeName, const QString& typeDefinition ) {
	bool added = false;
	for (QList<QSharedPointer<WireTypeDef> >::iterator i = wireTypeDefs_.begin();
		i != wireTypeDefs_.end(); ++i) {
		
			if ((*i)->typeName_ == typeName) {
				(*i)->typeDefinitions_.clear();
				(*i)->typeDefinitions_.insert(0, typeDefinition);
				added = true;
			}
	}

	// if there were no matches then create new one
	if (!added) {
		QSharedPointer<Wire::WireTypeDef> wtypedef(new Wire::WireTypeDef(typeName));
		wtypedef->typeDefinitions_.append(typeDefinition);
		wireTypeDefs_.append(wtypedef);
	}
}
