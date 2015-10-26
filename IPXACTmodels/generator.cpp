/* 
 *
 *  Created on: 11.8.2010
 *      Author: Antti Kamppi
 */

#include "generator.h"

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/ParameterReader.h>
#include <IPXACTmodels/common/ParameterWriter.h>

#include "generaldeclarations.h"
#include "GenericVendorExtension.h"

#include <QList>
#include <QString>
#include <QDomNode>
#include <QDomNamedNodeMap>
#include <QSharedPointer>
#include <QObject>
#include <QXmlStreamWriter>
#include "XmlUtils.h"

// the constructor
Generator::Generator(QDomNode &generatorNode): NameGroup(), hidden_(false),
phase_(-1), parameters_(), apiType_(Generator::NONE), generatorExe_(), vendorExtensions_()
{
	// get attributes for the componentGenerator
	QDomNamedNodeMap attributeMap = generatorNode.attributes();
	QString hidden = attributeMap.namedItem(QString("spirit:hidden")).
			childNodes().at(0).nodeValue();
	hidden_ = General::str2Bool(hidden, false);

	// go through all the child nodes of the component generator
	for (int i = 0; i < generatorNode.childNodes().count(); ++i) {
		QDomNode tempNode = generatorNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:name")) {
			setName(tempNode.childNodes().at(0).nodeValue());
		}

		else if (tempNode.nodeName() == QString("spirit:displayName")) {
			setDisplayName(tempNode.childNodes().at(0).nodeValue());
		}

		else if (tempNode.nodeName() == QString("spirit:phase")) {
			phase_ = tempNode.childNodes().at(0).nodeValue().toDouble();
		}

		else if (tempNode.nodeName() == QString("spirit:parameters"))
        {
            ParameterReader reader;
			// go through all parameters
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QDomNode parameterNode = tempNode.childNodes().at(j);
				parameters_.append(reader.createParameterFrom(parameterNode));
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
			setDescription(tempNode.childNodes().at(0).nodeValue());
		}
        else if (tempNode.nodeName() == QString("spirit:vendorExtensions")) 
        {
            int extensionCount = tempNode.childNodes().count();
            for (int j = 0; j < extensionCount; ++j) {
                QDomNode extensionNode = tempNode.childNodes().at(j);
                vendorExtensions_.append(QSharedPointer<VendorExtension>(new GenericVendorExtension(extensionNode)));
            }
        }
	}
}

Generator::Generator( const Generator &other ):
NameGroup(other),
hidden_(other.hidden_),
phase_(other.phase_),
parameters_(),
apiType_(other.apiType_),
generatorExe_(other.generatorExe_),
vendorExtensions_(other.vendorExtensions_)
{
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
        NameGroup::operator=(other);
		hidden_ = other.hidden_;
		phase_ = other.phase_;
		apiType_ = other.apiType_;
		generatorExe_ = other.generatorExe_;
        vendorExtensions_ = other.vendorExtensions_;

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

	writer.writeTextElement("spirit:name", name());

	if (!displayName().isEmpty()) {
                writer.writeTextElement("spirit:displayName", displayName());
	}

	if (!description().isEmpty()) {
		writer.writeTextElement("spirit:description", description());
	}

	if (phase_ >= 0) {
		writer.writeTextElement("spirit:phase", QString::number(phase_));
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

    if (!vendorExtensions_.isEmpty())
    {
        writer.writeStartElement("spirit:vendorExtensions");
        //XmlUtils::writeVendorExtensions(writer, vendorExtensions_);
        writer.writeEndElement(); // spirit:vendorExtensions
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

void Generator::setPhase(double phase) {
	phase_ = phase;
}

void Generator::setGeneratorExe(const QString &generatorExe) {
	generatorExe_ = generatorExe;
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
