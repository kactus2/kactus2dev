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

#include <editors/ComponentEditor/busInterfaces/interfaces/BusInterfaceInterface.h>
#include <editors/ComponentEditor/busInterfaces/interfaces/TransparentBridgeInterface.h>
#include <editors/ComponentEditor/memoryMaps/interfaces/MemoryMapInterface.h>
#include <editors/ComponentEditor/fileSet/interfaces/FileSetInterface.h>

#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

namespace
{
    QString const DEFAULT_FILEGROUP = QLatin1String("default");
};

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceSlave::BusIfInterfaceSlave()
//-----------------------------------------------------------------------------
BusIfInterfaceSlave::BusIfInterfaceSlave(BusInterfaceInterface* busInterface, std::string const& busName,
    QWidget* parent):
BusIfInterfaceModeEditor(busInterface, busName, tr("Slave"), parent),
memoryMapBox_(new QGroupBox(tr("Memory map"), this)),
memoryMapReferenceSelector_(this),
slaveBridges_(busInterface->getBridges(busName)),
bridges_(busInterface, slaveBridges_, this),
fileSetRefs_(busInterface->getFileSetInterface(), tr("File set references"), this)
{
    fileSetRefs_.initialize();

    memoryMapBox_->setCheckable(true);
    bridges_.setCheckable(true);

    setupLayout();

    connect(memoryMapBox_, SIGNAL(clicked(bool)),
        this, SLOT(onMemoryMapSelected(bool)), Qt::UniqueConnection);
    connect(&bridges_, SIGNAL(clicked(bool)),
        this, SLOT(onTransparentBridgeSelected(bool)), Qt::UniqueConnection);

	connect(&memoryMapReferenceSelector_, SIGNAL(itemSelected(QString const&)),
		this, SLOT(onMemoryMapChange(QString const&)), Qt::UniqueConnection);
	
    connect(&fileSetRefs_, SIGNAL(contentChanged()),
        this, SLOT(onFileSetReferencesChanged()), Qt::UniqueConnection);

    connect(&bridges_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
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
    BusInterfaceInterface* busInterface = getBusInterface();
    std::string busName = getBusName();

    QStringList mapNameList;
    for (auto mapName : busInterface->getMemoryMapInterface()->getItemNames())
    {
        mapNameList.append(QString::fromStdString(mapName));
    }

    QString mapReference = QString::fromStdString(busInterface->getMemoryMapReference(busName));

    memoryMapReferenceSelector_.refresh(mapNameList);
    memoryMapReferenceSelector_.selectItem(mapReference);
    memoryMapBox_->setChecked(!mapReference.isEmpty());

    setupFileSetReferences();

    bridges_.refresh();
    bridges_.setChecked(getBusInterface()->getBridgeInterface()->itemCount() != 0);
}

//-----------------------------------------------------------------------------
// Function: busifinterfaceslave::setupFileSetReferences()
//-----------------------------------------------------------------------------
void BusIfInterfaceSlave::setupFileSetReferences()
{
    QStringList newFileSetItems;

    for (auto fileSetName : getBusInterface()->getFileSetReferences(getBusName()))
    {
        newFileSetItems.append(QString::fromStdString(fileSetName));
    }

    fileSetRefs_.setItems(newFileSetItems);
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
    getBusInterface()->setMemoryMapReference(getBusName(), newMemoryMapName.toStdString());

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceSlave::onMemoryMapSelected()
//-----------------------------------------------------------------------------
void BusIfInterfaceSlave::onMemoryMapSelected(bool checked)
{
    BusInterfaceInterface* busInterface = getBusInterface();

    if (checked)
    {
        if (busInterface->getBridgeInterface()->itemCount() > 0)
        {
            memoryMapBox_->setChecked(false);
        }
        else
        {
            bridges_.setChecked(false);
        }
    }
    else if (!checked && !QString::fromStdString(busInterface->getMemoryMapReference(getBusName())).isEmpty())
    {
        memoryMapBox_->setChecked(true);
    }
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceSlave::onTransparentBridgeSelected()
//-----------------------------------------------------------------------------
void BusIfInterfaceSlave::onTransparentBridgeSelected(bool checked)
{
    BusInterfaceInterface* busInterface = getBusInterface();
    std::string busName = getBusName();

    if (checked)
    {
        if (!QString::fromStdString(busInterface->getMemoryMapReference(busName)).isEmpty())
        {
            bridges_.setChecked(false);
        }
        else
        {
            memoryMapBox_->setChecked(false);
        }

        if (!slaveBridges_)
        {
            slaveBridges_ = busInterface->createBridges(busName);
            bridges_.setupBridges(slaveBridges_);
        }
    }
    else if (!checked && busInterface->getBridgeInterface()->itemCount() > 0)
    {
        bridges_.setChecked(true);
    }
}

//-----------------------------------------------------------------------------
// Function: busifinterfaceslave::onFileSetReferencesChanged()
//-----------------------------------------------------------------------------
void BusIfInterfaceSlave::onFileSetReferencesChanged()
{
    std::vector<std::string> newItems;
    for (auto fileSetReference : fileSetRefs_.items())
    {
        newItems.push_back(fileSetReference.toStdString());
    }

    getBusInterface()->setFileSetReferences(getBusName(), newItems);

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceSlave::saveModeSpecific()
//-----------------------------------------------------------------------------
void BusIfInterfaceSlave::saveModeSpecific()
{
    onFileSetReferencesChanged();

    if (memoryMapBox_->isChecked())
    {
        onMemoryMapChange(memoryMapReferenceSelector_.currentText());
    }
    else if (bridges_.isChecked())
    {
        bridges_.refresh();
    }
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceSlave::setupLayout()
//-----------------------------------------------------------------------------
void BusIfInterfaceSlave::setupLayout()
{
    QHBoxLayout* memRefLayout = new QHBoxLayout(memoryMapBox_);
    memRefLayout->addWidget(&memoryMapReferenceSelector_, 1);

    QVBoxLayout* mapBridgeLayout = new QVBoxLayout();
    mapBridgeLayout->addWidget(memoryMapBox_);
    mapBridgeLayout->addWidget(&bridges_);
    mapBridgeLayout->addStretch();

    QHBoxLayout* topLayout = new QHBoxLayout(this);
    topLayout->addLayout(mapBridgeLayout);
    topLayout->addWidget(&fileSetRefs_);
}
