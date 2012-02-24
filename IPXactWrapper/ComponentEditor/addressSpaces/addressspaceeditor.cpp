/* 
 *  	Created on: 21.2.2012
 *      Author: Antti Kamppi
 * 		filename: addressspaceeditor.cpp
 *		Project: Kactus 2
 */

#include "addressspaceeditor.h"

#include <QVBoxLayout>

AddressSpaceEditor::AddressSpaceEditor( QSharedPointer<Component> component,
									   void* dataPointer, 
									   QWidget *parent ):
ItemEditor(component, parent),
addrSpace_(static_cast<AddressSpace*>(dataPointer)),
nameBox_(this, tr("Name and description")),
general_(addrSpace_, this),
segments_(addrSpace_, this),
parameterEditor_(&addrSpace_->getParameters(), this) {

	Q_ASSERT(addrSpace_);

	QVBoxLayout* layout = new QVBoxLayout(this);

	layout->addWidget(&nameBox_);
	connect(&nameBox_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&nameBox_, SIGNAL(nameChanged(const QString&)),
		this, SIGNAL(nameChanged(const QString&)), Qt::UniqueConnection);

	layout->addWidget(&general_);
	connect(&general_, SIGNAL(contentChanged()), 
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	layout->addWidget(&segments_);
	connect(&segments_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	layout->addWidget(&parameterEditor_);
	connect(&parameterEditor_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&parameterEditor_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(&parameterEditor_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	restoreChanges();
}

AddressSpaceEditor::~AddressSpaceEditor() {
}

bool AddressSpaceEditor::isValid() const {
	if (!nameBox_.isValid()) {
		return false;
	}
	else if (!general_.isValid()) {
		return false;
	}
	else if (!parameterEditor_.isValid()) {
		return false;
	}
	else if (!segments_.isValid()) {
		return false;
	}
	else {
		return true;
	}
}

void AddressSpaceEditor::makeChanges() {

	addrSpace_->setName(nameBox_.getName());
	addrSpace_->setDisplayName(nameBox_.getDisplayName());
	addrSpace_->setDescription(nameBox_.getDescription());
	general_.makeChanges();
	parameterEditor_.apply();
	segments_.makeChanges();
}

void AddressSpaceEditor::removeModel() {
	QSharedPointer<Component> component = ItemEditor::component();
	component->removeAddressSpace(addrSpace_->getName());
	addrSpace_ = 0;
}

void AddressSpaceEditor::restoreChanges() {
	nameBox_.setName(addrSpace_->getName());
	nameBox_.setDisplayName(addrSpace_->getDisplayName());
	nameBox_.setDescription(addrSpace_->getDescription());
	general_.restoreChanges();
	parameterEditor_.restore();
	segments_.restore();
}
