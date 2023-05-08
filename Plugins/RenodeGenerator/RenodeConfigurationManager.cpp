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
PluginConfigurationManager(utility)
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
        return configurationDocument.object();
    }

    return QJsonObject();
}

//-----------------------------------------------------------------------------
// Function: RenodeConfigurationManager::createConfigureFile()
//-----------------------------------------------------------------------------
void RenodeConfigurationManager::createConfigureFile(QVector<QSharedPointer<RenodeCPUDetailRoutes>> renodeData,
    QString const& selectedView, bool saveToFileSet, QString const& selectedFileSet, QString const& folderPath,
    bool writeCPU, bool writeMemory, bool writePeripherals, QSharedPointer<Component> topComponent)
{
    QFile configurationFile(getPathToConfigurationFile(topComponent, RenodeConstants::CONFIGURATIONFILEEXTENSION));
    if (!configurationFile.open(QIODevice::WriteOnly))
    {
        return;
    }

    QJsonDocument document = createJsonDocument(renodeData, selectedView, saveToFileSet, selectedFileSet, folderPath, writeCPU, writeMemory, writePeripherals);
    configurationFile.write(document.toJson());

    writeModelToFile(topComponent);
}

//-----------------------------------------------------------------------------
// Function: RenodeConfigurationManager::createJsonDocument()
//-----------------------------------------------------------------------------
QJsonDocument RenodeConfigurationManager::createJsonDocument(QVector<QSharedPointer<RenodeCPUDetailRoutes>> renodeData,
    QString const& selectedView, bool saveToFileSetFlag, QString const& selectedFileSet, QString const& folderPath,
    bool writeCPU, bool writeMemory, bool writePeripherals)
{
    QJsonObject configurationObject;

    if (!renodeData.isEmpty())
    {
        QJsonArray renodeArray;

        for (auto renodeCPU : renodeData)
        {
            QJsonObject cpuObject;

            cpuObject.insert(RenodeConstants::CPUNAME, renodeCPU->getCPUName());
            cpuObject.insert(RenodeConstants::CPUCLASS, renodeCPU->getClassName());
            cpuObject.insert(RenodeConstants::CPUTYPE, renodeCPU->getCpuType());
            cpuObject.insert(RenodeConstants::CPUTIME, renodeCPU->getTimeProvider());

            if (!renodeCPU->getMemories().isEmpty())
            {
                QJsonArray memoryArray;

                for (auto memory : renodeCPU->getMemories())
                {
                    QJsonObject memoryObject;
                    memoryObject.insert(RenodeConstants::MEMORYNAME, memory->memoryName_);
                    memoryObject.insert(RenodeConstants::MEMORYCLASS, memory->className_);

                    memoryArray.push_back(memoryObject);
                }

                cpuObject.insert(RenodeConstants::MEMORY, memoryArray);
            }

            if (!renodeCPU->getPeripherals().isEmpty())
            {
                QJsonArray peripheralArray;

                for (auto peripheral : renodeCPU->getPeripherals())
                {
                    QJsonObject peripheralObject;

                    peripheralObject.insert(RenodeConstants::PERIPHERALNAME, peripheral->peripheralName_);
                    peripheralObject.insert(RenodeConstants::PERIPHERALCLASS, peripheral->className_);
                    peripheralObject.insert(RenodeConstants::PERIPHERALINITABLE, peripheral->initable_);
                    peripheralObject.insert(RenodeConstants::PERIPHERALPATH, peripheral->filePath_);

                    peripheralArray.push_back(peripheralObject);
                }

                cpuObject.insert(RenodeConstants::PERIPHERALS, peripheralArray);
            }

            renodeArray.push_back(cpuObject);
        }

        configurationObject.insert(RenodeConstants::CPUS, renodeArray);
    }

    configurationObject.insert(RenodeConstants::VIEW, selectedView);
    configurationObject.insert(RenodeConstants::FOLDERPATH, folderPath);
    configurationObject.insert(RenodeConstants::SAVETOFILESET, saveToFileSetFlag);
    configurationObject.insert(RenodeConstants::FILESET, selectedFileSet);

    QJsonObject writeFlagsObject;
    writeFlagsObject.insert(RenodeConstants::SINGLECPU, writeCPU);
    writeFlagsObject.insert(RenodeConstants::MEMORY, writeMemory);
    writeFlagsObject.insert(RenodeConstants::PERIPHERALS, writePeripherals);

    configurationObject.insert(RenodeConstants::WRITEFILES, writeFlagsObject);

    QJsonDocument document(configurationObject);
    return document;
}