/* 
 *
 *  Created on: 9.8.2010
 *      Author: Antti Kamppi
 */

#include "componentgenerator.h"
#include "generaldeclarations.h"
#include "XmlUtils.h"

#include <QList>
#include <QString>
#include <QDomNode>
#include <QDomNamedNodeMap>
#include <QXmlStreamWriter>

// the constructor
ComponentGenerator::ComponentGenerator(QDomNode &generatorNode):
Generator(generatorNode),
scope_(ComponentGenerator::INSTANCE),
groups_() {

	QDomNamedNodeMap attributeMap = generatorNode.attributes();

	// get the spirit scope attribute
	QString scope = attributeMap.namedItem(QString("spirit:scope")).nodeValue();
	if (scope == QString("entity")) {
		scope_ = ComponentGenerator::ENTITY;
	}
	else {
		scope_ = ComponentGenerator::INSTANCE;
	}

	// go through all the child nodes of the component generator
	for (int i = 0; i < generatorNode.childNodes().count(); ++i) {
		QDomNode tempNode = generatorNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:group")) {
			QString groupName = tempNode.childNodes().at(0).nodeValue();
			groupName = XmlUtils::removeWhiteSpace(groupName);
			groups_.append(groupName);
		}
	}
}

ComponentGenerator::ComponentGenerator( const ComponentGenerator &other ):
Generator(other),
scope_(other.scope_),
groups_(other.groups_) {
}

ComponentGenerator& ComponentGenerator::operator=( const ComponentGenerator &other ) {
	if (this != &other) {
		Generator::operator=(other);
		scope_ = other.scope_;
		groups_ = other.groups_;
	}
	return *this;
}

// the destructor
ComponentGenerator::~ComponentGenerator() {
}

void ComponentGenerator::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:componentGenerator");
	writer.writeAttribute("spirit:hidden",
			General::bool2Str(Generator::hidden_));

	// write the scope-attribute
	switch (scope_) {
	case ComponentGenerator::ENTITY: {
		writer.writeAttribute("spirit:scope", "entity");
		break;
	}
	// ComponentGenerator::INSTANCE
	default: {
		writer.writeAttribute("spirit:scope", "instance");
		break;
	}
	}

	writer.writeTextElement("spirit:name", name_);

	if (!displayName_.isEmpty()) {
		writer.writeTextElement("spirit:displayName", displayName_);
	}

	if (!description_.isEmpty()) {
		writer.writeTextElement("spirit:description", description_);
	}

	if (phase_ >= 0) {
		writer.writeTextElement("spirit:phase", QString::number(phase_));
	}

	// if any parameters exist
	if (parameters_.size() != 0) {
		writer.writeStartElement("spirit:parameters");

		// write each parameter
		for (int i = 0; i < parameters_.size(); ++i) {
			parameters_.at(i)->write(writer);
		}

		writer.writeEndElement(); // spirit:parameters
	}

	// write the spirit:apiType element
	switch (apiType_) {
	case Generator::TGI: {
		writer.writeTextElement("spirit:apiType", "TGI");
		break;
	}
	// Generator::NONE
	default: {
		writer.writeTextElement("spirit:apiType", "none");
	}
	}

	writer.writeTextElement("spirit:generatorExe", generatorExe_);

	// write all groups
	for (int i = 0; i < groups_.size(); ++i) {
		writer.writeTextElement("spirit:group", groups_.at(i));
	}

	writer.writeEndElement(); // spirit:componentGenerator
}

bool ComponentGenerator::isValid( QStringList& errorList, const QString& parentIdentifier ) const {
	bool valid = true;

	if (name_.isEmpty()) {
		errorList.append(QObject::tr("No name specified for component generator"
			" within %1").arg(parentIdentifier));
		valid = false;
	}

	foreach (QSharedPointer<Parameter> param, parameters_) {
		if (!param->isValid(errorList, QObject::tr("component generator %1").arg(name_))) {
			valid = false;
		}
	}

	if (generatorExe_.isEmpty()) {
		errorList.append(QObject::tr("No path to the generator executable specified"
			" for component generator %1 within %2").arg(name_).arg(parentIdentifier));
		valid = false;
	}

	return valid;
}

bool ComponentGenerator::isValid() const {
	if (name_.isEmpty()) {
		return false;
	}

	foreach (QSharedPointer<Parameter> param, parameters_) {
		if (!param->isValid()) {
			return false;
		}
	}

	if (generatorExe_.isEmpty()) {
		return false;
	}
	return true;
}

ComponentGenerator::Instance ComponentGenerator::getScope() const {
	return scope_;
}

const QList<QString>& ComponentGenerator::getGroups() {
	return groups_;
}

void ComponentGenerator::setGroups(QList<QString> &groups) {
	// delete old groups
	groups_.clear();

	// save the new groups
	groups_ = groups;
}

void ComponentGenerator::setScope(Instance scope) {
	scope_ = scope;
}
