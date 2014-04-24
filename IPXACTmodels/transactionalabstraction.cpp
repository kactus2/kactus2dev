/* 
 *
 *  Created on: 18.8.2010
 *      Author: Antti Kamppi
 */

#include "transactionalabstraction.h"
#include "generaldeclarations.h"

#include <QDomNode>
#include <QList>
#include <QObject>
#include <QString>
#include <QSharedPointer>

// struct constructor
TransactionalAbstraction::TransactionalPort::TransactionalPort(
		QDomNode& portNode): presence_(General::OPTIONAL),
		serviceInitiative_(General::NONE), typeNames_() {
	for (int i = 0; i < portNode.childNodes().count(); ++i) {
		QDomNode tempNode = portNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:presence")) {
			presence_ = General::str2Presence(tempNode.childNodes().at(0).
					nodeValue(), General::OPTIONAL);
		}

		else if (tempNode.nodeName() == QString("spirit:service")) {
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QDomNode serviceNode = tempNode.childNodes().at(j);

				if (serviceNode.nodeName() == QString("spirit:initiative")) {
					serviceInitiative_ = General::str2Initiative(serviceNode.
							childNodes().at(0).nodeValue(), General::NONE);
				}

				else if (serviceNode.nodeName() == QString("spirit:typeName")) {
					typeNames_.append(serviceNode.childNodes().at(0).
							nodeValue());
				}
			}
		}
	}
	return;
}

// class constructor
TransactionalAbstraction::TransactionalAbstraction(QDomNode& transactionalNode):
qualifier_(), onMaster_(), onSlave_() {
	for (int i = 0; i < transactionalNode.childNodes().count(); ++i) {
		QDomNode tempNode = transactionalNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:qualifier")) {
			qualifier_ = QSharedPointer<General::Qualifier>(
				new General::Qualifier(tempNode));
		}

		else if (tempNode.nodeName() == QString("spirit:onMaster")) {
			onMaster_ = QSharedPointer<TransactionalPort>(
				new TransactionalPort(tempNode));
		}

		else if (tempNode.nodeName() == QString("spirit:onSlave")) {
			onSlave_ = QSharedPointer<TransactionalPort>(
				new TransactionalPort(tempNode));
		}
	}
	return;
}

TransactionalAbstraction::TransactionalAbstraction( const TransactionalAbstraction& other ):
qualifier_(),
onMaster_(),
onSlave_() {

	if (other.qualifier_) {
		qualifier_ = QSharedPointer<General::Qualifier>(
			new General::Qualifier(*other.qualifier_.data()));
	}
	if (other.onMaster_) {
		onMaster_ = QSharedPointer<TransactionalPort>(
			new TransactionalPort(*other.onMaster_.data()));
	}
	if (other.onSlave_) {
		onSlave_ = QSharedPointer<TransactionalPort>(
			new TransactionalPort(*other.onSlave_.data()));
	}
}

TransactionalAbstraction& TransactionalAbstraction::operator=( const TransactionalAbstraction& other ) {
	if (this != &other) {
		if (other.qualifier_) {
			qualifier_ = QSharedPointer<General::Qualifier>(
				new General::Qualifier(*other.qualifier_.data()));
		}
		else 
			qualifier_ = QSharedPointer<General::Qualifier>();

		if (other.onMaster_) {
			onMaster_ = QSharedPointer<TransactionalPort>(
				new TransactionalPort(*other.onMaster_.data()));
		}
		else
			onMaster_ = QSharedPointer<TransactionalPort>();

		if (other.onSlave_) {
			onSlave_ = QSharedPointer<TransactionalPort>(
				new TransactionalPort(*other.onSlave_.data()));
		}
		else
			onSlave_ = QSharedPointer<TransactionalPort>();

	}
	return *this;
}

// the desctructor
TransactionalAbstraction::~TransactionalAbstraction() {
}

void TransactionalAbstraction::write(QXmlStreamWriter& writer) {

	if (qualifier_) {
		writer.writeStartElement("spirit:qualifier");

		writer.writeTextElement("spirit:isAddress",
				General::bool2Str(qualifier_->isAddress_));

		writer.writeTextElement("spirit:isData",
				General::bool2Str(qualifier_->isData_));

		writer.writeEndElement(); // spirit:qualifier
	}

	if (onMaster_) {
		writer.writeStartElement("spirit:onMaster");
		writeTransactionalPort(writer, onMaster_.data());
		writer.writeEndElement(); // spirit:onMaster
	}

	if (onSlave_) {
		writer.writeStartElement("spirit:onSlave");
		writeTransactionalPort(writer, onSlave_.data());
		writer.writeEndElement(); // spirit:onSlave
	}
}

void TransactionalAbstraction::writeTransactionalPort(QXmlStreamWriter& writer,
			TransactionalPort* port) {
	writer.writeTextElement("spirit:presence",
			General::presence2Str(port->presence_));

	writer.writeStartElement("spirit:service");

	if (port->serviceInitiative_ != General::NONE &&
			port->serviceInitiative_ != General::INITIATIVE_PHANTOM) {
		writer.writeTextElement("spirit:initiative",
				General::Initiative2Str(port->serviceInitiative_));
	}

    for (int i = 0; i < port->typeNames_.size(); ++i) {
        writer.writeTextElement("spirit:typeName", port->typeNames_.at(i));
    }

	writer.writeEndElement(); // spirit:service
}

General::Qualifier *TransactionalAbstraction::getQualifier() const {
	return qualifier_.data();
}

TransactionalAbstraction::TransactionalPort *TransactionalAbstraction::getOnMaster() const {
	return onMaster_.data();
}

void TransactionalAbstraction::setOnMaster(TransactionalPort *onMaster) {
	if (onMaster_) {
		onMaster_.clear();
	}
	onMaster_ = QSharedPointer<TransactionalPort>(onMaster);
}

TransactionalAbstraction::TransactionalPort *TransactionalAbstraction::getOnSlave() const {
	return onSlave_.data();
}

void TransactionalAbstraction::setQualifier(General::Qualifier *qualifier) {
	if (qualifier_) {
		qualifier_.clear();
	}
	qualifier_ = QSharedPointer<General::Qualifier>(qualifier);
}

void TransactionalAbstraction::setOnSlave(TransactionalPort *onSlave) {
	if (onSlave_) {
		onSlave_.clear();
	}
	onSlave_ = QSharedPointer<TransactionalPort>(onSlave);
}
