/* 
 *
 *  Created on: 26.10.2010
 *      Author: Antti Kamppi
 */

#include "alternateregister.h"
#include "registerdefinition.h"
#include "generaldeclarations.h"


#include "../exceptions/write_error.h"

#include <QXmlStreamWriter>
#include <QDomNode>
#include <QString>
#include <QList>
#include <QSharedPointer>

AlternateRegister::AlternateRegister(QDomNode& registerNode):
RegisterModel(registerNode), alternateGroups_(), alternateRegisterDef_() {

	for (int i = 0; i < registerNode.childNodes().count(); ++i) {
		QDomNode tempNode = registerNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:alternateGroups")) {

			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QString groupName = tempNode.childNodes().at(0).nodeValue();
				alternateGroups_.append(General::removeWhiteSpace(groupName));
			}
		}
	}
	alternateRegisterDef_ = QSharedPointer<RegisterDefinition>(
			new RegisterDefinition(registerNode));

	// at least one alternate group must be found
// 	if (alternateGroups_.size() == 0) {
// 		throw Parse_error(QObject::tr("Atleast one alternateGroup must be "
// 				"found in spirit:alternateRegister"));
// 	}
	return;
}

AlternateRegister::AlternateRegister( const AlternateRegister& other ):
RegisterModel(other),
alternateGroups_(other.alternateGroups_),
alternateRegisterDef_() {

	if (other.alternateRegisterDef_) {
		alternateRegisterDef_ = QSharedPointer<RegisterDefinition>(
			new RegisterDefinition(*other.alternateRegisterDef_.data()));
	}
}

AlternateRegister& AlternateRegister::operator=( const AlternateRegister& other ) {
	if (this != &other) {
		RegisterModel::operator=(other);

		alternateGroups_ = other.alternateGroups_;

		if (other.alternateRegisterDef_) {
			alternateRegisterDef_ = QSharedPointer<RegisterDefinition>(
				new RegisterDefinition(*other.alternateRegisterDef_.data()));
		}
		else
			alternateRegisterDef_ = QSharedPointer<RegisterDefinition>();
	}
	return *this;
}

AlternateRegister::~AlternateRegister() {
	alternateGroups_.clear();
	alternateRegisterDef_.clear();
}

QSharedPointer<RegisterModel> AlternateRegister::clone() {
	return QSharedPointer<RegisterModel>(new AlternateRegister(*this));
}

void AlternateRegister::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:alternateRegister");

	// call base class to write itself
	RegisterModel::write(writer);

	// if no alternateGroups exist
	if (alternateGroups_.size() == 0) {
		throw Write_error(QObject::tr("Atleast one alternateGroup must be "
				"found in spirit:alternateRegister"));
	}
	else {
		writer.writeStartElement("spirit:alternateGroups");

		for (int i = 0; i < alternateGroups_.size(); ++i) {
			writer.writeTextElement("spirit:alternateGroup",
					alternateGroups_.at(i));
		}

		writer.writeEndElement(); // spirit:alternateGroups
	}

	// write register definition group is one exists
	if (alternateRegisterDef_) {
		alternateRegisterDef_->write(writer);
	}

	writer.writeEndElement(); // spirit:alternateRegister
}

bool AlternateRegister::isValid( QStringList& errorList, 
								const QString& parentIdentifier ) const {

	bool valid = true;

	if (name_.isEmpty()) {
		errorList.append(QObject::tr("No name specified for alternate register"
			" within %1").arg(parentIdentifier));
		valid = false;
	}

	if (alternateGroups_.isEmpty()) {
		errorList.append(QObject::tr("At least one alternate group must be "
			"specified for alternate register %1 within %2").arg(name_).arg(
			parentIdentifier));
		valid = false;
	}

	if (alternateRegisterDef_ && !alternateRegisterDef_->isValid(errorList, QObject::tr(
		"alternate register %1").arg(name_))) {
		valid = false;
	}

	foreach (QSharedPointer<Parameter> param, parameters_) {
		if (!param->isValid(errorList, QObject::tr("alternate register %1").arg(name_))) {
			valid = false;
		}
	}

	return valid;
}

bool AlternateRegister::isValid() const {
	
	if (name_.isEmpty()) {
		return false;
	}

	if (alternateGroups_.isEmpty()) {
		return false;
	}

	if (alternateRegisterDef_ && !alternateRegisterDef_->isValid()) {
			return false;
	}

	foreach (QSharedPointer<Parameter> param, parameters_) {
		if (!param->isValid()) {
			return false;
		}
	}

	return true;
}

const QList<QString>& AlternateRegister::getAlternateGroups() const {
	return alternateGroups_;
}

RegisterDefinition* AlternateRegister::getAlternateRegisterDef() const {
	return alternateRegisterDef_.data();
}

void AlternateRegister::setAlternateGroups(
		const QList<QString>& alternateGroups) {
	this->alternateGroups_.clear();
	this->alternateGroups_ = alternateGroups;
}

void AlternateRegister::setAlternateRegisterDef(
		RegisterDefinition* alternateRegisterDef) {
	this->alternateRegisterDef_ = QSharedPointer<RegisterDefinition>(alternateRegisterDef);
}
