/* 
 *
 *  Created on: 11.8.2010
 *      Author: Antti Kamppi
 */

#include "generator.h"
#include "parameter.h"
#include "generaldeclarations.h"

#include "../exceptions/parse_error.h"
#include "../exceptions/write_error.h"

#include <QList>
#include <QString>
#include <QDomNode>
#include <QDomNamedNodeMap>
#include <QSharedPointer>
#include <QObject>
#include <QXmlStreamWriter>

// the constructor
Generator::Generator(QDomNode &generatorNode): hidden_(false),
name_(), displayName_(), description_(),
phase_(-1), parameters_(), apiType_(Generator::NONE), generatorExe_() {

	// get attributes for the componentGenerator
	QDomNamedNodeMap attributeMap = generatorNode.attributes();
	QString hidden = attributeMap.namedItem(QString("spirit:hidden")).
			childNodes().at(0).nodeValue();
	hidden_ = General::str2Bool(hidden, false);

	// go through all the child nodes of the component generator
	for (int i = 0; i < generatorNode.childNodes().count(); ++i) {
		QDomNode tempNode = generatorNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:name")) {
			name_ = tempNode.childNodes().at(0).nodeValue();
		}

		else if (tempNode.nodeName() == QString("spirit:displayName")) {
			displayName_ = tempNode.childNodes().at(0).nodeValue();
		}

		else if (tempNode.nodeName() == QString("spirit:phase")) {
			phase_ = tempNode.childNodes().at(0).nodeValue().toDouble();
		}

		else if (tempNode.nodeName() == QString("spirit:parameters")) {

			// go through all parameters
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QDomNode parameterNode = tempNode.childNodes().at(j);
				parameters_.append(QSharedPointer<Parameter>(
						new Parameter(parameterNode)));
			}
		}

		else if (tempNode.nodeName() == QString("spirit:apiType")) {
			if (tempNode.childNodes().at(0).nodeValue() == QString("TGI")) {
				apiType_ = Generator::TGI;
			}
			else {
				apiType_ = Generator::NONE;
			}
		}

		else if (tempNode.nodeName() == QString("spirit:generatorExe")) {
			generatorExe_ = tempNode.childNodes().at(0).nodeValue();
		}

		else if (tempNode.nodeName() == QString("spirit:description")) {
			description_ = tempNode.childNodes().at(0).nodeValue();
		}
	}

	// if mandatory fields are missing

// 	if (name_.isNull() ) {
// 		throw Parse_error(QObject::tr("Mandatory element name missing in "
// 				" generator"));
// 	}
// 
// 	if (generatorExe_.isNull()) {
// 		throw Parse_error(QObject::tr("Mandatory element spirit:generatorExe "
// 				"missing in generator"));
// 	}
	return;
}

Generator::Generator( const Generator &other ):
hidden_(other.hidden_),
name_(other.name_),
displayName_(other.displayName_),
description_(other.description_),
phase_(other.phase_),
parameters_(),
apiType_(other.apiType_),
generatorExe_(other.generatorExe_) {

	foreach (QSharedPointer<Parameter> param, other.parameters_) {
		if (param) {
			QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(
				new Parameter(*param.data()));
			parameters_.append(copy);
		}
	}
}

Generator & Generator::operator=( const Generator &other ) {
	if (this != &other) {
		hidden_ = other.hidden_;
		name_ = other.name_;
		displayName_ = other.displayName_;
		description_ = other.description_;
		phase_ = other.phase_;
		apiType_ = other.apiType_;
		generatorExe_ = other.generatorExe_;

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

// the destructor
Generator::~Generator() {
	parameters_.clear();
}

void Generator::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:generator");
	writer.writeAttribute("spirit:hidden", General::bool2Str(hidden_));

	// if mandatory name is missing
	if (name_.isEmpty()) {
		throw Write_error(QObject::tr("Mandatory name missing in spirit:"
				"generator"));
	}
	else {
		writer.writeTextElement("spirit:name", name_);
	}

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

	// if mandatory generatorExe is missing
	if (generatorExe_.isEmpty()) {
		throw Write_error(QObject::tr("Mandatory generatorExe is missing in "
				"spirit:generator"));
	}
	else {
		writer.writeTextElement("spirit:generatorExe", generatorExe_);
	}

	writer.writeEndElement(); // spirit:generator
}

double Generator::getPhase() const {
	return phase_;
}

void Generator::setHidden(bool hidden) {
	hidden_ = hidden;
}

bool Generator::getHidden() const {
	return hidden_;
}

void Generator::setName(const QString &name) {
	name_ = name;
}

void Generator::setPhase(double phase) {
	phase_ = phase;
}

void Generator::setGeneratorExe(const QString &generatorExe) {
	generatorExe_ = generatorExe;
}

QString Generator::getName() const {
	return name_;
}

void Generator::setParameters(const QList<QSharedPointer<Parameter> > &parameters) {
	// delete old parameters
	parameters_.clear();

	// save the new parameters
	parameters_ = parameters;
}

void Generator::setApiType(ApiType apiType) {
	apiType_ = apiType;
}

const QList<QSharedPointer<Parameter> >& Generator::getParameters() {
	return parameters_;
}

QString Generator::getGeneratorExe() const {
	return generatorExe_;
}

Generator::ApiType Generator::getApiType() const {
	return apiType_;
}

void Generator::setDescription(const QString &description) {
	description_ = description;
}

QString Generator::getDescription() const {
	return description_;
}

QString Generator::getDisplayName() const {
	return displayName_;
}

void Generator::setDisplayName(const QString &displayName) {
	displayName_ = displayName;
}