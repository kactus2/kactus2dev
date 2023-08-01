//-----------------------------------------------------------------------------
// File: SlaveModeEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 7.4.2011
//
// Description:
// Editor to the slave details of a bus interface.
//-----------------------------------------------------------------------------

#include "SlaveModeEditor.h"

#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>

#include <KactusAPI/include/BusInterfaceInterface.h>
#include <KactusAPI/include/TransparentBridgeInterface.h>
#include <KactusAPI/include/MemoryMapInterface.h>
#include <KactusAPI/include/FileSetInterface.h>

#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

namespace
{
    QString const DEFAULT_FILEGROUP = QLatin1String("default");
};

//-----------------------------------------------------------------------------
// Function: SlaveModeEditor::SlaveModeEditor()
//-----------------------------------------------------------------------------
SlaveModeEditor::SlaveModeEditor(BusInterfaceInterface* busInterface, std::string const& busName,
    QWidget* parent):
ModeEditorBase(busInterface, busName, tr("Slave"), parent),
memoryMapSelector_(tr("Memory map"), this),
bridgesSelector_(tr("Transparent bridge"), this),
memoryMapReferenceSelector_(this),
slaveBridges_(busInterface->getBridges(busName)),
bridges_(busInterface, slaveBridges_, this),
fileSetRefs_(busInterface->getFileSetInterface(), tr("File set references"), this)
{
    fileSetRefs_.initialize();

    memoryMapReferenceSelector_.setHidden(true);
    bridges_.setHidden(true);

    setupLayout();

    connect(&memoryMapSelector_, SIGNAL(clicked(bool)),
        this, SLOT(onMemoryMapSelected(bool)), Qt::UniqueConnection);
    connect(&memoryMapSelector_, SIGNAL(toggled(bool)), &memoryMapReferenceSelector_, SLOT(setVisible(bool)));

    connect(&bridgesSelector_, SIGNAL(clicked(bool)),
        this, SLOT(onTransparentBridgeSelected(bool)), Qt::UniqueConnection);
    connect(&bridgesSelector_, SIGNAL(toggled(bool)), &bridges_, SLOT(setVisible(bool)));

	connect(&memoryMapReferenceSelector_, SIGNAL(itemSelected(QString const&)),
		this, SLOT(onMemoryMapChange(QString const&)), Qt::UniqueConnection);
	
    connect(&fileSetRefs_, SIGNAL(contentChanged()),
        this, SLOT(onFileSetReferencesChanged()), Qt::UniqueConnection);

    connect(&bridges_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: SlaveModeEditor::isValid()
//-----------------------------------------------------------------------------
bool SlaveModeEditor::isValid() const
{
	return true;
}

//-----------------------------------------------------------------------------
// Function: SlaveModeEditor::refresh()
//-----------------------------------------------------------------------------
void SlaveModeEditor::refresh()
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
    memoryMapSelector_.setChecked(mapReference.isEmpty() == false);

    setupFileSetReferences();

    bridges_.refresh();
    bridgesSelector_.setChecked(getBusInterface()->getBridgeInterface()->itemCount() != 0);
}


//-----------------------------------------------------------------------------
// Function: SlaveModeEditor::getInterfaceMode()
//-----------------------------------------------------------------------------
General::InterfaceMode SlaveModeEditor::getInterfaceMode() const
{
	return General::SLAVE;
}

//-----------------------------------------------------------------------------
// Function: SlaveModeEditor::onMemoryMapChange()
//-----------------------------------------------------------------------------
void SlaveModeEditor::onMemoryMapChange(QString const& newMemoryMapName)
{
    getBusInterface()->setMemoryMapReference(getBusName(), newMemoryMapName.toStdString());

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SlaveModeEditor::onMemoryMapSelected()
//-----------------------------------------------------------------------------
void SlaveModeEditor::onMemoryMapSelected(bool checked)
{
    BusInterfaceInterface* busInterface = getBusInterface();

    if (checked)
    {
        if (busInterface->getBridgeInterface()->itemCount() > 0)
        {
            memoryMapSelector_.setChecked(false);
        }
        else
        {
            bridgesSelector_.setChecked(false);
        }
    }
    else if (!checked && !busInterface->getMemoryMapReference(getBusName()).empty())
    {
        memoryMapSelector_.setChecked(true);
    }
}

//-----------------------------------------------------------------------------
// Function: SlaveModeEditor::onTransparentBridgeSelected()
//-----------------------------------------------------------------------------
void SlaveModeEditor::onTransparentBridgeSelected(bool checked)
{
    BusInterfaceInterface* busInterface = getBusInterface();
    std::string busName = getBusName();

    if (checked)
    {
        if (!QString::fromStdString(busInterface->getMemoryMapReference(busName)).isEmpty())
        {
            bridgesSelector_.setChecked(false);
        }
        else
        {
            memoryMapSelector_.setChecked(false);
        }

        if (!slaveBridges_)
        {
            slaveBridges_ = busInterface->createBridges(busName);
            bridges_.setupBridges(slaveBridges_);
        }
    }
    else if (!checked && busInterface->getBridgeInterface()->itemCount() > 0)
    {
        bridgesSelector_.setChecked(true);
    }
}

//-----------------------------------------------------------------------------
// Function: SlaveModeEditor::onFileSetReferencesChanged()
//-----------------------------------------------------------------------------
void SlaveModeEditor::onFileSetReferencesChanged()
{
    saveFileSetReferences();
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SlaveModeEditor::saveModeSpecific()
//-----------------------------------------------------------------------------
void SlaveModeEditor::saveModeSpecific()
{
    saveFileSetReferences();

    if (memoryMapSelector_.isChecked())
    {
        getBusInterface()->setMemoryMapReference(getBusName(), memoryMapReferenceSelector_.currentText().toStdString());
    }
    else if (bridgesSelector_.isChecked())
    {
        bridges_.refresh();
    }
}

//-----------------------------------------------------------------------------
// Function: SlaveModeEditor::setupFileSetReferences()
//-----------------------------------------------------------------------------
void SlaveModeEditor::setupFileSetReferences()
{
    QStringList newFileSetItems;

    for (auto const& fileSetName : getBusInterface()->getFileSetReferences(getBusName()))
    {
        newFileSetItems.append(QString::fromStdString(fileSetName));
    }

    fileSetRefs_.setItems(newFileSetItems);
}

//-----------------------------------------------------------------------------
// Function: SlaveModeEditor::saveFileSetReferences()
//-----------------------------------------------------------------------------
void SlaveModeEditor::saveFileSetReferences()
{
    std::vector<std::string> newItems;
    for (auto fileSetReference : fileSetRefs_.items())
    {
        newItems.push_back(fileSetReference.toStdString());
    }

    getBusInterface()->setFileSetReferences(getBusName(), newItems);
}

//-----------------------------------------------------------------------------
// Function: SlaveModeEditor::setupLayout()
//-----------------------------------------------------------------------------
void SlaveModeEditor::setupLayout()
{
    auto accessSelectionGroup = new QGroupBox(tr("Interface access target"), this);
    accessSelectionGroup->setFlat(true);

    auto accessSelectionLayout = new QHBoxLayout(accessSelectionGroup);
    accessSelectionLayout->addWidget(&memoryMapSelector_, 0, Qt::AlignLeft);
    accessSelectionLayout->addWidget(&bridgesSelector_, 1, Qt::AlignLeft);

    QVBoxLayout* mapBridgeLayout = new QVBoxLayout();
    mapBridgeLayout->addWidget(&memoryMapReferenceSelector_);
    mapBridgeLayout->addWidget(&bridges_);
    mapBridgeLayout->addStretch();

    auto topLayout = new QGridLayout(this);
    topLayout->addWidget(accessSelectionGroup, 0, 0, 1, 1);
    topLayout->addLayout(mapBridgeLayout, 1, 0, 1, 1);
    topLayout->addWidget(&fileSetRefs_, 0, 1, 2, 1);
    topLayout->setContentsMargins(0, 4, 0, 2);
}
