//-----------------------------------------------------------------------------
// File: busifinterfaceslave.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 7.4.2011
//
// Description:
// Editor to the slave details of a bus interface.
//-----------------------------------------------------------------------------

#include "busifinterfaceslave.h"

#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>

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
    memoryMapBox_(new QGroupBox(tr("Memory map"), this)),
    memoryMapReferenceSelector_(this),
    bridges_(slave_->getBridges(), component, this)
{
    Q_ASSERT(slave_);

    memoryMapBox_->setCheckable(true);
    bridges_.setCheckable(true);

    setupLayout();

    connect(memoryMapBox_, SIGNAL(clicked(bool)),
        this, SLOT(onMemoryMapSelected(bool)), Qt::UniqueConnection);
    connect(&bridges_, SIGNAL(clicked(bool)),
        this, SLOT(onTransparentBridgeSelected(bool)), Qt::UniqueConnection);

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
	// if the model contains slave-element
	if (busif_->getSlave())
    {
		slave_ = busif_->getSlave();
	}
	else 
    {
		slave_.clear();
		slave_ = QSharedPointer<SlaveInterface>(new SlaveInterface());
	}

	memoryMapReferenceSelector_.refresh(component_->getMemoryMapNames());
	memoryMapReferenceSelector_.selectItem(slave_->getMemoryMapRef());
    memoryMapBox_->setChecked(!slave_->getMemoryMapRef().isEmpty());

    bridges_.refresh(slave_->getBridges());
    bridges_.setChecked(!slave_->getBridges()->isEmpty());
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
// Function: BusIfInterfaceSlave::onMemoryMapSelected()
//-----------------------------------------------------------------------------
void BusIfInterfaceSlave::onMemoryMapSelected(bool checked)
{
    if (checked)
    {
        if (!slave_->getBridges()->isEmpty())
        {
            memoryMapBox_->setChecked(false);
        }
        else
        {
            bridges_.setChecked(false);
        }
    }
    else if (!checked && !slave_->getMemoryMapRef().isEmpty())
    {
        memoryMapBox_->setChecked(true);
    }
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceSlave::onTransparentBridgeSelected()
//-----------------------------------------------------------------------------
void BusIfInterfaceSlave::onTransparentBridgeSelected(bool checked)
{
    if (checked)
    {
        if (!slave_->getMemoryMapRef().isEmpty())
        {
            bridges_.setChecked(false);
        }
        else
        {
            memoryMapBox_->setChecked(false);
        }
    }
    else if (!checked && !slave_->getBridges()->isEmpty())
    {
        bridges_.setChecked(true);
    }
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceSlave::saveModeSpecific()
//-----------------------------------------------------------------------------
void BusIfInterfaceSlave::saveModeSpecific()
{
	busif_->setSlave(slave_);
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceSlave::setupLayout()
//-----------------------------------------------------------------------------
void BusIfInterfaceSlave::setupLayout()
{
    QHBoxLayout* memRefLayout = new QHBoxLayout(memoryMapBox_);
    memRefLayout->addWidget(&memoryMapReferenceSelector_, 1);

    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addWidget(memoryMapBox_);
    topLayout->addWidget(&bridges_);
    topLayout->addStretch();
}

