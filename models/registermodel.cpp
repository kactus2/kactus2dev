/* 
 *
 *  Created on: 26.10.2010
 *      Author: Antti Kamppi
 */

#include "registermodel.h"
#include "generaldeclarations.h"
#include "parameter.h"


#include "../exceptions/write_error.h"

#include <QList>
#include <QDomNode>
#include <QString>
#include <QSharedPointer>
#include <QXmlStreamWriter>
#include <QDomNamedNodeMap>

RegisterModel::RegisterModel(QDomNode& registerNode): id_(), name_(),
displayName_(), description_(), parameters_() {

	// get the spirit:id attribute
	QDomNamedNodeMap attributeMap = registerNode.attributes();
	id_ = attributeMap.namedItem(QString("spirit:id")).nodeValue();
	id_ = General::removeWhiteSpace(id_);

	// go through child nodes and parse info
	for (int i = 0; i < registerNode.childNodes().count(); ++i) {
		QDomNode tempNode = registerNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:name")) {
			name_ = tempNode.childNodes().at(0).nodeValue();
			name_ = General::removeWhiteSpace(name_);
		}
		else if (tempNode.nodeName() == QString("spirit:displayName")) {
			displayName_ = tempNode.childNodes().at(0).nodeValue();
		}
		else if (tempNode.nodeName() == QString("spirit:description")) {
			description_ = tempNode.childNodes().at(0).nodeValue();
		}
		else if (tempNode.nodeName() == QString("spirit:parameters")) {

			// parse each parameter
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QDomNode parameterNode = tempNode.childNodes().at(j);

				parameters_.append(QSharedPointer<Parameter>(
						new Parameter(parameterNode)));
			}
		}
	}
}

RegisterModel::RegisterModel( const RegisterModel& other ):
id_(other.id_),
name_(other.name_),
displayName_(other.displayName_),
description_(other.description_),
parameters_() {

	foreach (QSharedPointer<Parameter> param, other.parameters_) {
		if (param) {
			QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(
				new Parameter(*param.data()));
			parameters_.append(copy);
		}
	}
}

RegisterModel& RegisterModel::operator=( const RegisterModel& other ) {
	if (this != &other) {
		id_ = other.id_;
		name_ = other.name_;
		displayName_ = other.displayName_;
		description_ = other.description_;

		parameters_.clear();
		foreach (QSharedPointer<Parameter> param, other.parameters_) {
			if (param) {
				QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(
					new Parameter(*param.data()));
				parameters_.append(copy);
			}
		}
	}
	return *this;
}

RegisterModel::~RegisterModel() {
	parameters_.clear();
}

void RegisterModel::write(QXmlStreamWriter& writer) {
	// if optional spirit:id attribute exists
	if (!id_.isEmpty()) {
		writer.writeAttribute("spirit:id", id_);
	}

	// if mandatory name is missing
	if (name_.isEmpty()) {
		throw Write_error(QObject::tr("Mandatory name missing in "
				"RegisterModel"));
	}
	else {
		writer.writeTextElement("spirit:name", name_);
	}

	// if optional displayName defined
	if (!displayName_.isEmpty()) {
		writer.writeTextElement("spirit:displayName", displayName_);
	}

	// if optional description is defined
	if (!description_.isEmpty()) {
		writer.writeTextElement("spirit:description", description_);
	}

	// if optional parameters are defined
	if (parameters_.size() != 0) {
		writer.writeStartElement("spirit:parameters");

		// write each parameter
		for (int i = 0; i < parameters_.size(); ++i) {
			parameters_.at(i)->write(writer);
		}

		writer.writeEndElement(); // spirit:parameters
	}
	return;
}

QString RegisterModel::getDescription() const {
    return description_;
}

QString RegisterModel::getDisplayName() const {
    return displayName_;
}

QString RegisterModel::getId() const {
    return id_;
}

QString RegisterModel::getName() const {
    return name_;
}

const QList<QSharedPointer<Parameter> >& RegisterModel::getParameters() const {
    return parameters_;
}

void RegisterModel::setDescription(const QString& description) {
    this->description_ = description;
}

void RegisterModel::setDisplayName(const QString& displayName) {
    this->displayName_ = displayName;
}

void RegisterModel::setId(const QString& id) {
    this->id_ = id;
}

void RegisterModel::setName(const QString& name) {
    this->name_ = name;
}

void RegisterModel::setParameters(
		const QList<QSharedPointer<Parameter> >& parameters) {
	parameters_.clear();
    this->parameters_ = parameters;
}