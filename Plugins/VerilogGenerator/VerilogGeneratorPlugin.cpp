//-----------------------------------------------------------------------------
// File: VerilogGeneratorPlugin.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 26.01.2017
//
// Description:
// Plugin for Verilog generation.
//-----------------------------------------------------------------------------

#include "VerilogGeneratorPlugin.h"

#include "VerilogSourceHighlight.h"

#include "VerilogWriterFactory/VerilogWriterFactory.h"

#include <KactusAPI/include/LibraryInterface.h>

#include <Plugins/common/LanguageHighlighter.h>
#include <Plugins/common/HDLParser/HDLCommandLineParser.h>
#include <Plugins/PluginSystem/GeneratorPlugin/MessagePasser.h>
#include <KactusAPI/include/IPluginUtility.h>
#include <Plugins/PluginSystem/GeneratorPlugin/HDLGenerationDialog.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/DesignConfiguration/DesignConfiguration.h>

#include <QDir>

//----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::VerilogGeneratorPlugin()
//-----------------------------------------------------------------------------
VerilogGeneratorPlugin::VerilogGeneratorPlugin(): QObject(0)
{
    settings_.generateInterfaces_ = false;
    settings_.lastFileSetName_ = "";
    settings_.lastViewName_ = "";
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
    return "2.4";
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getDescription()
//-----------------------------------------------------------------------------
QString VerilogGeneratorPlugin::getDescription() const
{
    return tr("Generates a Verilog module for a HW component or a hierarchy of modules for a HW design.");
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getVendor()
//-----------------------------------------------------------------------------
QString VerilogGeneratorPlugin::getVendor() const
{
    return tr("Tampere University (tuni.fi)");
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getLicense()
//-----------------------------------------------------------------------------
QString VerilogGeneratorPlugin::getLicense() const
{
    return tr("GPL2");
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getLicenseHolder()
//-----------------------------------------------------------------------------
QString VerilogGeneratorPlugin::getLicenseHolder() const
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
    if (component.isNull())
    {
        utility->printError(tr("Null component given as a parameter."));
        return;
    }

    MessagePasser messages;

    GenerationTuple input;
    input.component = component;
    input.design = design;
    input.designConfiguration = designConfiguration;
    input.messages = &messages;

    VerilogWriterFactory factory(utility->getLibraryInterface(), &messages, &settings_,
        utility->getKactusVersion(), getVersion());

    // Create model for the configuration widget.
    QSharedPointer<GenerationControl> configuration(new GenerationControl
        (utility->getLibraryInterface(), &factory, input, &settings_));

    // Create the dialog and execute: The user will accept/decline the configuration.
    HDLGenerationDialog dialog(configuration, QStringLiteral("Verilog"), utility->getParentWidget());
    
    LanguageHighlighter highlighter(0);
    VerilogSourceHighlight style;
    style.apply(&highlighter);

    dialog.setPreviewHighlighter(&highlighter);

    connect(&messages, SIGNAL(errorMessage(const QString&)),
        &dialog, SLOT(onErrorMessage(const QString&)), Qt::UniqueConnection);
    connect(&messages, SIGNAL(noticeMessage(const QString&)),
       & dialog, SLOT(onNoticeMessage(const QString&)), Qt::UniqueConnection);

    dialog.onViewChanged();

    if (dialog.exec() == QDialog::Accepted)
    {
        utility->printInfo(tr("Generation complete."));
    }
    else
    {
        utility->printInfo(tr("Generation aborted."));    
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getOutputFormat()
//-----------------------------------------------------------------------------
QString VerilogGeneratorPlugin::getOutputFormat() const
{
    return QStringLiteral("Verilog");
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::runGenerator()
//-----------------------------------------------------------------------------
void VerilogGeneratorPlugin::runGenerator(IPluginUtility* utility,
    QSharedPointer<Component> component,
    QSharedPointer<Design> design,
    QSharedPointer<DesignConfiguration> designConfiguration,
    QString const& viewName,
    QString const& outputDirectory)
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

    settings_.lastViewName_ = viewName;    

    MessagePasser messages;

    GenerationTuple input;
    input.component = component;
    input.design = design;
    input.designConfiguration = designConfiguration;
    input.messages = &messages;

    VerilogWriterFactory factory(utility->getLibraryInterface(), &messages, &settings_,
        utility->getKactusVersion(), getVersion());

    // Use configuration to drive the generation.
    GenerationControl configuration(utility->getLibraryInterface(), &factory, input, &settings_);
    configuration.getOutputControl()->setOutputPath(outputDirectory);
    configuration.getViewSelection()->setSaveToFileset(false);

    configuration.parseDocuments();
    if (configuration.writeDocuments())
    {
        utility->printInfo(tr("Generation complete."));
    }
    else
    {
        utility->printInfo(tr("Generation failed."));
    }
}
