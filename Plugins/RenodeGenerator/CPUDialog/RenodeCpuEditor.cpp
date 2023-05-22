//-----------------------------------------------------------------------------
// File: RenodeCpuEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 17.03.2023
//
// Description:
// Editor for CPU details in renode generation.
//-----------------------------------------------------------------------------

#include "RenodeCpuEditor.h"

#include <Plugins/RenodeGenerator/CPUDialog/RenodeCpuRoutesContainer.h>
#include <Plugins/RenodeGenerator/CPUDialog/RenodeMemoriesEditor.h>
#include <Plugins/RenodeGenerator/CPUDialog/RenodePeripheralsEditor.h>
#include <Plugins/RenodeGenerator/CPUDialog/RenodeUtilities.h>

#include <editors/MemoryDesigner/ConnectivityInterface.h>
#include <editors/MemoryDesigner/MemoryItem.h>

#include <KactusAPI/include/IPluginUtility.h>

#include <QVBoxLayout>
#include <QDir>
#include <QFormLayout>
#include <QLabel>

//-----------------------------------------------------------------------------
// Function: RenodeCpuEditor::RenodeCpuEditor()
//-----------------------------------------------------------------------------
RenodeCpuEditor::RenodeCpuEditor(IPluginUtility* utility, QJsonObject const& configurationObject, QWidget *parent /* = 0 */):
CPUEditor(parent),
utility_(utility),
availableCpuContainers_(),
renodeCPU_(),
peripheralEditor_(new RenodePeripheralsEditor(this)),
memoryEditor_(new RenodeMemoriesEditor(this)),
cpuSelector_(new QComboBox(this)),
cpuClassCombo_(new QComboBox(this)),
cpuTypeEditor_(new QLineEdit(this)),
cpuTimeProviderEditor_(new QLineEdit(this)),
configurationObject_(configurationObject)
{
    setupCpuClassEditor();

    cpuClassCombo_->setEditable(true);

    QFormLayout* cpuLayout(new QFormLayout());
    cpuLayout->addRow("CPU", cpuSelector_);
    cpuLayout->addRow("Class", cpuClassCombo_);
    cpuLayout->addRow("Architecture", cpuTypeEditor_);
    cpuLayout->addRow("Time provider", cpuTimeProviderEditor_);

    QLabel* cpuDetailsLabel = new QLabel("CPU details");
    QLabel* memoriesLabel = new QLabel("Memory");
    QLabel* peripheralsLabel = new QLabel("Peripherals");

    QFont summaryFont = memoriesLabel->font();
    summaryFont.setBold(true);

    cpuDetailsLabel->setFont(summaryFont);
    memoriesLabel->setFont(summaryFont);
    peripheralsLabel->setFont(summaryFont);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(cpuDetailsLabel, 0, Qt::AlignCenter);
    layout->addLayout(cpuLayout, 0);
    layout->addWidget(memoriesLabel, 0, Qt::AlignCenter);
    layout->addWidget(memoryEditor_, 1);
    layout->addWidget(peripheralsLabel, 0, Qt::AlignCenter);
    layout->addWidget(peripheralEditor_, 2);
    layout->setContentsMargins(0, 0, 0, 0);
}

//-----------------------------------------------------------------------------
// Function: RenodeCpuEditor::setupCpuClassEditor()
//-----------------------------------------------------------------------------
void RenodeCpuEditor::setupCpuClassEditor()
{
    cpuClassCombo_->addItem("");
    cpuClassCombo_->addItem("Arm");
    cpuClassCombo_->addItem("CortexA7");
    cpuClassCombo_->addItem("CortexM");
    cpuClassCombo_->addItem("CV32E40P");
    cpuClassCombo_->addItem("IbexRiscV32");
    cpuClassCombo_->addItem("Minerva");
    cpuClassCombo_->addItem("PicoRV32");
    cpuClassCombo_->addItem("PowerPc");
    cpuClassCombo_->addItem("PowerPc64");
    cpuClassCombo_->addItem("Ri5cy");
    cpuClassCombo_->addItem("RiscV32");
    cpuClassCombo_->addItem("RiscV64");
    cpuClassCombo_->addItem("Sparc");
    cpuClassCombo_->addItem("VexRiscv");
    cpuClassCombo_->addItem("X86");
    cpuClassCombo_->addItem("Xtensa");

    cpuClassCombo_->setCurrentText("");
}

//-----------------------------------------------------------------------------
// Function: RenodeCpuEditor::onHandleCpuChange()
//-----------------------------------------------------------------------------
void RenodeCpuEditor::onHandleCpuChange(QString const& newCPU)
{
    renodeCPU_ = getContainerForCpu(newCPU);
    setupEditorsForCpu();
}

//-----------------------------------------------------------------------------
// Function: RenodeCpuEditor::getContainerForCpu()
//-----------------------------------------------------------------------------
QSharedPointer<RenodeCpuRoutesContainer> RenodeCpuEditor::getContainerForCpu(QString const& cpuName) const
{
    for (auto container : availableCpuContainers_)
    {
        if (container->getCpu()->name() == cpuName)
        {
            return container;
        }
    }

    return QSharedPointer<RenodeCpuRoutesContainer>();
}

//-----------------------------------------------------------------------------
// Function: RenodeCpuEditor::onHandleClassChange()
//-----------------------------------------------------------------------------
void RenodeCpuEditor::onHandleClassChange(QString const& newClass)
{
    if (renodeCPU_)
    {
        renodeCPU_->setClassName(newClass);
    }
}

//-----------------------------------------------------------------------------
// Function: RenodeCpuEditor::onHandleTypeChange()
//-----------------------------------------------------------------------------
void RenodeCpuEditor::onHandleTypeChange(QString const& newType)
{
    if (renodeCPU_)
    {
        renodeCPU_->setCpuType(newType);
    }
}

//-----------------------------------------------------------------------------
// Function: RenodeCpuEditor::onHandleTimeProviderChange()
//-----------------------------------------------------------------------------
void RenodeCpuEditor::onHandleTimeProviderChange(QString const& newTimeProvider)
{
    if (renodeCPU_)
    {
        renodeCPU_->setTimeProvider(newTimeProvider);
    }
}

//-----------------------------------------------------------------------------
// Function: RenodeCpuEditor::setCPUDetails()
//-----------------------------------------------------------------------------
void RenodeCpuEditor::setupCPUDetails(LibraryInterface* library, QSharedPointer<Component> component, QString const& activeView)
{
    availableCpuContainers_ = RenodeUtilities::getRenodeCpuRoutes(configurationObject_, library, component, activeView);
    if (!availableCpuContainers_.isEmpty())
    {
        setupActiveCpuContainer();
        setupEditorsForCpu();
    }

    connect(cpuSelector_, SIGNAL(currentTextChanged(QString const&)), this, SLOT(onHandleCpuChange(QString const&)), Qt::UniqueConnection);
    connect(cpuClassCombo_, SIGNAL(currentTextChanged(QString const&)), this, SLOT(onHandleClassChange(QString const&)), Qt::UniqueConnection);
    connect(cpuTypeEditor_, SIGNAL(textEdited(QString const&)), this, SLOT(onHandleTypeChange(QString const&)), Qt::UniqueConnection);
    connect(cpuTimeProviderEditor_, SIGNAL(textEdited(QString const&)), this, SLOT(onHandleTimeProviderChange(QString const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: RenodeCpuEditor::setupActiveCpuContainer()
//-----------------------------------------------------------------------------
void RenodeCpuEditor::setupActiveCpuContainer()
{
    QString selectedCpu = configurationObject_.value(RenodeConstants::SINGLECPU).toString("");
    if (selectedCpu.isEmpty())
    {
        selectedCpu = availableCpuContainers_.first()->getCpu()->name();
    }

    for (auto cpuContainer : availableCpuContainers_)
    {
        cpuSelector_->addItem(cpuContainer->getCpu()->name());

        if (!renodeCPU_ && cpuContainer->getCpu()->name() == selectedCpu)
        {
            renodeCPU_ = cpuContainer;
        }
    }

    cpuSelector_->setCurrentText(selectedCpu);
}

//-----------------------------------------------------------------------------
// Function: RenodeCpuEditor::setupEditorsForCpu()
//-----------------------------------------------------------------------------
void RenodeCpuEditor::setupEditorsForCpu()
{
    checkStartingInterfacesForErrors();

    cpuClassCombo_->setCurrentText(renodeCPU_->getClassName());
    cpuTypeEditor_->setText(renodeCPU_->getCpuType());
    cpuTimeProviderEditor_->setText(renodeCPU_->getTimeProvider());

    peripheralEditor_->setupPeripherals(renodeCPU_->getPeripherals());
    memoryEditor_->setupMemories(renodeCPU_->getMemories());
}

//-----------------------------------------------------------------------------
// Function: RenodeCpuEditor::checkStartingInterfacesForErrors()
//-----------------------------------------------------------------------------
void RenodeCpuEditor::checkStartingInterfacesForErrors()
{
    if (renodeCPU_->getRoutes().isEmpty())
    {
        return;
    }

    QSharedPointer<const ConnectivityInterface> firstInterface = renodeCPU_->getRoutes().first()->cpuInterface_;

    QString firstAUB = firstInterface->getConnectedMemory()->getAUB();
    QString firstWidth = firstInterface->getConnectedMemory()->getWidth();
    QString firstSize = firstInterface->getConnectedMemory()->getSize();
    QString firstBase = firstInterface->getBaseAddress();

    for (int i = 1; i < renodeCPU_->getRoutes().size(); ++i)
    {
        QSharedPointer<const ConnectivityInterface> currentInterface = renodeCPU_->getRoutes().at(i)->cpuInterface_;

        QString currentAUB = currentInterface->getConnectedMemory()->getAUB();
        if (currentAUB != firstAUB)
        {
            utility_->printError("Mis-matching address unit bits found in " + renodeCPU_->getFileName() + " interface " + currentInterface->getName());
        }
        QString currentWidth = currentInterface->getConnectedMemory()->getWidth();
        if (currentWidth != firstWidth)
        {
            utility_->printError("Mis-matching width found in " + renodeCPU_->getFileName() + " interface " + currentInterface->getName());
        }
        QString currentSize = currentInterface->getConnectedMemory()->getSize();
        if (currentSize != firstSize)
        {
            utility_->printError("Mis-matching size found in " + renodeCPU_->getFileName() + " interface " + currentInterface->getName());
        }
        QString currentBase = currentInterface->getBaseAddress();
        if (currentBase != firstBase)
        {
            utility_->printError("Mis-matching base address found in " + renodeCPU_->getFileName()  + " interface " + currentInterface->getName());
        }

    }
}

//-----------------------------------------------------------------------------
// Function: RenodeCpuEditor::getSelectedCPUs()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<CpuRoutesContainer> > RenodeCpuEditor::getSelectedCPUs() const
{
    QVector<QSharedPointer<CpuRoutesContainer> > defaultCPUs;
    if (renodeCPU_)
    {
        defaultCPUs.append(renodeCPU_);
    }

    return defaultCPUs;
}

//-----------------------------------------------------------------------------
// Function: RenodeCpuEditor::getSelectedCpuName()
//-----------------------------------------------------------------------------
QString RenodeCpuEditor::getSelectedCpuName()
{
    return cpuSelector_->currentText();
}

//-----------------------------------------------------------------------------
// Function: RenodeCpuEditor::setupFolderPath()
//-----------------------------------------------------------------------------
void RenodeCpuEditor::setupFolderPath(QString const& newFolderPath)
{
    peripheralEditor_->setupFolderPath(newFolderPath);
}
