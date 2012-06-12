/* 
 *  	Created on: 21.2.2012
 *      Author: Antti Kamppi
 * 		filename: addressspacegeneraleditor.cpp
 *		Project: Kactus 2
 */

#include "addressspacegeneraleditor.h"

#include <QGridLayout>
#include <QLabel>
#include <QVariant>

AddressSpaceGeneralEditor::AddressSpaceGeneralEditor(QSharedPointer<AddressSpace> addrSpace, 
													 QWidget *parent):
QGroupBox(tr("General"), parent),
addrSpace_(addrSpace),
addrUnit_(this),
width_(this),
range_(this) {

	Q_ASSERT(addrSpace_);

	addrUnit_.setRange(1, 4096);
	width_.setRange(0, 4096);
	range_.setPlaceholderText(tr("Range of address space i.e 4G"));

	// set the back ground colors for mandatory fields
        addrUnit_.setProperty("mandatoryField", true);
	width_.setProperty("mandatoryField", true);
	range_.setProperty("mandatoryField", true);

	QLabel* unitLabel = new QLabel(tr("Addressable unit size"), this);
	QLabel* widthLabel = new QLabel(tr("Width of address block"), this);
	QLabel* rangeLabel = new QLabel(tr("Range of address block"), this);

	QGridLayout* layout = new QGridLayout(this);
	layout->addWidget(unitLabel, 0, 0, 1, 1);
	layout->addWidget(&addrUnit_, 0, 1, 1, 1);
	layout->addWidget(widthLabel, 1, 0, 1, 1);
	layout->addWidget(&width_, 1, 1, 1, 1);
	layout->addWidget(rangeLabel, 2, 0, 1, 1);
	layout->addWidget(&range_, 2, 1, 1, 1);

	refresh();

	connect(&addrUnit_, SIGNAL(valueChanged(int)),
		this, SLOT(onAddrUnitChanged(int)), Qt::UniqueConnection);

	connect(&width_, SIGNAL(valueChanged(int)),
		this, SLOT(onWidthChanged(int)), Qt::UniqueConnection);

	connect(&range_, SIGNAL(textEdited(const QString&)),
		this, SLOT(onRangeChanged(const QString&)), Qt::UniqueConnection);
}

AddressSpaceGeneralEditor::~AddressSpaceGeneralEditor() {
}

bool AddressSpaceGeneralEditor::isValid() const {
	// if range is not set then this is invalid
	return !range_.text().isEmpty();
}

void AddressSpaceGeneralEditor::refresh() {
	addrUnit_.setValue(addrSpace_->getAddressUnitBits());
	width_.setValue(addrSpace_->getWidth());
	range_.setText(addrSpace_->getRange());
}

void AddressSpaceGeneralEditor::onAddrUnitChanged( int newValue ) {
	addrSpace_->setAddressUnitBits(newValue);
	emit addressableUnitsChanged(newValue);
	emit contentChanged();
}

void AddressSpaceGeneralEditor::onWidthChanged( int newWidth ) {
	addrSpace_->setWidth(newWidth);
	emit widthChanged(newWidth);
	emit contentChanged();
}

void AddressSpaceGeneralEditor::onRangeChanged( const QString& newRange ) {
	addrSpace_->setRange(newRange);
	emit rangeChanged(newRange);
	emit contentChanged();
}
