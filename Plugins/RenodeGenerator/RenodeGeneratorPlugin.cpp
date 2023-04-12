// Copyright Tampere University of Technology
//-----------------------------------------------------------------------------
// File: RenodeGeneratorPlugin.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 10.02.2023
//
// Description:
// Renode generator plugin.
//-----------------------------------------------------------------------------

#include "RenodeGeneratorPlugin.h"

#include <RenodeGenerator.h>

#include <Plugins/common/HDLParser/HDLParserCommon.h>
#include <Plugins/common/CPUDialog/CPUSelectionDialog.h>

#include <Plugins/PluginSystem/GeneratorPlugin/GenerationControl.h>
#include <Plugins/PluginSystem/GeneratorPlugin/MessagePasser.h>

#include <Plugins/RenodeGenerator/CPUDialog/RenodeCpuEditor.h>
#include <Plugins/RenodeGenerator/CPUDialog/RenodeCPUDetailRoutes.h>
#include <Plugins/RenodeGenerator/CPUDialog/RenodeFileSelectionGroup.h>
#include <Plugins/RenodeGenerator/CPUDialog/RenodeUtilities.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/File.h>

#include <QCoreApplication>
#include <QFileInfo>
#include <QMessageBox>
#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

//-----------------------------------------------------------------------------
// Function: RenodeGeneratorPlugin::RenodeGeneratorPlugin()
//-----------------------------------------------------------------------------
RenodeGeneratorPlugin::RenodeGeneratorPlugin() : QObject(0)
{

}

//-----------------------------------------------------------------------------
// Function: RenodeGeneratorPlugin::getName()
//----------------------------------------------------------------------------
QString RenodeGeneratorPlugin::getName() const
{
    return tr("Renode Generator");
}

//-----------------------------------------------------------------------------
// Function: RenodeGeneratorPlugin::getVersion()
//-----------------------------------------------------------------------------
QString RenodeGeneratorPlugin::getVersion() const
{
    return "0.1";
}

//-----------------------------------------------------------------------------
// Function: RenodeGeneratorPlugin::getDescription()
//-----------------------------------------------------------------------------
QString RenodeGeneratorPlugin::getDescription() const
{
    return tr("");
}

//-----------------------------------------------------------------------------
// Function: RenodeGeneratorPlugin::getVendor()
//-----------------------------------------------------------------------------
QString RenodeGeneratorPlugin::getVendor() const
{
    return tr("Tampere University (tuni.fi)");
}

//-----------------------------------------------------------------------------
// Function: RenodeGeneratorPlugin::getLicence()
//-----------------------------------------------------------------------------
QString RenodeGeneratorPlugin::getLicense() const
{
    return tr("GPL2");
}

//-----------------------------------------------------------------------------
// Function: RenodeGeneratorPlugin::getLicenceHolder()
//-----------------------------------------------------------------------------
QString RenodeGeneratorPlugin::getLicenseHolder() const
{
    return tr("Public");
}

//-----------------------------------------------------------------------------
// Function: RenodeGeneratorPlugin::getSettingsWidget()
//-----------------------------------------------------------------------------
QWidget* RenodeGeneratorPlugin::getSettingsWidget()
{
    return new QWidget();
}

//-----------------------------------------------------------------------------
// Function: RenodeGeneratorPlugin::getSettingsModel()
//-----------------------------------------------------------------------------
PluginSettingsModel* RenodeGeneratorPlugin::getSettingsModel()
{
    return NULL;
}

//-----------------------------------------------------------------------------
// Function: RenodeGeneratorPlugin::getIcon()
//-----------------------------------------------------------------------------
QIcon RenodeGeneratorPlugin::getIcon() const
{
    return QIcon(":icons/RenodeGenerator.png");
}

//-----------------------------------------------------------------------------
// Function: RenodeGeneratorPlugin::checkGeneratorSupport()
//-----------------------------------------------------------------------------
bool RenodeGeneratorPlugin::checkGeneratorSupport(QSharedPointer<Component const> component, QSharedPointer<Design const> design, QSharedPointer<DesignConfiguration const> designConfiguration) const
{
    // Must be ran on a design.
    return (design != 0 && designConfiguration != 0);
}

//-----------------------------------------------------------------------------
// Function: RenodeGeneratorPlugin::runGenerator()
//-----------------------------------------------------------------------------
void RenodeGeneratorPlugin::runGenerator(IPluginUtility* utility, QSharedPointer<Component> component, QSharedPointer<Design> design, QSharedPointer<DesignConfiguration> designConfiguration)
{
    utility->printInfo(tr("Running %1 %2.").arg(getName(), getVersion()));

    GenerationTuple controlTuple;
    controlTuple.component = component;
    controlTuple.design = design;
    controlTuple.designConfiguration = designConfiguration;

    QSharedPointer<QList<QSharedPointer<View> > > views = GenerationControl::findPossibleViews(controlTuple);
    QStringList viewNames;
    for (auto view : *views)
    {
        viewNames.append(view->name());
    }

    QJsonObject configurationObject = getConfigurationObject(component, utility);

    QString configurationFolderPath = configurationObject.value(RenodeConstants::FOLDERPATH).toString("");
    QString configurationView = configurationObject.value(RenodeConstants::VIEW).toString("");
    QString configurationFileSet = configurationObject.value(RenodeConstants::FILESET).toString("");
    bool saveToFileSetFlag = configurationObject.value(RenodeConstants::SAVETOFILESET).toBool(true);

    RenodeFileSelectionGroup* fileSelectionGroup(new RenodeFileSelectionGroup(configurationObject));
    RenodeCpuEditor* cpuEditor(new RenodeCpuEditor(configurationObject));

    CPUSelectionDialog selectionDialog(component, utility->getLibraryInterface(), viewNames, component->getFileSetNames(),
        cpuEditor, "Renode platform", fileSelectionGroup, utility->getParentWidget(), configurationFolderPath,
        saveToFileSetFlag, configurationFileSet, configurationView);

    if (selectionDialog.exec() == QDialog::Accepted)
    {
        QVector<QSharedPointer<CPUDetailRoutes> > cpuRoutes = selectionDialog.getSelectedCPUs();
        if (!cpuRoutes.isEmpty())
        {
            QVector<QSharedPointer<RenodeCPUDetailRoutes> > renodeCPURoutes;
            for (auto cpu : cpuRoutes)
            {
                QSharedPointer<RenodeCPUDetailRoutes> renodeCPU = cpu.dynamicCast<RenodeCPUDetailRoutes>();
                if (renodeCPU)
                {
                    renodeCPURoutes.append(renodeCPU);
                }
            }

            if (!renodeCPURoutes.isEmpty())
            {
                QString selectedView = selectionDialog.getView();
                bool saveToFileSet = selectionDialog.saveToFileSet();
                QString selectedFileSet = selectionDialog.getTargetFileSet();
                QString xmlFilePath = selectionDialog.getTargetFolder();
                bool writeCpuFlag = fileSelectionGroup->writeCpu();
                bool writeMemoryFlag = fileSelectionGroup->writeMemory();
                bool writePeripheralFlag = fileSelectionGroup->writePeripherals();

                RenodeGenerator generator(utility->getLibraryInterface());
                generator.generate(component, xmlFilePath, renodeCPURoutes, writeCpuFlag, writeMemoryFlag, writePeripheralFlag);

                createConfigureFile(renodeCPURoutes, selectedView, saveToFileSet, selectedFileSet, xmlFilePath, writeCpuFlag, writeMemoryFlag, writePeripheralFlag,
                    utility->getLibraryInterface(), component);

                if (selectionDialog.saveToFileSet())
                {
                    QString fileSetName = selectionDialog.getTargetFileSet();
                    if (!fileSetName.isEmpty())
                    {
                        QStringList generatedFiles = generator.getGeneratedFiles();
                        bool testi = false;
                        saveToFileset(utility, generatedFiles, component, fileSetName);
                    }
                }

                utility->printInfo(tr("Generation complete."));
            }
        }
        else
        {
            utility->printError(tr("Could not find CPU routes. Aborting generation."));
        }
    }
    else
    {
        utility->printInfo(tr("Generation aborted."));
    }
}

//-----------------------------------------------------------------------------
// Function: RenodeGeneratorPlugin::getConfigurationObject()
//-----------------------------------------------------------------------------
QJsonObject RenodeGeneratorPlugin::getConfigurationObject(QSharedPointer<Component> component, IPluginUtility* utility)
{
    QJsonDocument configurationDocument = getConfigurationDocument(component, utility);
    QJsonArray configurationArray = configurationDocument.array();

    if (!configurationArray.isEmpty())
    {
        QJsonValue mainConfigurationValue = configurationArray.first();
        if (mainConfigurationValue.type() == QJsonValue::Object)
        {
            return mainConfigurationValue.toObject();
        }
    }

    return QJsonObject();
}

//-----------------------------------------------------------------------------
// Function: RenodeGeneratorPlugin::getConfigurationDocument()
//-----------------------------------------------------------------------------
QJsonDocument RenodeGeneratorPlugin::getConfigurationDocument(QSharedPointer<Component> component, IPluginUtility* utility)
{
    QSharedPointer<FileSet> configurationFileSet = getConfigurationFileSet(component);
    if (configurationFileSet)
    {
        QSharedPointer<File> configurationFileItem = getConfigurationFile(configurationFileSet);
        if (configurationFileItem)
        {
            QString componentPath = utility->getLibraryInterface()->getDirectoryPath(component->getVlnv()) + "/";
            QString configurationFilePath = General::getAbsolutePath(componentPath, configurationFileItem->name());

            QFile configurationFile(configurationFilePath);

            if (!configurationFile.open(QFile::ReadOnly))
            {
                utility->printError("Could not open configuration file " + configurationFilePath);
            }
            else
            {
                QJsonDocument configurationDocument = QJsonDocument::fromJson(configurationFile.readAll());
                return configurationDocument;
            }
        }
    }

    return QJsonDocument();
}

//-----------------------------------------------------------------------------
// Function: RenodeGeneratorPlugin::createConfigureFile()
//-----------------------------------------------------------------------------
void RenodeGeneratorPlugin::createConfigureFile(QVector<QSharedPointer<RenodeCPUDetailRoutes>> renodeData,
    QString const& selectedView, bool saveToFileSet, QString const& selectedFileSet, QString const& folderPath,
    bool writeCPU, bool writeMemory, bool writePeripherals, LibraryInterface* library, QSharedPointer<Component> topComponent)
{
    QSharedPointer<FileSet> configurationFileSet = getConfigurationFileSet(topComponent);
    if (!configurationFileSet)
    {
        configurationFileSet = QSharedPointer<FileSet>(new FileSet(RenodeConstants::CONFIGURATIONFILESETNAME));
        topComponent->getFileSets()->append(configurationFileSet);
    }
    QSharedPointer<File> configurationFileItem = getConfigurationFile(configurationFileSet);
    if (!configurationFileItem)
    {
        QString configurationFileName = topComponent->getVlnv().getName() + "." + topComponent->getVlnv().getVersion() + "_" +
            RenodeConstants::CONFIGURATIONFILEEXTENSION + "." + RenodeConstants::JSONFILETYPE;

        configurationFileItem = QSharedPointer<File>(new File(configurationFileName));
        configurationFileItem->getFileTypes()->append(RenodeConstants::JSONFILETYPE);

        configurationFileSet->getFiles()->append(configurationFileItem);
    }

    QString componentPath = library->getDirectoryPath(topComponent->getVlnv()) + "/";

    QString configurationFilePath = General::getAbsolutePath(componentPath, configurationFileItem->name());

    QFile configurationFile(configurationFilePath);
    if (!configurationFile.open(QIODevice::WriteOnly))
    {
        return;
    }

    QJsonDocument document = createJsonDocument(renodeData, selectedView, saveToFileSet, selectedFileSet, folderPath, writeCPU, writeMemory, writePeripherals);
    configurationFile.write(document.toJson());

    library->writeModelToFile(topComponent);
}

//-----------------------------------------------------------------------------
// Function: RenodeGeneratorPlugin::getConfigurationFileSet()
//-----------------------------------------------------------------------------
QSharedPointer<FileSet> RenodeGeneratorPlugin::getConfigurationFileSet(QSharedPointer<Component> topComponent)
{
    for (auto fileSet : *topComponent->getFileSets())
    {
        if (fileSet->name() == RenodeConstants::CONFIGURATIONFILESETNAME)
        {
            return fileSet;
        }
    }

    return QSharedPointer<FileSet>();
}

//-----------------------------------------------------------------------------
// Function: RenodeGeneratorPlugin::getConfigurationFile()
//-----------------------------------------------------------------------------
QSharedPointer<File> RenodeGeneratorPlugin::getConfigurationFile(QSharedPointer<FileSet> configurationFileSet)
{
    if (configurationFileSet)
    {
        for (auto containedFile : *configurationFileSet->getFiles())
        {
            if (containedFile->name().contains(RenodeConstants::CONFIGURATIONFILEEXTENSION))
            {
                return containedFile;
            }
        }
    }

    return QSharedPointer<File>();
}

//-----------------------------------------------------------------------------
// Function: RenodeGeneratorPlugin::createJsonDocument()
//-----------------------------------------------------------------------------
QJsonDocument RenodeGeneratorPlugin::createJsonDocument(QVector<QSharedPointer<RenodeCPUDetailRoutes>> renodeData,
    QString const& selectedView, bool saveToFileSetFlag, QString const& selectedFileSet, QString const& folderPath,
    bool writeCPU, bool writeMemory, bool writePeripherals)
{
    QJsonArray configurationArray;

    for (auto renodeCPU : renodeData)
    {
        QJsonObject singleConfigurationObject;

        QJsonObject cpuObject;
        cpuObject.insert(RenodeConstants::CPUCLASS, renodeCPU->getClassName());
        cpuObject.insert(RenodeConstants::CPUTYPE, renodeCPU->getCpuType());
        cpuObject.insert(RenodeConstants::CPUTIME, renodeCPU->getTimeProvider());

        singleConfigurationObject.insert(RenodeConstants::CPU, cpuObject);

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

            singleConfigurationObject.insert(RenodeConstants::MEMORY, memoryArray);
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

            singleConfigurationObject.insert(RenodeConstants::PERIPHERALS, peripheralArray);
        }

        singleConfigurationObject.insert(RenodeConstants::VIEW, selectedView);
        singleConfigurationObject.insert(RenodeConstants::FOLDERPATH, folderPath);

        singleConfigurationObject.insert(RenodeConstants::SAVETOFILESET, saveToFileSetFlag);
        singleConfigurationObject.insert(RenodeConstants::FILESET, selectedFileSet);

        QJsonObject writeFlagsObject;
        writeFlagsObject.insert(RenodeConstants::CPU, writeCPU);
        writeFlagsObject.insert(RenodeConstants::MEMORY, writeMemory);
        writeFlagsObject.insert(RenodeConstants::PERIPHERALS, writePeripherals);

        singleConfigurationObject.insert(RenodeConstants::WRITEFILES, writeFlagsObject);

        configurationArray.push_back(singleConfigurationObject);
    }

    QJsonDocument document(configurationArray);
    return document;
}

//-----------------------------------------------------------------------------
// Function: RenodeGeneratorPlugin::saveToFileset()
//-----------------------------------------------------------------------------
void RenodeGeneratorPlugin::saveToFileset(IPluginUtility* utility, QStringList const& renodeFiles, QSharedPointer<Component> component, QString const& fileSetName)
{
    if (renodeFiles.isEmpty())
    {
        return;
    }

    QString xmlFilePath = utility->getLibraryInterface()->getDirectoryPath(component->getVlnv());
    QSharedPointer<FileSet> targetFileset = component->getFileSet(fileSetName);
    if (!targetFileset)
    {
        targetFileset = QSharedPointer<FileSet>(new FileSet(fileSetName));
        component->getFileSets()->append(targetFileset);
    }

    for (auto fileName : renodeFiles)
    {
        QString relativeFilePath = General::getRelativePath(xmlFilePath, fileName);

        QSharedPointer<File> file;
        foreach(QSharedPointer<File> filesetFile, *targetFileset->getFiles())
        {
            if (filesetFile->name().compare(relativeFilePath) == 0)
            {
                file = filesetFile;
            }
        }

        if (!file)
        {
            file = QSharedPointer<File>(new File(relativeFilePath));

            QString fileType = RenodeConstants::RENODEFILETYPE;
            QString fileExtension = relativeFilePath.split(".").last();
            if (fileExtension == RenodeConstants::PYTHONFILETYPE)
            {
                fileType = RenodeConstants::PYTHONFILETYPE;
            }

            file->getFileTypes()->append(fileType);

            targetFileset->addFile(file);
            utility->printInfo(tr("Added file %1 to top component file set %2.").arg(fileName, fileSetName));
        }

        file->setDescription(tr("Generated on %1 by Kactus2 %2 plugin version %3.").arg(
            QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss"), getName(), getVersion()));
    }


    utility->getLibraryInterface()->writeModelToFile(component);
}

//-----------------------------------------------------------------------------
// Function: RenodeGeneratorPlugin::runGenerator()
//-----------------------------------------------------------------------------
void RenodeGeneratorPlugin::runGenerator(IPluginUtility* utility, QSharedPointer<Component> component, QSharedPointer<Design> design, QSharedPointer<DesignConfiguration> designConfiguration, QString const& viewName, QString const& outputDirectory)
{
    utility->printInfo(tr("Running %1 %2.").arg(getName(), getVersion()));
    utility->printInfo(tr("Running generation for %1 and view '%2'.").arg(component->getVlnv().toString(), viewName));

    QDir targetDirectory;
    if (!targetDirectory.mkpath(outputDirectory))
    {
        utility->printError(tr("Could not create target directory: %1").arg(outputDirectory));
        return;
    }

    utility->printInfo(tr("Target directory: %1").arg(outputDirectory));


    MessagePasser messages;

    GenerationTuple input;
    input.component = component;
    input.design = design;
    input.designConfiguration = designConfiguration;
    input.messages = &messages;

    LibraryInterface* utilityLibrary = utility->getLibraryInterface();

    QVector<QSharedPointer<RenodeCPUDetailRoutes> > renodeCPUs;
    for (auto defaultCPU : ConnectivityGraphUtilities::getDefaultCPUs(utilityLibrary, component, viewName))
    {
        QSharedPointer<RenodeCPUDetailRoutes> newCPU(new RenodeCPUDetailRoutes(*defaultCPU.data()));
        renodeCPUs.append(newCPU);
    }

    if (renodeCPUs.isEmpty())
    {
        utility->printInfo(tr("Generation Failed. No CPUs found."));
    }

    RenodeGenerator generator(utilityLibrary);
    generator.generate(component, outputDirectory, renodeCPUs, true, true, true);

    utility->printInfo(tr("Generation complete."));
}

//-----------------------------------------------------------------------------
// Function: RenodeGeneratorPlugin::getProgramRequirements()
//-----------------------------------------------------------------------------
QList<IPlugin::ExternalProgramRequirement> RenodeGeneratorPlugin::getProgramRequirements()
{
    return QList<IPlugin::ExternalProgramRequirement>();
}

//-----------------------------------------------------------------------------
// Function: RenodeGeneratorPlugin:::onErrorReport()
//-----------------------------------------------------------------------------
void RenodeGeneratorPlugin::onErrorReport(const QString& /*report*/)
{
//     utility_->printError(report);
}

//-----------------------------------------------------------------------------
// Function: RenodeGeneratorPlugin::getOutputFormat()
//-----------------------------------------------------------------------------
QString RenodeGeneratorPlugin::getOutputFormat() const
{
    return QStringLiteral("repl");
}
