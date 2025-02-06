//-----------------------------------------------------------------------------
// File: RenodeFileEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 22.05.2023
//
// Description:
// Editor selecting files and file names for Renode generation.
//-----------------------------------------------------------------------------

#include "RenodeFileEditor.h"

#include <Plugins/RenodeGenerator/CPUDialog/RenodeFileSelectionGroup.h>
#include <Plugins/RenodeGenerator/CPUDialog/RenodeUtilities.h>

#include <QVBoxLayout>
#include <QFormLayout>

//-----------------------------------------------------------------------------
// Function: RenodeFileEditor::RenodeFileEditor()
//-----------------------------------------------------------------------------
RenodeFileEditor::RenodeFileEditor(QJsonObject const& configurationObject, QWidget *parent):
QWidget(parent),
fileSelectionGroup_(new RenodeFileSelectionGroup(configurationObject, this)),
cpuFileNameEditor_(new QLineEdit(this)),
memoryFileNameEditor_(new QLineEdit(this)),
peripheralFileNameEditor_(new QLineEdit(this))
{
    QFormLayout* lineEditLayout(new QFormLayout());
    lineEditLayout->addRow("CPU:", cpuFileNameEditor_);
    lineEditLayout->addRow("Memory:", memoryFileNameEditor_);
    lineEditLayout->addRow("Peripherals:", peripheralFileNameEditor_);

    QGroupBox* fileNameGroup(new QGroupBox("File names"));
    fileNameGroup->setLayout(lineEditLayout);

    QVBoxLayout* boxLayout(new QVBoxLayout());
    boxLayout->addWidget(fileSelectionGroup_, 1);
    boxLayout->addWidget(fileNameGroup);

    setLayout(boxLayout);

    applyConfigurations(configurationObject);
}

//-----------------------------------------------------------------------------
// Function: RenodeFileEditor::applyConfigurations()
//-----------------------------------------------------------------------------
void RenodeFileEditor::applyConfigurations(QJsonObject const& configurationObject)
{
    QJsonValue writeFilesValue = configurationObject.value(RenodeConstants::WRITEFILES);
    if (writeFilesValue.isObject())
    {
        QJsonObject writeFilesObject = writeFilesValue.toObject();

        cpuFileNameEditor_->setText(writeFilesObject.value(RenodeConstants::RENODECPUFILEIDENTIFIER).toString(""));
        memoryFileNameEditor_->setText(writeFilesObject.value(RenodeConstants::RENODEMEMORYFILEIDENTIFIER).toString(""));
        peripheralFileNameEditor_->setText(writeFilesObject.value(RenodeConstants::RENODEPERIPHERALFILEIDENTIFIER).toString(""));
    }
}

//-----------------------------------------------------------------------------
// Function: RenodeFileEditor::writeCpu()
//-----------------------------------------------------------------------------
bool RenodeFileEditor::writeCpu()
{
    return fileSelectionGroup_->writeCpu();
}

//-----------------------------------------------------------------------------
// Function: RenodeFileEditor::writeMemory()
//-----------------------------------------------------------------------------
bool RenodeFileEditor::writeMemory()
{
    return fileSelectionGroup_->writeMemory();
}

//-----------------------------------------------------------------------------
// Function: RenodeFileEditor::writePeripherals()
//-----------------------------------------------------------------------------
bool RenodeFileEditor::writePeripherals()
{
    return fileSelectionGroup_->writePeripherals();
}

//-----------------------------------------------------------------------------
// Function: RenodeFileEditor::changeFileNames()
//-----------------------------------------------------------------------------
void RenodeFileEditor::changeFileNames(QString const& newCpuName)
{
    cpuFileNameEditor_->setPlaceholderText(newCpuName + "_" + RenodeConstants::RENODECPUFILENAME);
    memoryFileNameEditor_->setPlaceholderText(newCpuName + "_" + RenodeConstants::RENODEMEMORYFILENAME);
    peripheralFileNameEditor_->setPlaceholderText(newCpuName + "_" + RenodeConstants::RENODEPERIPHERALFILENAME);
}

//-----------------------------------------------------------------------------
// Function: RenodeFileEditor::getCpuFileName()
//-----------------------------------------------------------------------------
QString RenodeFileEditor::getCpuFileName() const
{
    return getFileName(cpuFileNameEditor_);
}

//-----------------------------------------------------------------------------
// Function: RenodeFileEditor::getCpuEditorText()
//-----------------------------------------------------------------------------
QString RenodeFileEditor::getCpuEditorText() const
{
    return cpuFileNameEditor_->text();
}

//-----------------------------------------------------------------------------
// Function: RenodeFileEditor::getmemoryFileName()
//-----------------------------------------------------------------------------
QString RenodeFileEditor::getmemoryFileName() const
{
    return getFileName(memoryFileNameEditor_);
}

//-----------------------------------------------------------------------------
// Function: RenodeFileEditor::getMemoryEditorText()
//-----------------------------------------------------------------------------
QString RenodeFileEditor::getMemoryEditorText() const
{
    return memoryFileNameEditor_->text();
}

//-----------------------------------------------------------------------------
// Function: RenodeFileEditor::getPeripheralFileName()
//-----------------------------------------------------------------------------
QString RenodeFileEditor::getPeripheralFileName() const
{
    return getFileName(peripheralFileNameEditor_);
}

//-----------------------------------------------------------------------------
// Function: RenodeFileEditor::getPeripheralEditorText()
//-----------------------------------------------------------------------------
QString RenodeFileEditor::getPeripheralEditorText() const
{
    return peripheralFileNameEditor_->text();
}

//-----------------------------------------------------------------------------
// Function: RenodeFileEditor::getFileName()
//-----------------------------------------------------------------------------
template <typename fileEditor>
QString RenodeFileEditor::getFileName(fileEditor* editor) const
{
    QString fileName = editor->text();
    if (fileName.isEmpty())
    {
        fileName = editor->placeholderText();
    }

    return fileName;
}
