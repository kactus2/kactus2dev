//-----------------------------------------------------------------------------
// File: TargetModeEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 29.06.2023
//
// Description:
// Editor to the target details of a bus interface.
//-----------------------------------------------------------------------------

#include "TargetModeEditor.h"

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
// Function: TargetModeEditor::TargetModeEditor()
//-----------------------------------------------------------------------------
TargetModeEditor::TargetModeEditor(BusInterfaceInterface* busInterface, std::string const& busName, Document::Revision revision, QWidget* parent):
    ModeEditorBase(busInterface, busName, tr("Target"), parent),
    memoryMapSelector_(tr("Memory map"), this),
    bridgesSelector_(tr("Transparent bridge"), this),
    memoryMapReferenceSelector_(this),
    targetBridges_(busInterface->getBridges(busName)),
    bridges_(busInterface, targetBridges_, revision, this),
    modeReferences_(tr("Modes"), this),
    fileSetRefs_(busInterface->getFileSetInterface(), tr("File set references"), this)
{
    fileSetRefs_.initialize();

    modeReferences_.initialize();

    memoryMapReferenceSelector_.setHidden(true);
    modeReferences_.setHidden(true);
    bridges_.setHidden(true);

    setupLayout();

    connect(&memoryMapSelector_, SIGNAL(clicked(bool)),
        this, SLOT(onMemoryMapSelected(bool)), Qt::UniqueConnection);
    connect(&memoryMapSelector_, SIGNAL(toggled(bool)), &memoryMapReferenceSelector_, SLOT(setVisible(bool)));
    connect(&memoryMapSelector_, SIGNAL(toggled(bool)), &modeReferences_, SLOT(setVisible(bool)));


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
// Function: TargetModeEditor::isValid()
//-----------------------------------------------------------------------------
bool TargetModeEditor::isValid() const
{
	return true;
}

//-----------------------------------------------------------------------------
// Function: TargetModeEditor::refresh()
//-----------------------------------------------------------------------------
void TargetModeEditor::refresh()
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


    bridges_.refresh();
    bridgesSelector_.setChecked(getBusInterface()->getBridgeInterface()->itemCount() != 0);
    
    setupFileSetReferences();
}

//-----------------------------------------------------------------------------
// Function: TargetModeEditor::getInterfaceMode()
//-----------------------------------------------------------------------------
General::InterfaceMode TargetModeEditor::getInterfaceMode() const
{
	return General::TARGET;
}

//-----------------------------------------------------------------------------
// Function: TargetModeEditor::onMemoryMapChange()
//-----------------------------------------------------------------------------
void TargetModeEditor::onMemoryMapChange(QString const& newMemoryMapName)
{
    getBusInterface()->setMemoryMapReference(getBusName(), newMemoryMapName.toStdString());

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: TargetModeEditor::onMemoryMapSelected()
//-----------------------------------------------------------------------------
void TargetModeEditor::onMemoryMapSelected(bool checked)
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
// Function: TargetModeEditor::onTransparentBridgeSelected()
//-----------------------------------------------------------------------------
void TargetModeEditor::onTransparentBridgeSelected(bool checked)
{
    BusInterfaceInterface* busInterface = getBusInterface();
    std::string busName = getBusName();

    if (checked)
    {
        if (busInterface->getMemoryMapReference(busName).empty() == false)
        {
            bridgesSelector_.setChecked(false);
        }
        else
        {
            memoryMapSelector_.setChecked(false);    
        }
        
        if (!targetBridges_)
        {
            targetBridges_ = busInterface->createBridges(busName);
            bridges_.setupBridges(targetBridges_);
        }
    }
    else if (!checked && busInterface->getBridgeInterface()->itemCount() > 0)
    {
        bridgesSelector_.setChecked(true);
    }
}

//-----------------------------------------------------------------------------
// Function: TargetModeEditor::onFileSetReferencesChanged()
//-----------------------------------------------------------------------------
void TargetModeEditor::onFileSetReferencesChanged()
{
    saveFileSetReferences();
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: TargetModeEditor::saveModeSpecific()
//-----------------------------------------------------------------------------
void TargetModeEditor::saveModeSpecific()
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
// Function: TargetModeEditor::setupFileSetReferences()
//-----------------------------------------------------------------------------
void TargetModeEditor::setupFileSetReferences()
{
    QStringList newFileSetItems;

    for (auto const& fileSetName : getBusInterface()->getFileSetReferences(getBusName()))
    {
        newFileSetItems.append(QString::fromStdString(fileSetName));
    }

    fileSetRefs_.setItems(newFileSetItems);
}

//-----------------------------------------------------------------------------
// Function: TargetModeEditor::saveFileSetReferences()
//-----------------------------------------------------------------------------
void TargetModeEditor::saveFileSetReferences()
{
    std::vector<std::string> newItems;
    for (auto fileSetReference : fileSetRefs_.items())
    {
        newItems.push_back(fileSetReference.toStdString());
    }

    getBusInterface()->setFileSetReferences(getBusName(), newItems);
}

//-----------------------------------------------------------------------------
// Function: TargetModeEditor::setupLayout()
//-----------------------------------------------------------------------------
void TargetModeEditor::setupLayout()
{
    auto accessSelectionGroup = new QGroupBox(tr("Interface access target"), this);
    accessSelectionGroup->setFlat(true);
    auto accessSelectionLayout = new QHBoxLayout(accessSelectionGroup);
    accessSelectionLayout->addWidget(&memoryMapSelector_, 0, Qt::AlignLeft);
    accessSelectionLayout->addWidget(&bridgesSelector_, 1, Qt::AlignLeft);


    QVBoxLayout* mapBridgeLayout = new QVBoxLayout();
    mapBridgeLayout->addWidget(&memoryMapReferenceSelector_);
    mapBridgeLayout->addWidget(&bridges_);
    mapBridgeLayout->addWidget(&modeReferences_);
    mapBridgeLayout->addStretch();

    auto topLayout = new QGridLayout(this);
    topLayout->addWidget(accessSelectionGroup, 0, 0, 1, 1);
    topLayout->addLayout(mapBridgeLayout, 1, 0, 1, 1);
    topLayout->addWidget(&fileSetRefs_, 0, 1, 2, 1);
    topLayout->setContentsMargins(0, 4, 0, 2);
}
