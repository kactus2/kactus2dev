//-----------------------------------------------------------------------------
// File: VerilogGeneratorPlugin.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 27.08.2014
//
// Description:
// Plugin for structural verilog generation.
//-----------------------------------------------------------------------------

#include "VerilogGeneratorPlugin.h"

#include <Plugins/PluginSystem/IPluginUtility.h>
#include <Plugins/PluginSystem/GeneratorPlugin/GeneratorConfigurationDialog.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <QFileDialog>

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::VerilogGeneratorPlugin()
//-----------------------------------------------------------------------------
VerilogGeneratorPlugin::VerilogGeneratorPlugin(): QObject(0)
{
    settings_.generateInterfaces_ = false;
    settings_.lastFileSetName_ = "";
    settings_.lastFileSetName_ = "";
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::()
//-----------------------------------------------------------------------------
VerilogGeneratorPlugin::~VerilogGeneratorPlugin()
{
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getName()
//-----------------------------------------------------------------------------
QString VerilogGeneratorPlugin::getName() const
{
    return tr("Verilog Generator");
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getVersion()
//-----------------------------------------------------------------------------
QString VerilogGeneratorPlugin::getVersion() const
{
    return "2.0";
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getDescription()
//-----------------------------------------------------------------------------
QString VerilogGeneratorPlugin::getDescription() const
{
    return tr("Generates a Verilog module for a component or a hierarchy of modules for a HW design.");
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getVendor()
//-----------------------------------------------------------------------------
QString VerilogGeneratorPlugin::getVendor() const
{
    return tr("TUT");
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getLicence()
//-----------------------------------------------------------------------------
QString VerilogGeneratorPlugin::getLicence() const
{
    return tr("GPL2");
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getLicenceHolder()
//-----------------------------------------------------------------------------
QString VerilogGeneratorPlugin::getLicenceHolder() const
{
    return tr("Public");
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getSettingsWidget()
//-----------------------------------------------------------------------------
QWidget* VerilogGeneratorPlugin::getSettingsWidget()
{
    return new QWidget();
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getSettingsModel()
//-----------------------------------------------------------------------------
PluginSettingsModel* VerilogGeneratorPlugin::getSettingsModel()
{
    return NULL;
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getProgramRequirements()
//-----------------------------------------------------------------------------
QList<IPlugin::ExternalProgramRequirement> VerilogGeneratorPlugin::getProgramRequirements()
{
    return QList<IPlugin::ExternalProgramRequirement>();
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getIcon()
//-----------------------------------------------------------------------------
QIcon VerilogGeneratorPlugin::getIcon() const
{
    return QIcon(":icons/verilogGenerator.png");
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::checkGeneratorSupport()
//-----------------------------------------------------------------------------
bool VerilogGeneratorPlugin::checkGeneratorSupport(QSharedPointer<Component const> component,
    QSharedPointer<Design const> design,
    QSharedPointer<DesignConfiguration const> designConfiguration) const
{
    // If design or design configuration exists, their implementation overrides the top component.
    if (design)
    {
        return design->getImplementation() == KactusAttribute::HW;
    }

    if (designConfiguration)
    {
        return designConfiguration->getImplementation() == KactusAttribute::HW;
    }
    
    // Else the availability is determined based on the top component.
    return component && component->getImplementation() == KactusAttribute::HW;    
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::runGenerator()
//-----------------------------------------------------------------------------
void VerilogGeneratorPlugin::runGenerator(IPluginUtility* utility, 
    QSharedPointer<Component> component,
    QSharedPointer<Design> design,
    QSharedPointer<DesignConfiguration> designConfiguration)
{
    // First state we are running. Tell the version.
    utility->printInfo(tr("Running %1 %2.").arg(getName(), getVersion()));

    // Must have a component under any condition.
    if (!component)
    {
        utility->printError(tr("Null  component given as a parameter."));
        return;
    }

    GenerationTuple input;
    input.component = component;
    input.design = design;
    input.designConfiguration = designConfiguration;

    VerilogWriterFactory factory(utility->getLibraryInterface(), &settings_, utility->getKactusVersion(), getVersion());

    // Create model for the configuration widget.
    QSharedPointer<GeneratorConfiguration> configuration(new GeneratorConfiguration
        (utility->getLibraryInterface(), &factory, input, &settings_));

    // Create the dialog and execute: The user will ultimately accept the configuration.
    GeneratorConfigurationDialog dialog(configuration, utility->getParentWidget());

    if (dialog.exec() != QDialog::Accepted)
    {
        utility->printInfo(tr("Generation aborted."));
        return;
    }

    utility->printInfo(tr("Generation complete."));
}