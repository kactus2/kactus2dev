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

#include <Plugins/common/NameGenerationPolicy.h>

#include <Plugins/PluginSystem/IPluginUtility.h>
#include <Plugins/PluginSystem/GeneratorPlugin/GeneratorConfigurationDialog.h>

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::VerilogGeneratorPlugin()
//-----------------------------------------------------------------------------
VerilogGeneratorPlugin::VerilogGeneratorPlugin(): QObject(0), utility_(0), topComponent_(0),
    generateInterface_(false), generateMemory_(false), lastViewName_(), lastFileSetName_()
{
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
    return "1.5c";
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

    // For later use.
    utility_ = utility;
    topComponent_ = component;

    GenerationTuple input;
    input.component = component;
    input.design = design;
    input.designConfiguration = designConfiguration;

	// Try to configure the generation, if it is rejected, abort the generation.
    if (!couldConfigure(input))
    {
        utility_->printInfo(tr("Generation aborted."));
        return;
    }

	// Get the resulting configuration, it is obtained with this way to make it compatible with the tests.
    QSharedPointer<GeneratorConfiguration> configuration = getConfiguration();

    // Generation is configured: Starting the generation.
    utility_->printInfo(tr("Generation started %1.").arg(QDateTime::currentDateTime().toString(Qt::LocalDate)));
        

    // TODO: Write files
    utility_->printInfo(tr("Finished writing the file(s). TODO: Write files"));

	// The resulting file will be added to the file set.
    //addGeneratedFileToFileSet(configuration->getViewSelection(), generator.getDocuments());

    // Remember the values chosen by the user.
    generateInterface_ = configuration->getInterfaceGeneration();
    generateMemory_ = configuration->getMemoryGeneration();
    lastFileSetName_ = configuration->getViewSelection()->getFileSetName();

    if (configuration->getViewSelection()->getView())
    {
        lastViewName_ = configuration->getViewSelection()->getView()->name();
    }

    // Finally, save the changes to the affected document.
    saveChanges();
    utility_->printInfo(tr("Generation complete."));
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::findPossibleViews()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<View> > > VerilogGeneratorPlugin::findPossibleViews(
	QSharedPointer<const Component> targetComponent) const
{
	// If the generation is targeted to a component, return the flat views of the component.
	QSharedPointer<QList<QSharedPointer<View> > > views = QSharedPointer<QList<QSharedPointer<View> > >
		(new QList<QSharedPointer<View> >);

	// In practice, go through each view, and if it is not hierarchical, append to the list.
	foreach (QSharedPointer<View> view, *targetComponent->getViews())
	{
		if (!view->isHierarchical())
		{
			views->append(view);
		}
	}

    return views;
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::findPossibleViews()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<View> > > VerilogGeneratorPlugin::findPossibleViews(
    GenerationTuple input) const
{
    // Create a new list object.
    QSharedPointer<QList<QSharedPointer<View> > > views = QSharedPointer<QList<QSharedPointer<View> > >
        (new QList<QSharedPointer<View> >);

    // Go through each view of the containing component and pick the eligible ones.
    foreach(QSharedPointer<View> view, *input.component->getViews())
    {
        // Find the relevant instantiations of the inspected view.
        QSharedPointer<DesignConfigurationInstantiation> disg = input.component->getModel()->
            findDesignConfigurationInstantiation(view->getDesignConfigurationInstantiationRef());
        QSharedPointer<DesignInstantiation> dis = input.component->getModel()->
            findDesignInstantiation(view->getDesignInstantiationRef());

        // If either of the exists AND targets the correct VLNV, the view is eligible.
        if (disg && *disg->getDesignConfigurationReference() == input.designConfiguration->getVlnv()
            || dis && *dis->getDesignReference() == input.design->getVlnv())
        {
            views->append(view);
        }
    }

    // Finally, return the pickings.
    return views;
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::couldConfigure()
//-----------------------------------------------------------------------------
bool VerilogGeneratorPlugin::couldConfigure(GenerationTuple input)
{
    // If design parameter is defined, it is a generation for design.
    bool isDesignGeneration = (input.design != 0);

    QSharedPointer<QList<QSharedPointer<View> > > possibleViews;

    if (isDesignGeneration)
    {
        possibleViews = findPossibleViews(input);
    }
    else
    {
        possibleViews = findPossibleViews(input.component);
    }

    QSharedPointer<QList<QSharedPointer<ComponentInstantiation> > > possibleInstantiations =
    topComponent_->getComponentInstantiations();

    QSharedPointer<QList<QSharedPointer<FileSet> > > possibleFileSets = topComponent_->getFileSets();

    // Must have at least one eligible view.
    if (possibleViews->isEmpty())
	{
		utility_->printError(tr("No usable views in the component for generation: Hierarchical views"
			" are only usable for design generation and flat views for component generation."));
		return false;
	}

    // Initialize model for view selection.
    QSharedPointer<ViewSelection> viewSelect(
        new ViewSelection("verilog", lastViewName_, lastFileSetName_,
        possibleViews, possibleInstantiations, possibleFileSets));

    // Create model for the configuration widget.
    configuration_ = QSharedPointer<GeneratorConfiguration>(
        new GeneratorConfiguration(utility_->getLibraryInterface(), input, viewSelect, isDesignGeneration));
    // Set the defaults for convenience.
    configuration_->getFileOuput()->setOutputPath(defaultOutputPath());
    configuration_->setInterfaceGeneration(generateInterface_);
    configuration_->setMemoryGeneration(generateMemory_);

    // Create the dialog and execute: The user will ultimately accept the configuration.
    GeneratorConfigurationDialog dialog(configuration_, utility_->getParentWidget());
    return dialog.exec() == QDialog::Accepted;
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getConfiguration()
//-----------------------------------------------------------------------------
QSharedPointer<GeneratorConfiguration> VerilogGeneratorPlugin::getConfiguration()
{
    return configuration_;
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin:::onErrorReport()
//-----------------------------------------------------------------------------
void VerilogGeneratorPlugin::onErrorReport(const QString& report)
{
	utility_->printError(report);
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::defaultOutputPath()
//-----------------------------------------------------------------------------
QString VerilogGeneratorPlugin::defaultOutputPath() const
{
    QString suggestedDir = "";

    QString topComponentPath = utility_->getLibraryInterface()->getPath(topComponent_->getVlnv());
    QString xmlDir =  QFileInfo(topComponentPath).canonicalPath();
    suggestedDir = xmlDir;

    return suggestedDir;
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::relativePathForFile()
//-----------------------------------------------------------------------------
QString VerilogGeneratorPlugin::relativePathFromXmlToFile(QString const& filePath) const
{
    QString xmlPath = utility_->getLibraryInterface()->getPath(topComponent_->getVlnv());
    return General::getRelativePath(xmlPath, filePath);
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::addFileToFileSet()
//-----------------------------------------------------------------------------
void VerilogGeneratorPlugin::addGeneratedFileToFileSet(QSharedPointer<ViewSelection> configuration,
    QSharedPointer<QList<QSharedPointer<VerilogDocument> > > documents)
{
	// The view and the name of the file set are assumed to exist.
	/*Q_ASSERT (configuration->getView() && !configuration->getFileSetName().isEmpty());

	QSharedPointer<View> activeView = configuration->getView();
	QSharedPointer<ComponentInstantiation> instantiation = configuration->getInstantiation();
	QSharedPointer<FileSet> fileSet = configuration->getFileSet();

	// If the file set does not exist, create a new one with the same name.
	if (!fileSet)
	{
		fileSet = QSharedPointer<FileSet>(new FileSet);
		fileSet->setName(configuration->getFileSetName());
		topComponent_->getFileSets()->append(fileSet);
	}

    // Go through each generated file.
    foreach(QSharedPointer<VerilogDocument> doc, *documents)
    {
	    // Need a path for the file: It must be relative to the file path of the document.
	    QString filePath = relativePathFromXmlToFile(doc->filePath_);
	    // Add the new file to the file set.
	    QSettings settings;
	    QSharedPointer<File> file = fileSet->addFile(filePath, settings);

	    // If instantiation exists, make sure that the instantiation refers to the file set.
	    if (instantiation && !instantiation->getFileSetReferences()->contains(fileSet->name()))
	    {
		    instantiation->getFileSetReferences()->append(fileSet->name());
	    }

        // Insert the proper description to the file.
        insertFileDescription(file);
    }*/
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::insertFileDescription()
//-----------------------------------------------------------------------------
void VerilogGeneratorPlugin::insertFileDescription(QSharedPointer<File> file)
{
    // Get the current description.
    QString desc = file->getDescription();

    // Find the existing generation time statement.
    QRegularExpression regExp = QRegularExpression("(Generated at).+(by Kactus2. *)");
    // Rip it off.
    QRegularExpressionMatch match = regExp.match(desc);
    desc = desc.remove(match.capturedStart(), match.capturedLength());

    // Form string time and date string.
    QDateTime generationTime = QDateTime::currentDateTime();
    QString date = generationTime.date().toString("dd.MM.yyyy");
    QString time = generationTime.time().toString("hh:mm:ss");

    // Append the generation time description to the description.
    file->setDescription("Generated at " + time + " on " + date + " by Kactus2. " + desc);
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::saveChanges()
//-----------------------------------------------------------------------------
void VerilogGeneratorPlugin::saveChanges() const
{
    // The component VLNV in string format.
    QString component = topComponent_->getVlnv().toString();

    // Try to save.
    bool saveSucceeded = utility_->getLibraryInterface()->writeModelToFile(topComponent_);

    if (saveSucceeded)
    {
        // Success: Inform the user.
        utility_->printInfo(tr("Saved changes to component %1.").arg(component));
    }    
    else
    {
        // Fail: Report the error, including the path.
        QString savePath = utility_->getLibraryInterface()->getPath(topComponent_->getVlnv());
        utility_->printError(tr("Could not write component %1 to file %2.").arg(component, savePath));
    }
}