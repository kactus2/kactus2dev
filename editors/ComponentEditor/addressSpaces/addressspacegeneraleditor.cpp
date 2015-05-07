/* 
 *  	Created on: 21.2.2012
 *      Author: Antti Kamppi
 * 		filename: addressspacegeneraleditor.cpp
 *		Project: Kactus 2
 */

#include "addressspacegeneraleditor.h"

#include <common/utils.h>

#include <QFormLayout>
#include <QLabel>
#include <QVariant>

AddressSpaceGeneralEditor::AddressSpaceGeneralEditor(QSharedPointer<AddressSpace> addrSpace, 
													 QWidget *parent):
QGroupBox(tr("General"), parent),
addrSpace_(addrSpace),
addrUnitEditor_(this),
rangeEditor_(this),
widthEditor_(this)
{
	Q_ASSERT(addrSpace_);

	rangeEditor_.setPlaceholderText(tr("Range of address space i.e 4G"));

	// Set the back ground colors for mandatory fields.
	addrUnitEditor_.setProperty("mandatoryField", true);
	widthEditor_.setProperty("mandatoryField", true);
	rangeEditor_.setProperty("mandatoryField", true);

	QFormLayout* layout = new QFormLayout(this);
	layout->addRow(tr("Addressable unit bits (AUB)"),&addrUnitEditor_);
	layout->addRow(tr("Range (=size) [AUB]"), &rangeEditor_);
    layout->addRow(tr("Width [bits]"), &widthEditor_);
	layout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	refresh();

	connect(&addrUnitEditor_, SIGNAL(textChanged(QString const&)), this, SLOT(onAddrUnitChanged()), Qt::UniqueConnection);
    connect(&addrUnitEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);

	connect(&widthEditor_, SIGNAL(textChanged(QString const&)), this, SLOT(onWidthChanged()), Qt::UniqueConnection);
    connect(&widthEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);

	connect(&rangeEditor_, SIGNAL(textChanged(QString const&)), this, SLOT(onRangeChanged()), Qt::UniqueConnection);
    connect(&rangeEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
}

AddressSpaceGeneralEditor::~AddressSpaceGeneralEditor()
{
}

bool AddressSpaceGeneralEditor::isValid() const
{
	return !rangeEditor_.text().isEmpty();
}

void AddressSpaceGeneralEditor::refresh()
{
	addrUnitEditor_.setText(QString::number(addrSpace_->getAddressUnitBits()));
    addrSpace_->getLocalMemoryMap()->setAddressUnitBits(addrSpace_->getAddressUnitBits());
	widthEditor_.setText(QString::number(addrSpace_->getWidth()));
	rangeEditor_.setText(addrSpace_->getRange());
}

void AddressSpaceGeneralEditor::onAddrUnitChanged()
{
    int aub = Utils::str2Int(addrUnitEditor_.text());
	addrSpace_->setAddressUnitBits(aub);
    addrSpace_->getLocalMemoryMap()->setAddressUnitBits(aub);

	emit addressableUnitsChanged(aub);
	emit contentChanged();
}

void AddressSpaceGeneralEditor::onWidthChanged()
{
    int width = Utils::str2Int(widthEditor_.text());
	addrSpace_->setWidth(width);    
	emit widthChanged(width);

	emit contentChanged();
}

void AddressSpaceGeneralEditor::onRangeChanged()
{
	addrSpace_->setRange(rangeEditor_.text());
	emit rangeChanged(rangeEditor_.text());

	emit contentChanged();
}
