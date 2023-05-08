//-----------------------------------------------------------------------------
// File: SVDConfigurationManager.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 25.04.2023
//
// Description:
// Manager for storing and restoring SVD editor configurations.
//-----------------------------------------------------------------------------

#include "SVDConfigurationManager.h"

#include <Plugins/SVDGenerator/CPUDialog/SVDUtilities.h>
#include <Plugins/SVDGenerator/CPUDialog/SVDCPUDetailRoutes.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/File.h>

#include <LibraryInterface.h>

#include <QJsonArray>

//-----------------------------------------------------------------------------
// Function: SVDConfigurationManager::SVDConfigurationManager()
//-----------------------------------------------------------------------------
SVDConfigurationManager::SVDConfigurationManager(IPluginUtility* utility) :
PluginConfigurationManager(utility)
{

}

//-----------------------------------------------------------------------------
// Function: SVDConfigurationManager::getConfigurationObject()
//-----------------------------------------------------------------------------
QJsonObject SVDConfigurationManager::getConfigurationObject(QSharedPointer<Component> component)
{
    QJsonDocument configurationDocument = getConfigurationDocument(component, SVDConstants::CONFIGURATIONFILEEXTENSION);
    if (configurationDocument.isObject())
    {
        return configurationDocument.object();
    }

    return QJsonObject();
}

//-----------------------------------------------------------------------------
// Function: SVDConfigurationManager::createConfigureFile()
//-----------------------------------------------------------------------------
void SVDConfigurationManager::createConfigureFile(QVector<QSharedPointer<SVDCPUDetailRoutes>> svdData,
    QSharedPointer<Component> topComponent, QString const& selectedView, bool saveToFileSet,
    QString const& selectedFileSet, QString const& folderPath)
{
    QFile configurationFile(getPathToConfigurationFile(topComponent, SVDConstants::CONFIGURATIONFILEEXTENSION));
    if (!configurationFile.open(QIODevice::WriteOnly))
    {
        return;
    }

    QJsonDocument document = createJsonDocument(svdData, selectedView, saveToFileSet, selectedFileSet, folderPath);
    configurationFile.write(document.toJson());

    writeModelToFile(topComponent);
}

//-----------------------------------------------------------------------------
// Function: SVDConfigurationManager::createJsonDocument()
//-----------------------------------------------------------------------------
QJsonDocument SVDConfigurationManager::createJsonDocument(QVector<QSharedPointer<SVDCPUDetailRoutes>> svdData,
    QString const& selectedView, bool saveToFileSetFlag,
    QString const& selectedFileSet, QString const& folderPath)
{
    QJsonObject configurationObject;

    if (!svdData.isEmpty())
    {
        QJsonArray svdArray;

        for (auto singleCPU : svdData)
        {
            QJsonObject cpuObject;
            
            cpuObject.insert(SVDConstants::NAME, singleCPU->getCPUName());
            cpuObject.insert(SVDConstants::CPUREVISION, singleCPU->getRevision());
            cpuObject.insert(SVDConstants::CREATESVDFLAG, singleCPU->shouldCreateFile());
            cpuObject.insert(SVDConstants::CPUENDIAN, singleCPU->getEndian());
            cpuObject.insert(SVDConstants::CPPUNVICPRIOBITS, singleCPU->getNVICPrioBits());
            cpuObject.insert(SVDConstants::CPUMPUPRESENT, singleCPU->isMPUPresent());
            cpuObject.insert(SVDConstants::CPUFPUPRESENT, singleCPU->isFPUPresent());
            cpuObject.insert(SVDConstants::CPUVENDORSYSTICKCONFIG, singleCPU->isVendorSystickConfig());

            svdArray.push_back(cpuObject);
        }

        configurationObject.insert(SVDConstants::CONFIGURATIONCPUS, svdArray);
    }

    configurationObject.insert(SVDConstants::VIEW, selectedView);
    configurationObject.insert(SVDConstants::FOLDERPATH, folderPath);
    configurationObject.insert(SVDConstants::SAVETOFILESET, saveToFileSetFlag);
    configurationObject.insert(SVDConstants::FILESET, selectedFileSet);

    QJsonDocument document(configurationObject);
    return document;
}
