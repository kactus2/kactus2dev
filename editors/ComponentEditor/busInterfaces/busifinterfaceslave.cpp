//-----------------------------------------------------------------------------
// File: busifinterfaceslave.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 7.4.2011
//
// Description:
// Editor to the slave details of a bus interface.
//-----------------------------------------------------------------------------

#include "busifinterfaceslave.h"

#include <IPXACTmodels/generaldeclarations.h>

#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceSlave::BusIfInterfaceSlave()
//-----------------------------------------------------------------------------
BusIfInterfaceSlave::BusIfInterfaceSlave(QSharedPointer<BusInterface> busif,
										 QSharedPointer<Component> component,
										 QWidget *parent):
BusIfInterfaceModeEditor(busif, component, tr("Slave"), parent),
slave_(QSharedPointer<SlaveInterface>(new SlaveInterface())),
memoryMapReferenceSelector_(this),
bridges_(slave_, component, this)
{
	Q_ASSERT(slave_);

	QHBoxLayout* memRefLayout = new QHBoxLayout();
    memRefLayout->addWidget(new QLabel(tr("Memory map")));
    memRefLayout->addWidget(&memoryMapReferenceSelector_);
    memRefLayout->addStretch();
	
	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->addLayout(memRefLayout);
	topLayout->addWidget(&bridges_);

	connect(&memoryMapReferenceSelector_, SIGNAL(itemSelected(QString const&)),
		this, SLOT(onMemoryMapChange(QString const&)), Qt::UniqueConnection);
	
    connect(&bridges_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceSlave::~BusIfInterfaceSlave()
//-----------------------------------------------------------------------------
BusIfInterfaceSlave::~BusIfInterfaceSlave()
{
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceSlave::isValid()
//-----------------------------------------------------------------------------
bool BusIfInterfaceSlave::isValid() const
{
	return true;
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceSlave::refresh()
//-----------------------------------------------------------------------------
void BusIfInterfaceSlave::refresh()
{
	// if the model contains master-element
	if (busif_->getSlave())
    {
		slave_ = busif_->getSlave();
	}
	else 
    {
		slave_.clear();
		slave_ = QSharedPointer<SlaveInterface>(new SlaveInterface());
	}

	// update the selectable items
	memoryMapReferenceSelector_.refresh(component_->getMemoryMapNames());

	// select the correct memory map in the combo box
	QString memMapRef = slave_->getMemoryMapRef();
	memoryMapReferenceSelector_.selectItem(memMapRef);

	bridges_.refresh(slave_);
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceSlave::getInterfaceMode()
//-----------------------------------------------------------------------------
General::InterfaceMode BusIfInterfaceSlave::getInterfaceMode() const
{
	return General::SLAVE;
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceSlave::onMemoryMapChange()
//-----------------------------------------------------------------------------
void BusIfInterfaceSlave::onMemoryMapChange(QString const& newMemoryMapName)
{
	slave_->setMemoryMapRef(newMemoryMapName);
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceSlave::saveModeSpecific()
//-----------------------------------------------------------------------------
void BusIfInterfaceSlave::saveModeSpecific()
{
	busif_->setSlave(slave_);
}
