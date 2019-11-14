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

namespace
{
    QString const DEFAULT_FILEGROUP = QLatin1String("default");
};

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
bridges_(slave_->getBridges(), component, this),
fileSetRefs_(component, tr("File set references"), this)
{
    Q_ASSERT(slave_);

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
    if (getBusInterface()->getSlave())
    {
        slave_ = getBusInterface()->getSlave();
    }
	else 
    {
		slave_.clear();
		slave_ = QSharedPointer<SlaveInterface>(new SlaveInterface());
	}

    memoryMapReferenceSelector_.refresh(getComponent()->getMemoryMapNames());
    memoryMapReferenceSelector_.selectItem(slave_->getMemoryMapRef());
    memoryMapBox_->setChecked(!slave_->getMemoryMapRef().isEmpty());

    setupFileSetReferences();

    bridges_.refresh(slave_->getBridges());
    bridges_.setChecked(!slave_->getBridges()->isEmpty());
}

//-----------------------------------------------------------------------------
// Function: busifinterfaceslave::setupFileSetReferences()
//-----------------------------------------------------------------------------
void BusIfInterfaceSlave::setupFileSetReferences()
{
    QStringList newFileSetItems;

    if (slave_->getFileSetRefGroup())
    {
        for (auto fileGroup : *slave_->getFileSetRefGroup())
        {
            if (fileGroup->group_.compare(DEFAULT_FILEGROUP) == 0)
            {
                for (auto fileReference : fileGroup->fileSetRefs_)
                {
                    newFileSetItems.append(fileReference);
                }
            }
        }
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
// Function: busifinterfaceslave::onFileSetReferencesChanged()
//-----------------------------------------------------------------------------
void BusIfInterfaceSlave::onFileSetReferencesChanged()
{
    if (slave_->getFileSetRefGroup())
    {
        slave_->getFileSetRefGroup()->clear();
    }
    else
    {
        QSharedPointer<QList<QSharedPointer<SlaveInterface::FileSetRefGroup> > > newFileSetRefGroup
            (new QList<QSharedPointer<SlaveInterface::FileSetRefGroup> >());
        slave_->getFileSetRefGroup() = newFileSetRefGroup;
    }

    QStringList newItems = fileSetRefs_.items();
    if (!newItems.isEmpty())
    {
        QSharedPointer<SlaveInterface::FileSetRefGroup> fileGroup(new SlaveInterface::FileSetRefGroup());
        fileGroup->group_ = DEFAULT_FILEGROUP;

        fileGroup->fileSetRefs_ = fileSetRefs_.items();
        slave_->getFileSetRefGroup()->append(fileGroup);
    }

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusIfInterfaceSlave::saveModeSpecific()
//-----------------------------------------------------------------------------
void BusIfInterfaceSlave::saveModeSpecific()
{
    getBusInterface()->setSlave(slave_);
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
