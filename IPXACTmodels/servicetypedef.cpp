/* 
 *
 *  Created on: 3.8.2010
 *      Author: Antti Kamppi
 */

#include "servicetypedef.h"
#include "parameter.h"
#include "generaldeclarations.h"

#include <IPXACTmodels/XmlUtils.h>

#include <QDomNode>
#include <QString>
#include <QList>
#include <QDomNamedNodeMap>
#include <QSharedPointer>
#include <QXmlStreamWriter>

ServiceTypeDef::ServiceTypeDef(QDomNode &serviceTypeNode): typeName_(QString()),
	typeDefinition_(QString()), parameters_(), constrained_(false),
	implicit_(false) {

	for (int i = 0; i < serviceTypeNode.childNodes().count(); ++i) {
		QDomNode tempNode = serviceTypeNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:typeName")) {
			typeName_ = tempNode.childNodes().at(0).nodeValue();
			typeName_ = XmlUtils::removeWhiteSpace(typeName_);

			// get attributes
			QDomNamedNodeMap attributeMap = tempNode.attributes();
			QString tempAtt;

			// get the constrained value as QString
			tempAtt = attributeMap.namedItem(QString("spirit:constrained")).
					childNodes().at(0).nodeValue();

			// convert the QString into boolean
			constrained_ = General::str2Bool(tempAtt, false);

			tempAtt = attributeMap.namedItem(QString("spirit:implicit")).
					childNodes().at(0).nodeValue();

			implicit_ = General::str2Bool(tempAtt, false);
		}

		else if (tempNode.nodeName() == QString("spirit:typeDefinition")) {
			typeDefinition_ = tempNode.childNodes().at(0).nodeValue();
		}

		else if (tempNode.nodeName() == QString("spirit:parameters")) {

			// go through all parameters
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QDomNode parameterNode = tempNode.childNodes().at(j);
				parameters_.append(QSharedPointer<Parameter>(
						new Parameter(parameterNode)));
			}
		}
	}
}

ServiceTypeDef::ServiceTypeDef( const ServiceTypeDef &other ):
typeName_(other.typeName_),
typeDefinition_(other.typeDefinition_),
parameters_(),
constrained_(other.constrained_),
implicit_(other.implicit_) {

	foreach (QSharedPointer<Parameter> param, other.parameters_) {
		if (param) {
			QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(
				new Parameter(*param.data()));
			parameters_.append(copy);
		}
	}
}


ServiceTypeDef & ServiceTypeDef::operator=( const ServiceTypeDef &other ) {
	if (this != &other) {
		typeName_ = other.typeName_;
		typeDefinition_ = other.typeDefinition_;
		constrained_ = other.constrained_;
		implicit_ = other.implicit_;

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


ServiceTypeDef::~ServiceTypeDef() {
	parameters_.clear();
}

void ServiceTypeDef::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:serviceTypeDef");

	// start the spirit:typeName tag
	writer.writeStartElement("spirit:typeName");

    // write attributes for the element
    writer.writeAttribute("spirit:constrained", General::bool2Str(constrained_));
    writer.writeAttribute("spirit:implicit", General::bool2Str(implicit_));

    // write the value of the element and close the tag
    writer.writeCharacters(typeName_);
    writer.writeEndElement(); // spirit:typeName

	// if optional typeDefinition is specified
	if (!typeDefinition_.isEmpty()) {
                writer.writeTextElement("spirit:typeDefinition",
                		typeDefinition_);
	}

	// if any parameter are found
	if (parameters_.size() != 0) {
		writer.writeStartElement("spirit:parameters");

		// go through each parameter
		for (int i = 0; i < parameters_.size(); ++i) {
			parameters_.at(i)->write(writer);
		}
		writer.writeEndElement(); // spirit:parameters
	}
	writer.writeEndElement(); // spirit:serviceTypeDef
}

QString ServiceTypeDef::getTypeName() const {
	return typeName_;
}

void ServiceTypeDef::setTypeName(const QString &typeName) {
	typeName_ = typeName;
}

const QList<QSharedPointer<Parameter> >& ServiceTypeDef::getParameters() {
	return parameters_;
}

void ServiceTypeDef::setParameters(const
		QList<QSharedPointer<Parameter> > &parameters) {
	// delete old parameters first
	parameters_.clear();

	// save new parameters
	parameters_ = parameters;
}

QString ServiceTypeDef::getTypeDefinition() const {
	return typeDefinition_;
}

void ServiceTypeDef::setTypeDefinition(const QString &typeDefinition) {
	typeDefinition_ = typeDefinition;
}

bool ServiceTypeDef::getConstrained() const {
	return constrained_;
}

void ServiceTypeDef::setImplicit(bool implicit) {
	implicit_ = implicit;
}

bool ServiceTypeDef::getImplicit() const {
	return implicit_;
}

void ServiceTypeDef::setConstrained(bool constrained) {
	constrained_ = constrained;
}