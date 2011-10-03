/* 
 *
 *  Created on: 3.8.2010
 *      Author: Antti Kamppi
 */

#include "transactional.h"
#include "servicetypedef.h"
#include "generaldeclarations.h"

#include "../exceptions/parse_error.h"
#include "../exceptions/write_error.h"

#include <stdexcept>
#include <QString>
#include <QDomNode>
#include <QDomNamedNodeMap>
#include <QList>
#include <QSharedPointer>
#include <QXmlStreamWriter>

const unsigned int DEFAULT_MAX_CONNECTIONS = 0;
const unsigned int DEFAULT_MIN_CONNECTIONS = 1;

// The contructor
Transactional::TransTypeDef::TransTypeDef(QDomNode &transTypeNode):
				typeName_(), constrained_(false), typeDefinitions_() {

	for (int i = 0; i < transTypeNode.childNodes().count(); ++i) {
		QDomNode tempNode = transTypeNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:typeName")) {
			Transactional::TransTypeDef::typeName_ =
					tempNode.childNodes().at(0).nodeValue();

			// get the attributes
			QDomNamedNodeMap attributeMap = tempNode.attributes();

			// get the value as QString
			QString constrainedStr = attributeMap.namedItem(QString(
					"spirit:constrained")).childNodes().at(0).nodeValue();

			// convert QString -> boolean
			Transactional::TransTypeDef::constrained_ =
					General::str2Bool(constrainedStr, false);
		}

		else if (tempNode.nodeName() == QString("spirit:typeDefinition")) {
			Transactional::TransTypeDef::typeDefinitions_.append(
					tempNode.childNodes().at(0).nodeValue());
		}
	}

	// if mandatory element is missing
	if (Transactional::TransTypeDef::typeName_.isNull()) {
		throw Parse_error(QObject::tr("Mandatory element spirit:typeName"
				" missing in spirit:transTypeDef"));
	}
	return;
}

Transactional::Transactional(QDomNode &transactionalNode): transTypeDef_(0),
		allLogicalInitiativesAllowed_(false),
		maxConnections_(DEFAULT_MAX_CONNECTIONS),
		minConnections_(DEFAULT_MIN_CONNECTIONS),
		serviceInitiative_(General::NONE), serviceTypeDefs_() {

	for (int i = 0; i < transactionalNode.childNodes().count(); ++i) {
		QDomNode tempNode = transactionalNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:transTypeDef")) {
			transTypeDef_ = QSharedPointer<TransTypeDef>(
					new TransTypeDef(tempNode));
		}

		// parse the spirit:service element within this class
		else if (tempNode.nodeName() == QString("spirit:service")) {
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {

				if (tempNode.childNodes().at(j).nodeName() ==
						QString("spirit:serviceTypeDefs")) {

					// go through each serviceTypeDef
					for (int k = 0;
							k < tempNode.childNodes().at(j).childNodes().at(k).
									childNodes().count(); ++k) {
						QDomNode serviceNode = tempNode.childNodes().at(j).
								childNodes().at(k);
						serviceTypeDefs_.append(
								QSharedPointer<ServiceTypeDef>(
										new ServiceTypeDef(serviceNode)));
					}
				}

				else if (tempNode.childNodes().at(j).nodeName() ==
						QString("spirit:initiative")) {
					serviceInitiative_ = General::str2Initiative(tempNode.
							childNodes().at(j).childNodes().at(0).nodeValue(),
							General::NONE);
				}
			}
		}

		else if (tempNode.nodeName() == QString("spirit:connection")) {

			for (int j = 0; j < tempNode.childNodes().count(); ++j) {

				if (tempNode.childNodes().at(j).nodeName() ==
						QString("spirit:maxConnections")) {
					bool ok = false;
					maxConnections_ = tempNode.childNodes().at(j).childNodes().
							at(0).nodeValue().
							toInt(&ok);

					// if conversion failed
					if (!ok) {
						maxConnections_ = DEFAULT_MAX_CONNECTIONS;
					}
				}
				else if (tempNode.childNodes().at(j).nodeName() ==
						QString("spirit:minConnections")) {
					bool ok = false;
					minConnections_ = tempNode.
							childNodes().at(j).childNodes().at(0).
							nodeValue().toInt(&ok);

					// if conversion failed
					if (!ok) {
						minConnections_ = DEFAULT_MIN_CONNECTIONS;
					}
				}
			}
		}
	}

	// if mandatory element initiative was not defined
	if (serviceInitiative_ == General::NONE) {
		throw Parse_error(QObject::tr("Mandatory element spirit:initiative"
				" missing in spirit:service"));
	}
	return;
}

Transactional::Transactional( const Transactional &other ):
transTypeDef_(),
allLogicalInitiativesAllowed_(other.allLogicalInitiativesAllowed_),
maxConnections_(other.maxConnections_), 
minConnections_(other.minConnections_),
serviceInitiative_(other.serviceInitiative_),
serviceTypeDefs_() {

	if (other.transTypeDef_) {
		transTypeDef_ = QSharedPointer<TransTypeDef>(
			new TransTypeDef(*other.transTypeDef_.data()));
	}

	foreach (QSharedPointer<ServiceTypeDef> typeDef, other.serviceTypeDefs_) {
		if (typeDef) {
			QSharedPointer<ServiceTypeDef> copy = QSharedPointer<ServiceTypeDef>(
				new ServiceTypeDef(*typeDef.data()));
			serviceTypeDefs_.append(copy);
		}
	}
}

Transactional& Transactional::operator=( const Transactional &other ) {
	if (this != &other) {
		allLogicalInitiativesAllowed_ = other.allLogicalInitiativesAllowed_;
		maxConnections_ = other.maxConnections_;
		minConnections_ = other.minConnections_;
		serviceInitiative_ = other.serviceInitiative_;

		if (other.transTypeDef_) {
			transTypeDef_ = QSharedPointer<TransTypeDef>(
				new TransTypeDef(*other.transTypeDef_.data()));
		}
		else
			transTypeDef_ = QSharedPointer<TransTypeDef>();

		serviceTypeDefs_.clear();
		foreach (QSharedPointer<ServiceTypeDef> typeDef, other.serviceTypeDefs_) {
			if (typeDef) {
				QSharedPointer<ServiceTypeDef> copy = QSharedPointer<ServiceTypeDef>(
					new ServiceTypeDef(*typeDef.data()));
				serviceTypeDefs_.append(copy);
			}
		}
	}
	return *this;
}

Transactional::~Transactional() {
	serviceTypeDefs_.clear();
}

void Transactional::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:transactional");
	writer.writeAttribute("spirit:allLogicalInitiativesAllowed",
			General::bool2Str(allLogicalInitiativesAllowed_));

	// if spirit:transTypeDef is defined
	if (transTypeDef_) {
		writer.writeStartElement("spirit:transTypeDef");

		// if mandatory typeName is not found
		if (transTypeDef_->typeName_.isEmpty()) {
			throw Write_error(QObject::tr("Mandatory typeName missing in "
					"spirit:transTypeDef"));
		}
		else {
			// start the spirit:typeName tag
			writer.writeStartElement("spirit:typeName");

			// write the attributes for the element
			writer.writeAttribute("spirit:constrained",
					General::bool2Str(transTypeDef_->constrained_));

			// write the value of the element and close the tag
			writer.writeCharacters(transTypeDef_->typeName_);
			writer.writeEndElement(); // spirit:typeName
		}

		// write all typeDefinitions
		for (int i = 0; i < transTypeDef_->typeDefinitions_.size(); ++i) {
			writer.writeTextElement("spirit:typeDefinition",
					transTypeDef_->typeDefinitions_.at(i));
		}

		writer.writeEndElement(); // spirit:transTypeDef
	}

	writer.writeStartElement("spirit:service");

	// if mandatory spirit:initiative is not defined
	if (General::Initiative2Str(serviceInitiative_).isEmpty()) {
		throw Write_error(QObject::tr("Invalid service initiative defined in"
				" spirit:service"));
	}
	else {
		writer.writeTextElement("spirit:initiative",
				General::Initiative2Str(serviceInitiative_));
	}

	if (serviceTypeDefs_.size() != 0) {
		writer.writeStartElement("spirit:serviceTypeDefs");

		// write each service type def
		for (int i = 0; i < serviceTypeDefs_.size(); ++i) {
			serviceTypeDefs_.at(i)->write(writer);
		}
		writer.writeEndElement(); // spirit:serviceTypeDefs
	}

	writer.writeEndElement(); // spirit:service

	writer.writeStartElement("spirit:connection");
	writer.writeTextElement("spirit:maxConnections",
			QString::number(maxConnections_));
	writer.writeTextElement("spirit:minConnections",
			QString::number(minConnections_));
	writer.writeEndElement(); // spirit:connection

	writer.writeEndElement(); // spirit:transactional
}

void Transactional::setServiceInitiative(General::Initiative
		&serviceInitiative) {
	if (serviceInitiative == General::NONE) {
		throw std::invalid_argument("Invalid value given to "
				"serviceInitiative");
	}
	serviceInitiative_ = serviceInitiative;
}

void Transactional::setMaxConnections(unsigned int maxConnections) {
	maxConnections_ = maxConnections;
}

void Transactional::setAllLogicalInitiativesAllowed(
		bool &allLogicalInitiativesAllowed) {
	allLogicalInitiativesAllowed_ = allLogicalInitiativesAllowed;
}

void Transactional::setServiceTypeDefs(const
		QList<QSharedPointer<ServiceTypeDef> > &serviceTypeDefs) {
	// delete the old serviceTypeDefs
	serviceTypeDefs_.clear();

	// save the new serviceTypeDefs
	serviceTypeDefs_ = serviceTypeDefs;
}

const QList<QSharedPointer<ServiceTypeDef> >&
Transactional::getServiceTypeDefs() {
	return serviceTypeDefs_;
}

unsigned int Transactional::getMaxConnections() const {
	return maxConnections_;
}

void Transactional::setMinConnections(unsigned int minConnections) {
	minConnections_ = minConnections;
}

void Transactional::setTransTypeDef(TransTypeDef *transTypeDef) {
	// delete the old transTypeDef
	if (transTypeDef_) {
		transTypeDef_.clear();
	}

	// save the new transTypeDef
	transTypeDef_ = QSharedPointer<TransTypeDef>(transTypeDef);
}

unsigned int Transactional::getMinConnections() const {
	return minConnections_;
}

General::Initiative Transactional::getServiceInitiative() const {
	return serviceInitiative_;
}

Transactional::TransTypeDef *Transactional::getTransTypeDef() const {
	return transTypeDef_.data();
}

bool Transactional::getAllLogicalInitiativesAllowed() const {
	return allLogicalInitiativesAllowed_;
}