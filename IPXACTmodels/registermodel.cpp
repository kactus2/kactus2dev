/* 
 *
 *  Created on: 26.10.2010
 *      Author: Antti Kamppi
 */

#include "registermodel.h"
#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/ParameterReader.h>
#include <IPXACTmodels/common/ParameterWriter.h>

#include <IPXACTmodels/XmlUtils.h>

#include <QList>
#include <QDomNode>
#include <QString>
#include <QSharedPointer>
#include <QXmlStreamWriter>
#include <QDomNamedNodeMap>

RegisterModel::RegisterModel(QDomNode& registerNode):
NameGroup(), 
    id_(), 
parameters_() {

	// get the spirit:id attribute
	QDomNamedNodeMap attributeMap = registerNode.attributes();
	id_ = attributeMap.namedItem(QString("spirit:id")).nodeValue();
	id_ = XmlUtils::removeWhiteSpace(id_);

	// go through child nodes and parse info
	for (int i = 0; i < registerNode.childNodes().count(); ++i) {
		QDomNode tempNode = registerNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:parameters")) {
            ParameterReader reader;
			// parse each parameter
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QDomNode parameterNode = tempNode.childNodes().at(j);

				parameters_.append(QSharedPointer<Parameter>(reader.createParameterFrom(parameterNode)));
			}
		}
	}
}

RegisterModel::RegisterModel(): NameGroup()
{

}

RegisterModel::RegisterModel( const RegisterModel& other ):
NameGroup(other),
    id_(other.id_),
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
	if (this != &other)
    {
		NameGroup::operator=(other);
		id_ = other.id_;
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

	writer.writeTextElement("spirit:name", name());

	// if optional displayName defined
	if (!displayName().isEmpty()) {
		writer.writeTextElement("spirit:displayName", displayName());
	}

	// if optional description is defined
	if (!description().isEmpty()) {
		writer.writeTextElement("spirit:description", description());
	}

    if (parameters_.size() != 0)
    {
        writer.writeStartElement("ipxact:parameters");

        ParameterWriter parameterWriter;
        // write each parameter
        for (int i = 0; i < parameters_.size(); ++i)
        {
            parameterWriter.writeParameter(writer, parameters_.at(i));
        }

        writer.writeEndElement(); // ipxact:parameters
    }
}

QString RegisterModel::getId() const {
    return id_;
}

const QList<QSharedPointer<Parameter> >& RegisterModel::getParameters() const {
    return parameters_;
}


void RegisterModel::setId(const QString& id) {
    this->id_ = id;
}

void RegisterModel::setParameters(
		const QList<QSharedPointer<Parameter> >& parameters) {
	parameters_.clear();
    parameters_ = parameters;
}
