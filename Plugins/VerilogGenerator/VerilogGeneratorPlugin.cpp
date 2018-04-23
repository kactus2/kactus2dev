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

#include <library/LibraryInterface.h>

#include <Plugins/common/LanguageHighlighter.h>
#include <Plugins/PluginSystem/GeneratorPlugin/MessagePasser.h>
#include <Plugins/PluginSystem/IPluginUtility.h>
#include <Plugins/PluginSystem/GeneratorPlugin/HDLGenerationDialog.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <QCommandLineOption>
#include <QCommandLineParser>
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
    return "2.1";
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

    // Create the dialog and execute: The user will ultimately accept the configuration.
    HDLGenerationDialog dialog(configuration, "Verilog", utility->getParentWidget());
    
    LanguageHighlighter* highlighter = new LanguageHighlighter(0);
    VerilogSourceHighlight style;
    style.apply(highlighter);

    dialog.setPreviewHighlighter(highlighter);

    connect(&messages, SIGNAL(errorMessage(const QString&)),
        &dialog, SLOT(onErrorMessage(const QString&)), Qt::UniqueConnection);
    connect(&messages, SIGNAL(noticeMessage(const QString&)),
       & dialog, SLOT(onNoticeMessage(const QString&)), Qt::UniqueConnection);

    dialog.onViewChanged();

    if (dialog.exec() != QDialog::Accepted)
    {
        utility->printInfo(tr("Generation aborted."));
        delete highlighter;
        
        return;
    }

    // Finally, save the changes.
    utility->printInfo(tr("Generation complete."));
    delete highlighter;
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getCommand()
//-----------------------------------------------------------------------------
QString VerilogGeneratorPlugin::getCommand() const
{
    return QStringLiteral("generate_verilog");
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::run()
//-----------------------------------------------------------------------------
void VerilogGeneratorPlugin::process(QStringList const& arguments, IPluginUtility* utility)
{
    if (arguments.count() < 2)
    {
        return;
    }

    QScopedPointer<QCommandLineParser> parser(createCommandParser());
    parser->parse(arguments);

    if (parser->isSet(QStringLiteral("help")))
    {
        QString helpText = parser->helpText();
        helpText.replace(0, helpText.indexOf(QLatin1Char('[')), 
            QStringLiteral("Usage: Kactus2 generate_verilog "));
        utility->printInfo(helpText);
        return;
    }

    utility->printInfo(tr("Running %1 %2.").arg(getName(), getVersion()));

    VLNV componentVLNV = VLNV(VLNV::COMPONENT, parser->value("c"));
    QSharedPointer<Component> component = 
        utility->getLibraryInterface()->getModel(componentVLNV).dynamicCast<Component>();

    // Must have a component under any condition.
    if (!component)
    {
        utility->printError(tr("Invalid component given as a parameter."));
        return;
    }

    QString view = parser->value("w");
    utility->printInfo(tr("Running generation for %1 and view '%2'.").arg(componentVLNV.toString(), view));

    QString directory = parser->value("o");

    QDir targetDirectory;
    if (!targetDirectory.mkpath(directory))
    {
        utility->printError(tr("Could not create target directory: %1").arg(directory));
        return;
    }

    utility->printInfo(tr("Target directory: %1").arg(directory));

    settings_.lastViewName_ = view;    

    VLNV configurationVLNV = component->getHierRef(view);
    VLNV designVLNV;

    if (configurationVLNV.getType() == VLNV::DESIGNCONFIGURATION)
    {
        designVLNV = utility->getLibraryInterface()->getModelReadOnly(configurationVLNV).dynamicCast<const DesignConfiguration>()->getDesignRef();
    }
    else if (configurationVLNV.getType() == VLNV::DESIGN)
    {
        designVLNV = configurationVLNV;
        configurationVLNV.clear();
    }

    MessagePasser messages;

    GenerationTuple input;
    input.component = component;
    input.design = utility->getLibraryInterface()->getModel(designVLNV).dynamicCast<Design>();
    input.designConfiguration = utility->getLibraryInterface()->getModel(configurationVLNV).dynamicCast<DesignConfiguration>();
    input.messages = &messages;

    VerilogWriterFactory factory(utility->getLibraryInterface(), &messages, &settings_,
        utility->getKactusVersion(), getVersion());

    // Create model for the configuration widget.
    QSharedPointer<GenerationControl> configuration(new GenerationControl(utility->getLibraryInterface(),
        &factory, input, &settings_));

    configuration->getOutputControl()->setOutputPath(directory);
    configuration->getViewSelection()->setSaveToFileset(false);
    
    configuration->parseDocuments();
    if (!configuration->writeDocuments())
    {
        utility->printInfo(tr("Generation aborted."));
        return;
    }

    // Finally, save the changes.
    utility->printInfo(tr("Generation complete."));
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::createCommandParser()
//-----------------------------------------------------------------------------
QCommandLineParser* VerilogGeneratorPlugin::createCommandParser()
{
    QCommandLineParser* parser = new QCommandLineParser();
    QCommandLineOption helpOption = parser->addHelpOption();

    QCommandLineOption targetComponent(QStringList() << QStringLiteral("c") << QStringLiteral("top-component"), 
        QStringLiteral("The top-component VLNV separated by semicolons (:)."), QStringLiteral("VLVN"));

    QCommandLineOption targetView(QStringList() << QStringLiteral("w") << QStringLiteral("target-view"), 
        QStringLiteral("The component view to use in generation."), QStringLiteral("view name"));

    QCommandLineOption outputDirectory(QStringList() << QStringLiteral("o") << QStringLiteral("output"), 
        QStringLiteral("The output directory for generation."), QStringLiteral("path"), 
        QStringLiteral("generated_verilog"));

    parser->addOption(targetComponent);
    parser->addOption(targetView);
    parser->addOption(outputDirectory);

    return parser;
}
