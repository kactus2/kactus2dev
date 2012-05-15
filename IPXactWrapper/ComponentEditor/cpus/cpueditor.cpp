/* 
 *  	Created on: 27.2.2012
 *      Author: Antti Kamppi
 * 		filename: cpueditor.cpp
 *		Project: Kactus 2
 */

#include "cpueditor.h"

#include <QVBoxLayout>

CpuEditor::CpuEditor( QSharedPointer<Component> component, 
					 void* dataPointer, 
					 QWidget *parent ):
ItemEditor(component, parent),
cpu_(static_cast<Cpu*>(dataPointer)),
nameGroup_(this, tr("Name and description")), 
addrSpaceRefs_(component, tr("Address space references"), this), 
parameters_(&cpu_->getParameters(), this) {

	Q_ASSERT(component);
	Q_ASSERT(dataPointer);
	initialize();
}

CpuEditor::CpuEditor( QSharedPointer<Component> component,
					 QSharedPointer<Cpu> cpu,
					 QWidget *parent ):
ItemEditor(component, parent),
cpu_(cpu.data()),
nameGroup_(this, tr("Name and description")), 
addrSpaceRefs_(component, tr("Address space references"), this), 
parameters_(&cpu_->getParameters(), this) {

	Q_ASSERT(component);
	Q_ASSERT(cpu_);
	initialize();
}

CpuEditor::~CpuEditor() {
}

void CpuEditor::initialize() {
	
	connect(&nameGroup_, SIGNAL(nameChanged(const QString&)),
		this, SIGNAL(nameChanged(const QString&)), Qt::UniqueConnection);
	connect(&nameGroup_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&addrSpaceRefs_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&parameters_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&parameters_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(&parameters_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&nameGroup_);
	layout->addWidget(&addrSpaceRefs_);
	layout->addWidget(&parameters_);

	restore();
}

bool CpuEditor::isValid() const {
	if (!nameGroup_.isValid()) {
		return false;
	}
	if (addrSpaceRefs_.size() <= 0) {
		return false;
	}
	if (!parameters_.isValid()) {
		return false;
	}

	return true;
}

void CpuEditor::removeModel() {
	component()->removeCpu(cpu_->getName());
	cpu_ = 0;
}

void CpuEditor::makeChanges() {
	cpu_->setName(nameGroup_.getName());
	cpu_->setDisplayName(nameGroup_.getDisplayName());
	cpu_->setDescription(nameGroup_.getDescription());

	cpu_->setAddressSpaceRefs(addrSpaceRefs_.items());

	parameters_.apply();
}

void CpuEditor::restore() {
	nameGroup_.setName(cpu_->getName());
	nameGroup_.setDisplayName(cpu_->getDisplayName());
	nameGroup_.setDescription(cpu_->getDescription());

	addrSpaceRefs_.initialize(cpu_->getAddressSpaceRefs());

	parameters_.restore();
}
