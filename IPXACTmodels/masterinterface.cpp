/* 
 *
 *  Created on: 31.8.2010
 *      Author: Antti Kamppi
 */

#include "masterinterface.h"
#include "XmlUtils.h"


#include <QString>
#include <QMap>
#include <QObject>

MasterInterface::MasterInterface():
addressSpaceRef_(), 
baseAddress_(),
prompt_(QString("Base Address:")),
baseAttributes_() {
}

MasterInterface::MasterInterface( const MasterInterface& other ):
addressSpaceRef_(other.addressSpaceRef_),
baseAddress_(other.baseAddress_),
prompt_(other.prompt_),
baseAttributes_(other.baseAttributes_) {
}


MasterInterface& MasterInterface::operator=( const MasterInterface& other ) {
	if (this != &other) {
		addressSpaceRef_ = other.addressSpaceRef_;
		baseAddress_ = other.baseAddress_;
		prompt_ = other.prompt_;
		baseAttributes_ = other.baseAttributes_;
	}
	return *this;
}


// the destructor
MasterInterface::~MasterInterface() {
}

QString MasterInterface::getAddressSpaceRef() const {
	return addressSpaceRef_;
}

QString MasterInterface::getBaseAddress() const {
	return baseAddress_;
}

const QMap<QString,QString>& MasterInterface::getBaseAttributes() {

	return baseAttributes_;
}

QString MasterInterface::getPrompt() const {
	// if base address is not specified then there is no prompt for it either
	if (baseAddress_.isEmpty())
		return QString();

	return prompt_;
}

void MasterInterface::setAddressSpaceRef(const QString& addressSpaceRef) {
	this->addressSpaceRef_ = addressSpaceRef;
}

void MasterInterface::setBaseAddress(const QString& baseAddress) {
	this->baseAddress_ = baseAddress;
}

void MasterInterface::setBaseAttributes(const
		QMap<QString,QString>& baseAttributes) {
	this->baseAttributes_ = baseAttributes;
}

void MasterInterface::setPrompt(const QString& prompt) {
	this->prompt_ = prompt;
}