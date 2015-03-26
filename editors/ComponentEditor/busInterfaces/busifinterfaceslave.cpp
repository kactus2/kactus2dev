/* 
 *
 *  Created on: 7.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfaceslave.cpp
 */

#include "busifinterfaceslave.h"

#include <IPXACTmodels/generaldeclarations.h>

#include <QFormLayout>
#include <QVBoxLayout>

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

	QFormLayout* memRefLayout = new QFormLayout();
	memRefLayout->addRow(tr("Memory map"), &memoryMapReferenceSelector_);

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->addLayout(memRefLayout);
	topLayout->addWidget(&bridges_);

	connect(&memoryMapReferenceSelector_, SIGNAL(memoryMapSelected(QString const&)),
		this, SLOT(onMemoryMapChange(QString const&)), Qt::UniqueConnection);
	connect(&bridges_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
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
	memoryMapReferenceSelector_.selectMemoryMap(memMapRef);

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
