//-----------------------------------------------------------------------------
// File: SVDGeneratorPlugin.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 14.04.2021
//
// Description:
// Plugin for creating a CMSIS System View Description listing.
//-----------------------------------------------------------------------------

#include "SVDGeneratorPlugin.h"
#include "SVDGenerator.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/FileSet.h>

#include <KactusAPI/include/IPluginUtility.h>
#include <KactusAPI/include/LibraryInterface.h>

#include <Plugins/common/HDLParser/HDLParserCommon.h>
#include <Plugins/common/ConnectivityGraphUtilities.h>
#include <Plugins/common/CPUDialog/CPUSelectionDialog.h>
#include <Plugins/PluginSystem/GeneratorPlugin/GenerationControl.h>
#include <Plugins/PluginSystem/GeneratorPlugin/MessagePasser.h>
#include <Plugins/SVDGenerator/CPUDialog/SVDCPUEditor.h>
#include <Plugins/SVDGenerator/CPUDialog/SVDCpuRoutesContainer.h>
#include <Plugins/SVDGenerator/SVDConfigurationManager.h>
#include <Plugins/SVDGenerator/CPUDialog/SVDUtilities.h>

#include <editors/MemoryDesigner/ConnectivityGraphFactory.h>
#include <editors/MemoryDesigner/MasterSlavePathSearch.h>

#include <QDateTime>
#include <QFileDialog>

//----------------------------------------------------------------------------
// Function: SVDGeneratorPlugin::SVDGeneratorPlugin()
//-----------------------------------------------------------------------------
SVDGeneratorPlugin::SVDGeneratorPlugin(): QObject(0)
{
}

//-----------------------------------------------------------------------------
// Function: SVDGeneratorPlugin::getName()
//-----------------------------------------------------------------------------
QString SVDGeneratorPlugin::getName() const
{
    return tr("SVD Generator");
}

//-----------------------------------------------------------------------------
// Function: SVDGeneratorPlugin::getVersion()
//-----------------------------------------------------------------------------
QString SVDGeneratorPlugin::getVersion() const
{
    return "0.2";
}

//-----------------------------------------------------------------------------
// Function: SVDGeneratorPlugin::getDescription()
//-----------------------------------------------------------------------------
QString SVDGeneratorPlugin::getDescription() const
{
    return tr("Creates a CMSIS System View Description.");
}

//-----------------------------------------------------------------------------
// Function: SVDGeneratorPlugin::getVendor()
//-----------------------------------------------------------------------------
QString SVDGeneratorPlugin::getVendor() const
{
    return tr("Tampere University (tuni.fi)");
}

//-----------------------------------------------------------------------------
// Function: SVDGeneratorPlugin::getLicense()
//-----------------------------------------------------------------------------
QString SVDGeneratorPlugin::getLicense() const
{
    return tr("GPL2");
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getLicenseHolder()
//-----------------------------------------------------------------------------
QString SVDGeneratorPlugin::getLicenseHolder() const
{
    return tr("Public");
}

//-----------------------------------------------------------------------------
// Function: SVDGeneratorPlugin::getSettingsWidget()
//-----------------------------------------------------------------------------
QWidget* SVDGeneratorPlugin::getSettingsWidget()
{
    return nullptr;
}

//-----------------------------------------------------------------------------
// Function: SVDGeneratorPlugin::getSettingsModel()
//-----------------------------------------------------------------------------
PluginSettingsModel* SVDGeneratorPlugin::getSettingsModel()
{
    return nullptr;
}

//-----------------------------------------------------------------------------
// Function: SVDGeneratorPlugin::getProgramRequirements()
//-----------------------------------------------------------------------------
QList<IPlugin::ExternalProgramRequirement> SVDGeneratorPlugin::getProgramRequirements()
{
    return QList<IPlugin::ExternalProgramRequirement>();
}

//-----------------------------------------------------------------------------
// Function: SVDGeneratorPlugin::getIcon()
//-----------------------------------------------------------------------------
QIcon SVDGeneratorPlugin::getIcon() const
{
    return QIcon(":icons/SVDGenerator.png");
}

//-----------------------------------------------------------------------------
// Function: SVDGeneratorPlugin::checkGeneratorSupport()
//-----------------------------------------------------------------------------
bool SVDGeneratorPlugin::checkGeneratorSupport(QSharedPointer<Component const> component,
    QSharedPointer<Design const> design,
    QSharedPointer<DesignConfiguration const> designConfiguration) const
{
    if (!component || component->getImplementation() != KactusAttribute::HW)
    {
        return false;
    }

    return design || designConfiguration;
}

//-----------------------------------------------------------------------------
// Function: SVDGeneratorPlugin::runGenerator()
//-----------------------------------------------------------------------------
void SVDGeneratorPlugin::runGenerator(IPluginUtility* utility, QSharedPointer<Component> component,
    QSharedPointer<Design> design, QSharedPointer<DesignConfiguration> designConfiguration)
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

    SVDConfigurationManager* configManager(new SVDConfigurationManager(utility));
    QJsonObject configurationObject = configManager->getConfigurationObject(component);

    QString configurationFolderPath = configurationObject.value(SVDConstants::FOLDERPATH).toString("");
    QString configurationView = configurationObject.value(SVDConstants::VIEW).toString("");
    QString configurationFileSet = configurationObject.value(SVDConstants::FILESET).toString("");
    bool saveToFileSetFlag = configurationObject.value(SVDConstants::SAVETOFILESET).toBool(true);

    SVDCPUEditor* cpuEditor(new SVDCPUEditor(configurationObject));

    CPUSelectionDialog selectionDialog(component, utility->getLibraryInterface(), viewNames,
        component->getFileSetNames(), cpuEditor, "SVD", 0, utility->getParentWidget(),
        configurationFolderPath, saveToFileSetFlag, configurationFileSet, configurationView);
    if (selectionDialog.exec() == QDialog::Accepted)
    {
        QVector<QSharedPointer<CpuRoutesContainer> > cpuRoutes = selectionDialog.getSelectedCPUs();
        if (!cpuRoutes.isEmpty())
        {
            QVector<QSharedPointer<SVDCpuRoutesContainer> > svdCPURoutes;
            for (auto cpu : cpuRoutes)
            {
                QSharedPointer<SVDCpuRoutesContainer> svdCPU = cpu.dynamicCast<SVDCpuRoutesContainer>();
                if (svdCPU)
                {
                    svdCPURoutes.append(svdCPU);
                }
            }

            if (!svdCPURoutes.isEmpty())
            {
                QString selectedView = selectionDialog.getView();
                bool saveToFileSet = selectionDialog.saveToFileSet();
                QString selectedFileSet = selectionDialog.getTargetFileSet();
                QString xmlFilePath = selectionDialog.getTargetFolder();

                SVDGenerator generator(utility);
                generator.generate(component, xmlFilePath, svdCPURoutes);

                configManager->createConfigureFile(svdCPURoutes, component,
                    selectedView, saveToFileSet, selectedFileSet, xmlFilePath);

                if (selectionDialog.saveToFileSet())
                {
                    QString fileSetName = selectionDialog.getTargetFileSet();
                    if (!fileSetName.isEmpty())
                    {
                        QStringList generatedFiles = generator.getGeneratedFiles();
                        saveToFileset(utility, generatedFiles, component, fileSetName);
                    }
                }

                utility->printInfo(tr("Generation complete."));
            }
        }
        else
        {
            utility->printError(tr("Could not find CPU routes"));
        }
    }
    else
    {
        utility->printInfo(tr("Generation aborted."));
    }
}

//-----------------------------------------------------------------------------
// Function: SVDGeneratorPlugin::runGenerator()
//-----------------------------------------------------------------------------
void SVDGeneratorPlugin::runGenerator(IPluginUtility* utility, QSharedPointer<Component> component, QSharedPointer<Design> design, QSharedPointer<DesignConfiguration> designConfiguration, QString const& viewName, QString const& outputDirectory)
{
    utility->printInfo(tr("Running %1 %2.").arg(getName(), getVersion()));
    utility->printInfo(tr("Running generation for %1 and view '%2'.").arg(component->getVlnv().toString(),
        viewName));

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

    QVector<QSharedPointer<SVDCpuRoutesContainer> > svdCPUs;
    for (auto defaultCPU : ConnectivityGraphUtilities::getDefaultCPUs(utilityLibrary, component, viewName))
    {
        QSharedPointer<SVDCpuRoutesContainer> newCPU(new SVDCpuRoutesContainer(*defaultCPU.data()));
        svdCPUs.append(newCPU);
    }

    if (svdCPUs.isEmpty())
    {
        utility->printInfo(tr("Generation Failed. No CPUs found."));
    }

    SVDGenerator generator(utility);
    generator.generate(component, outputDirectory, svdCPUs);

    utility->printInfo(tr("Generation complete."));
}

//-----------------------------------------------------------------------------
// Function: SVDGeneratorPlugin::getOutputFormat()
//-----------------------------------------------------------------------------
QString SVDGeneratorPlugin::getOutputFormat() const
{
    return QStringLiteral("SVD");
}

//-----------------------------------------------------------------------------
// Function: SVDGeneratorPlugin::saveToFileset()
//-----------------------------------------------------------------------------
void SVDGeneratorPlugin::saveToFileset(IPluginUtility* utility, QStringList const& svdFiles,
    QSharedPointer<Component> component, QString const& fileSetName)
{
    if (svdFiles.isEmpty())
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

    for (auto fileName : svdFiles)
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
            file->getFileTypes()->append(QStringLiteral("svd"));

            targetFileset->addFile(file);
            utility->printInfo(tr("Added file %1 to top component file set %2.").arg(fileName, fileSetName));
        }

        file->setDescription(tr("Generated on %1 by Kactus2 %2 plugin version %3.").arg(
            QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss"), getName(), getVersion()));
    }


    utility->getLibraryInterface()->writeModelToFile(component);
}
