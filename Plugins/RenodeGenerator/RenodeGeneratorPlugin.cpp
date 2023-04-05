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

#include <QCoreApplication>
#include <QFileInfo>
#include <QMessageBox>
#include <QDir>

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

    RenodeFileSelectionGroup* fileSelectionGroup(new RenodeFileSelectionGroup());
    RenodeCpuEditor* cpuEditor(new RenodeCpuEditor());

    CPUSelectionDialog selectionDialog(component, utility->getLibraryInterface(), viewNames,
        component->getFileSetNames(), cpuEditor, "Renode platform", fileSelectionGroup, utility->getParentWidget());
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

                QString xmlFilePath = selectionDialog.getTargetFolder();

                RenodeGenerator generator(utility->getLibraryInterface());
                generator.generate(component, xmlFilePath, renodeCPURoutes,
                    fileSelectionGroup->writeCpu(), fileSelectionGroup->writeMemory(), fileSelectionGroup->writePeripherals());

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
void RenodeGeneratorPlugin::onErrorReport(const QString& report)
{
    utility_->printError(report);
}

//-----------------------------------------------------------------------------
// Function: RenodeGeneratorPlugin::getOutputFormat()
//-----------------------------------------------------------------------------
QString RenodeGeneratorPlugin::getOutputFormat() const
{
    return QStringLiteral("repl");
}
