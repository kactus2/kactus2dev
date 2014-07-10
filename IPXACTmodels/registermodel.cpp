/* 
 *
 *  Created on: 26.10.2010
 *      Author: Antti Kamppi
 */

#include "registermodel.h"
#include "parameter.h"

#include <IPXACTmodels/XmlUtils.h>

#include <QList>
#include <QDomNode>
#include <QString>
#include <QSharedPointer>
#include <QXmlStreamWriter>
#include <QDomNamedNodeMap>

RegisterModel::RegisterModel(QDomNode& registerNode):
id_(), 
nameGroup_(registerNode), 
parameters_() {

	// get the spirit:id attribute
	QDomNamedNodeMap attributeMap = registerNode.attributes();
	id_ = attributeMap.namedItem(QString("spirit:id")).nodeValue();
	id_ = XmlUtils::removeWhiteSpace(id_);

	// go through child nodes and parse info
	for (int i = 0; i < registerNode.childNodes().count(); ++i) {
		QDomNode tempNode = registerNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:parameters")) {

			// parse each parameter
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QDomNode parameterNode = tempNode.childNodes().at(j);

				parameters_.append(QSharedPointer<Parameter>(
						new Parameter(parameterNode)));
			}
		}
	}
}

RegisterModel::RegisterModel() {

}

RegisterModel::RegisterModel( const RegisterModel& other ):
id_(other.id_),
nameGroup_(other.nameGroup_),
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
		nameGroup_ = other.nameGroup_;

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

	writer.writeTextElement("spirit:name", nameGroup_.name());

	// if optional displayName defined
	if (!nameGroup_.displayName().isEmpty()) {
		writer.writeTextElement("spirit:displayName", nameGroup_.displayName());
	}

	// if optional description is defined
	if (!nameGroup_.description().isEmpty()) {
		writer.writeTextElement("spirit:description", nameGroup_.description());
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
    return nameGroup_.description();
}

QString RegisterModel::getDisplayName() const {
    return nameGroup_.displayName();
}

QString RegisterModel::getId() const {
    return id_;
}

QString RegisterModel::getName() const {
    return nameGroup_.name();
}

const QList<QSharedPointer<Parameter> >& RegisterModel::getParameters() const {
    return parameters_;
}

void RegisterModel::setDescription(const QString& description) {
    nameGroup_.setDescription(description);
}

void RegisterModel::setDisplayName(const QString& displayName) {
    nameGroup_.setDisplayName(displayName);
}

void RegisterModel::setId(const QString& id) {
    this->id_ = id;
}

void RegisterModel::setName(const QString& name) {
    nameGroup_.setName(name);
}

void RegisterModel::setParameters(
		const QList<QSharedPointer<Parameter> >& parameters) {
	parameters_.clear();
    parameters_ = parameters;
}