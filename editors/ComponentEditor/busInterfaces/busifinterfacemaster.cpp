//-----------------------------------------------------------------------------
// File: busifinterfacemaster.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 7.4.2011
//
// Description:
// Editor to edit master or mirrored master settings of a bus interface.
//-----------------------------------------------------------------------------

#include "busifinterfacemaster.h"

#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/masterinterface.h>

#include <QLabel>
#include <QGridLayout>
#include <QString>

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceMaster::BusIfInterfaceMaster()
//-----------------------------------------------------------------------------
BusIfInterfaceMaster::BusIfInterfaceMaster(General::InterfaceMode mode,
										   QSharedPointer<BusInterface> busif,
										   QSharedPointer<Component> component,
										   QWidget *parent):
BusIfInterfaceModeEditor(busif, component, tr("Master"), parent), 
master_(QSharedPointer<MasterInterface>(new MasterInterface())),
mode_(mode),
addressSpaceReferenceSelector_(this), 
baseAddress_(this) 
{
    // set the title depending on the mode
    if (mode == General::MASTER)
    {
        setTitle(tr("Master"));
    }
    else if (mode == General::MIRROREDMASTER)
    {
        setTitle(tr("Mirrored master"));
    }
    // this editor should only be used for masters and mirrored masters
    else
    {
        Q_ASSERT(false);
    }

	QLabel* addrSpaceLabel = new QLabel(tr("Address space"), this);
	QLabel* baseAddrLabel = new QLabel(tr("Base address"), this);

	QGridLayout* topLayout = new QGridLayout(this);
	topLayout->addWidget(addrSpaceLabel, 0, 0, Qt::AlignLeft);
	topLayout->addWidget(&addressSpaceReferenceSelector_, 0, 1, Qt::AlignLeft);
	topLayout->addWidget(baseAddrLabel, 1, 0, Qt::AlignLeft);
	topLayout->addWidget(&baseAddress_, 1, 1, Qt::AlignLeft);
	topLayout->setColumnStretch(2, 1);
	topLayout->setRowStretch(2, 1);

	connect(&addressSpaceReferenceSelector_, SIGNAL(itemSelected(const QString&)),
		this, SLOT(onAddressSpaceChange(const QString&)), Qt::UniqueConnection);
	connect(&baseAddress_, SIGNAL(textEdited(const QString&)),
		this, SLOT(onBaseAddressChange(const QString&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceMaster::~BusIfInterfaceMaster()
//-----------------------------------------------------------------------------
BusIfInterfaceMaster::~BusIfInterfaceMaster()
{
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceMaster::isValid()
//-----------------------------------------------------------------------------
bool BusIfInterfaceMaster::isValid() const
{
	QString selectedAddrSpace = addressSpaceReferenceSelector_.currentText();

	// address space ref is not mandatory
	if (selectedAddrSpace.isEmpty())
    {
		return true;
	}
	
	// if the selected address space does not belong to component
	QStringList addrSpaceNames = component_->getAddressSpaceNames();
	if (!addrSpaceNames.contains(selectedAddrSpace))
    {
		return false;
	}
	
	return true;
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceMaster::isValid()
//-----------------------------------------------------------------------------
void BusIfInterfaceMaster::refresh()
{
	// if the model contains master-element
	if (busif_->getMaster())
    {
		master_ = busif_->getMaster();
	}
	else
    {
		master_.clear();
		master_ = QSharedPointer<MasterInterface>(new MasterInterface());
	}

	// update the selectable items
	addressSpaceReferenceSelector_.refresh(component_->getAddressSpaceNames());

	QString addrSpaceRef = master_->getAddressSpaceRef();

	// if address space ref is empty then there can be no base address
	if (addrSpaceRef.isEmpty())
    {
		baseAddress_.setDisabled(true);
	}
	else 
    {
		baseAddress_.setEnabled(true);
		baseAddress_.setText(master_->getBaseAddress());
	}

	// select the address space ref and base address
	addressSpaceReferenceSelector_.selectItem(addrSpaceRef);
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceMaster::getInterfaceMode()
//-----------------------------------------------------------------------------
General::InterfaceMode BusIfInterfaceMaster::getInterfaceMode() const
{
	return mode_;
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceMaster::onAddressSpaceChange()
//-----------------------------------------------------------------------------
void BusIfInterfaceMaster::onAddressSpaceChange(const QString& addrSpaceName)
{
	master_->setAddressSpaceRef(addrSpaceName);

	// if address space reference is empty then there can be no base address
	if (addrSpaceName.isEmpty())
    {
		baseAddress_.clear();
		baseAddress_.setDisabled(true);
		master_->setBaseAddress("");
	}
	else
    {
		baseAddress_.setEnabled(true);
	}

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceMaster::onBaseAddressChange()
//-----------------------------------------------------------------------------
void BusIfInterfaceMaster::onBaseAddressChange(const QString& newBase)
{
	master_->setBaseAddress(newBase);
	emit contentChanged();
}	

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceMaster::saveModeSpecific()
//-----------------------------------------------------------------------------
void BusIfInterfaceMaster::saveModeSpecific()
{
	busif_->setMaster(master_);
}
