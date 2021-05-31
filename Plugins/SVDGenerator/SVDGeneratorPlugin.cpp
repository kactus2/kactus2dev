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

#include <Plugins/common/HDLParser/HDLParserCommon.h>

#include <Plugins/PluginSystem/IPluginUtility.h>
#include <Plugins/PluginSystem/GeneratorPlugin/GenerationControl.h>
#include <Plugins/SVDGenerator/CPUDialog/CPUSelectionDialog.h>

#include <library/LibraryInterface.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/FileSet.h>

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
    return "0.1";
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
    return tr("TUT");
}

//-----------------------------------------------------------------------------
// Function: SVDGeneratorPlugin::getLicence()
//-----------------------------------------------------------------------------
QString SVDGeneratorPlugin::getLicence() const
{
    return tr("GPL2");
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getLicenceHolder()
//-----------------------------------------------------------------------------
QString SVDGeneratorPlugin::getLicenceHolder() const
{
    return tr("Public");
}

//-----------------------------------------------------------------------------
// Function: SVDGeneratorPlugin::getSettingsWidget()
//-----------------------------------------------------------------------------
QWidget* SVDGeneratorPlugin::getSettingsWidget()
{
    return new QWidget();
}

//-----------------------------------------------------------------------------
// Function: SVDGeneratorPlugin::getSettingsModel()
//-----------------------------------------------------------------------------
PluginSettingsModel* SVDGeneratorPlugin::getSettingsModel()
{
    return NULL;
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

    CPUSelectionDialog selectionDialog(component, utility->getLibraryInterface(), viewNames,
        component->getFileSetNames(), utility->getParentWidget());
    if (selectionDialog.exec() == QDialog::Accepted)
    {
        QVector<QSharedPointer<ConnectivityGraphUtilities::cpuDetailRoutes> > cpuRoutes =
            selectionDialog.getSelectedCPUs();
        if (!cpuRoutes.isEmpty())
        {
            bool blocksArePeripherals = selectionDialog.peripheralsAreBlocks();
            bool mapsArePeripherals = selectionDialog.peripheralsAreMaps();
            QString xmlFilePath = selectionDialog.getTargetFolder();

            SVDGenerator generator(utility->getLibraryInterface());
            generator.generate(component, xmlFilePath, cpuRoutes, blocksArePeripherals, mapsArePeripherals);

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
