//-----------------------------------------------------------------------------
// File: RenodeConfigurationManager.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 21.04.2023
//
// Description:
// Manager for storing and restoring renode editor configurations.
//-----------------------------------------------------------------------------

#include "RenodeConfigurationManager.h"

#include <Plugins/RenodeGenerator/CPUDialog/RenodeUtilities.h>

#include <IPXACTmodels/Component/File.h>

#include <QJsonArray>

//-----------------------------------------------------------------------------
// Function: RenodeConfigurationManager::RenodeConfigurationManager()
//-----------------------------------------------------------------------------
RenodeConfigurationManager::RenodeConfigurationManager(IPluginUtility* utility) :
PluginConfigurationManager(utility),
cpuArray_()
{

}

//-----------------------------------------------------------------------------
// Function: RenodeConfigurationManager::getConfigurationObject()
//-----------------------------------------------------------------------------
QJsonObject RenodeConfigurationManager::getConfigurationObject(QSharedPointer<Component> component)
{
    QJsonDocument configurationDocument = getConfigurationDocument(component, RenodeConstants::CONFIGURATIONFILEEXTENSION);
    if (configurationDocument.isObject())
    {
        QJsonObject configurationObject = configurationDocument.object();

        QJsonValue multiCpuValue = configurationObject.value(RenodeConstants::CPUS);
        if (multiCpuValue.isArray())
        {
            cpuArray_ = multiCpuValue.toArray();
        }

        return configurationObject;
    }

    return QJsonObject();
}

//-----------------------------------------------------------------------------
// Function: RenodeConfigurationManager::createConfigureFile()
//-----------------------------------------------------------------------------
void RenodeConfigurationManager::createConfigureFile(QSharedPointer<RenodeCpuRoutesContainer> renodeData,
    QString const& selectedView, bool saveToFileSet, QString const& selectedFileSet, QString const& folderPath,
    bool writeCPU, QString const& cpuFileName, bool writeMemory, QString const& memoryFileName, bool writePeripherals,
    QString const& peripheralsFileName, QString const& selectedCpu, QSharedPointer<Component> topComponent)
{
    QFile configurationFile(getPathToConfigurationFile(topComponent, RenodeConstants::CONFIGURATIONFILEEXTENSION));
    if (!configurationFile.open(QIODevice::WriteOnly))
    {
        return;
    }

    QJsonDocument document = createJsonDocument(renodeData, selectedView, saveToFileSet, selectedFileSet, folderPath,
        writeCPU, cpuFileName, writeMemory, memoryFileName, writePeripherals, peripheralsFileName, selectedCpu);
    configurationFile.write(document.toJson());

    writeModelToFile(topComponent);
}

//-----------------------------------------------------------------------------
// Function: RenodeConfigurationManager::createJsonDocument()
//-----------------------------------------------------------------------------
QJsonDocument RenodeConfigurationManager::createJsonDocument(QSharedPointer<RenodeCpuRoutesContainer> renodeCpu,
    QString const& selectedView, bool saveToFileSetFlag, QString const& selectedFileSet, QString const& folderPath,
    bool writeCPU, QString const& cpuFileName, bool writeMemory, QString const& memoryFileName, bool writePeripherals,
    QString const& peripheralsFileName, QString const& selectedCpu)
{
    QJsonObject configurationObject;

    removeCpuContainerFromConfigurationArray(renodeCpu);

    QJsonObject cpuObject;

    cpuObject.insert(RenodeConstants::CPUNAME, renodeCpu->getFileName());
    cpuObject.insert(RenodeConstants::CPUCLASS, renodeCpu->getClassName());
    cpuObject.insert(RenodeConstants::CPUTYPE, renodeCpu->getCpuType());
    cpuObject.insert(RenodeConstants::CPUTIME, renodeCpu->getTimeProvider());

    if (!renodeCpu->getMemories().isEmpty())
    {
        QJsonArray memoryArray;

        for (auto memory : renodeCpu->getMemories())
        {
            QJsonObject memoryObject;
            memoryObject.insert(RenodeConstants::MEMORYNAME, memory->memoryName_);
            memoryObject.insert(RenodeConstants::MEMORYCLASS, memory->className_);

            memoryArray.push_back(memoryObject);
        }

        cpuObject.insert(RenodeConstants::MEMORY, memoryArray);
    }

    if (!renodeCpu->getPeripherals().isEmpty())
    {
        QJsonArray peripheralArray;

        for (auto peripheral : renodeCpu->getPeripherals())
        {
            QJsonObject peripheralObject;

            peripheralObject.insert(RenodeConstants::PERIPHERALNAME, peripheral->peripheralName_);
            peripheralObject.insert(RenodeConstants::PERIPHERALCLASS, peripheral->className_);
            peripheralObject.insert(RenodeConstants::PERIPHERALINITABLE, peripheral->initable_);
            peripheralObject.insert(RenodeConstants::PERIPHERALPATH, peripheral->filePath_);

            if (peripheral->template_)
            {
                peripheralObject.insert(RenodeConstants::PERIPHERALTEMPLATE, peripheral->template_->identifier_);
            }

            peripheralArray.push_back(peripheralObject);
        }

        cpuObject.insert(RenodeConstants::PERIPHERALS, peripheralArray);
    }

    cpuArray_.push_back(cpuObject);

    configurationObject.insert(RenodeConstants::CPUS, cpuArray_);

    configurationObject.insert(RenodeConstants::SINGLECPU, selectedCpu);
    configurationObject.insert(RenodeConstants::VIEW, selectedView);
    configurationObject.insert(RenodeConstants::FOLDERPATH, folderPath);
    configurationObject.insert(RenodeConstants::SAVETOFILESET, saveToFileSetFlag);
    configurationObject.insert(RenodeConstants::FILESET, selectedFileSet);

    QJsonObject writeFlagsObject;
    writeFlagsObject.insert(RenodeConstants::SINGLECPU, writeCPU);
    writeFlagsObject.insert(RenodeConstants::MEMORY, writeMemory);
    writeFlagsObject.insert(RenodeConstants::PERIPHERALS, writePeripherals);

    if (!cpuFileName.isEmpty())
    {
        writeFlagsObject.insert(RenodeConstants::RENODECPUFILEIDENTIFIER, cpuFileName);
    }
    if (!memoryFileName.isEmpty())
    {
        writeFlagsObject.insert(RenodeConstants::RENODEMEMORYFILEIDENTIFIER, memoryFileName);
    }
    if (!peripheralsFileName.isEmpty())
    {
        writeFlagsObject.insert(RenodeConstants::RENODEPERIPHERALFILEIDENTIFIER, peripheralsFileName);
    }

    configurationObject.insert(RenodeConstants::WRITEFILES, writeFlagsObject);

    QJsonDocument document(configurationObject);
    return document;
}

//-----------------------------------------------------------------------------
// Function: RenodeConfigurationManager::removeCpuContainerFromConfigurationArray()
//-----------------------------------------------------------------------------
void RenodeConfigurationManager::removeCpuContainerFromConfigurationArray(QSharedPointer<RenodeCpuRoutesContainer> cpuContainer)
{
    for (int i = 0; i < cpuArray_.size(); ++i)
    {
        auto configurationCpu = cpuArray_.at(i);
        if (configurationCpu.isObject())
        {
            QJsonObject arraydCpuObject = configurationCpu.toObject();
            if (arraydCpuObject.value(RenodeConstants::CPUNAME).toString() == cpuContainer->getFileName())
            {
                cpuArray_.removeAt(i);
                return;
            }
        }
    }
}
