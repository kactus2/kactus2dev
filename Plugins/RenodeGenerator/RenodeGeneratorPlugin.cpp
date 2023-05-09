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
#include <Plugins/RenodeGenerator/CPUDialog/RenodeCpuRoutesContainer.h>
#include <Plugins/RenodeGenerator/CPUDialog/RenodeFileSelectionGroup.h>
#include <Plugins/RenodeGenerator/CPUDialog/RenodeUtilities.h>
#include <Plugins/RenodeGenerator/RenodeConfigurationManager.h>

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

    RenodeConfigurationManager* configManager(new RenodeConfigurationManager(utility));
    QJsonObject configurationObject = configManager->getConfigurationObject(component);

    QString configurationFolderPath = configurationObject.value(RenodeConstants::FOLDERPATH).toString("");
    QString configurationView = configurationObject.value(RenodeConstants::VIEW).toString("");
    QString configurationFileSet = configurationObject.value(RenodeConstants::FILESET).toString("");
    bool saveToFileSetFlag = configurationObject.value(RenodeConstants::SAVETOFILESET).toBool(true);

    RenodeFileSelectionGroup* fileSelectionGroup(new RenodeFileSelectionGroup(configurationObject));
    RenodeCpuEditor* cpuEditor(new RenodeCpuEditor(utility, configurationObject));

    CPUSelectionDialog selectionDialog(component, utility->getLibraryInterface(), viewNames, component->getFileSetNames(),
        cpuEditor, "Renode platform", fileSelectionGroup, utility->getParentWidget(), configurationFolderPath,
        saveToFileSetFlag, configurationFileSet, configurationView);

    if (selectionDialog.exec() == QDialog::Accepted)
    {
        QVector<QSharedPointer<CpuRoutesContainer> > cpuRoutes = selectionDialog.getSelectedCPUs();
        if (!cpuRoutes.isEmpty())
        {
            QVector<QSharedPointer<RenodeCpuRoutesContainer> > renodeCPURoutes;
            for (auto cpu : cpuRoutes)
            {
                QSharedPointer<RenodeCpuRoutesContainer> renodeCPU = cpu.dynamicCast<RenodeCpuRoutesContainer>();
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

                configManager->createConfigureFile(renodeCPURoutes, selectedView, saveToFileSet, selectedFileSet, xmlFilePath,
                    writeCpuFlag, writeMemoryFlag, writePeripheralFlag, component);

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

    QVector<QSharedPointer<RenodeCpuRoutesContainer> > renodeCPUs;
    for (auto defaultCPU : ConnectivityGraphUtilities::getDefaultCPUs(utilityLibrary, component, viewName))
    {
        QSharedPointer<RenodeCpuRoutesContainer> newCPU(new RenodeCpuRoutesContainer(*defaultCPU.data()));
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
