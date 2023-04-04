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

#include <Plugins/RenodeGenerator/CPUDialog/RenodeCPUDetailRoutes.h>
#include <Plugins/RenodeGenerator/CPUDialog/RenodeMemoriesEditor.h>
#include <Plugins/RenodeGenerator/CPUDialog/RenodePeripheralsEditor.h>
#include <Plugins/RenodeGenerator/CPUDialog/RenodeUtilities.h>

#include <QVBoxLayout>
#include <QDir>
#include <QFormLayout>
#include <QLabel>

//-----------------------------------------------------------------------------
// Function: RenodeCpuEditor::RenodeCpuEditor()
//-----------------------------------------------------------------------------
RenodeCpuEditor::RenodeCpuEditor(QWidget *parent):
CPUEditor(parent),
renodeCPU_(),
peripheralEditor_(new RenodePeripheralsEditor(this)),
memoryEditor_(new RenodeMemoriesEditor(this)),
cpuClassCombo_(new QComboBox(this)),
cpuTypeEditor_(new QLineEdit(this)),
cpuTimeProviderEditor_(new QLineEdit(this))
{
    setupCpuClassEditor();

    cpuClassCombo_->setEditable(true);

    QFormLayout* cpuLayout(new QFormLayout());
    cpuLayout->addRow("Class", cpuClassCombo_);
    cpuLayout->addRow("Type", cpuTypeEditor_);
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
    QVector<QSharedPointer<RenodeCPUDetailRoutes> > cpuDetails = RenodeUtilities::getRenodeCpuRoutes(library, component, activeView);
    renodeCPU_ = cpuDetails.first();

    cpuClassCombo_->setCurrentText(renodeCPU_->getClassName());
    cpuTypeEditor_->setText(renodeCPU_->getCpuType());
    cpuTimeProviderEditor_->setText(renodeCPU_->getTimeProvider());

    peripheralEditor_->setupPeripherals(renodeCPU_->getPeripherals());
    memoryEditor_->setupMemories(renodeCPU_->getMemories());

    connect(cpuClassCombo_, SIGNAL(currentTextChanged(QString const&)), this, SLOT(onHandleClassChange(QString const&)), Qt::UniqueConnection);
    connect(cpuTypeEditor_, SIGNAL(textEdited(QString const&)), this, SLOT(onHandleTypeChange(QString const&)), Qt::UniqueConnection);
    connect(cpuTimeProviderEditor_, SIGNAL(textEdited(QString const&)), this, SLOT(onHandleTimeProviderChange(QString const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: RenodeCpuEditor::getSelectedCPUs()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<CPUDetailRoutes> > RenodeCpuEditor::getSelectedCPUs() const
{
    QVector<QSharedPointer<CPUDetailRoutes> > defaultCPUs;
    defaultCPUs.append(renodeCPU_);

    return defaultCPUs;
}

//-----------------------------------------------------------------------------
// Function: RenodeCpuEditor::setupFolderPath()
//-----------------------------------------------------------------------------
void RenodeCpuEditor::setupFolderPath(QString const& newFolderPath)
{
    peripheralEditor_->setupFolderPath(newFolderPath);
}
