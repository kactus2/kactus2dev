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
VerilogGeneratorPlugin::VerilogGeneratorPlugin(): QObject(0), utility_(0), topComponent_(0)
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
    return "1.5b";
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
PluginSettingsWidget* VerilogGeneratorPlugin::getSettingsWidget()
{
    return new PluginSettingsWidget();
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
bool VerilogGeneratorPlugin::checkGeneratorSupport(QSharedPointer<Document const> libComp,
    QSharedPointer<Document const> /*libDesConf*/,
    QSharedPointer<Document const> /*libDes*/) const
{
    QSharedPointer<const Component> targetComponent = libComp.dynamicCast<const Component>();
    
    return targetComponent && targetComponent->getImplementation() == KactusAttribute::HW;    
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::runGenerator()
//-----------------------------------------------------------------------------
void VerilogGeneratorPlugin::runGenerator(IPluginUtility* utility, 
    QSharedPointer<Document> libComp,
    QSharedPointer<Document> libDesConf,
    QSharedPointer<Document> libDes)
{
    utility->printInfo(tr("Running %1 %2.").arg(getName(), getVersion()));

    utility_ = utility;

    if (!libComp)
    {
        utility->printError(tr("Null top component given as a parameter."));
        return;
    }

    topComponent_ = libComp.dynamicCast<Component>();

    if (!topComponent_)
    {
        utility->printError(tr("Could not cast the top component to the proper type! VLNV: %1").arg(libComp->getVlnv().toString()));
        return;
    }

    QSharedPointer<Design> design = libDes.dynamicCast<Design>();
    QSharedPointer<DesignConfiguration> designConfig = libDesConf.dynamicCast<DesignConfiguration>();

    if (libDes && !design)
    {
        utility->printError(tr("A design was specified, but could not cast it to the proper type! VLNV: %1").arg(libDes->getVlnv().toString()));
        return;
    }

    if (libDesConf && !designConfig)
    {
        utility->printError(tr("A design configuration was specified, but could not cast it to the proper type! VLNV: %1").arg(libDesConf->getVlnv().toString()));
        return;
    }

    if ((design && !designConfig) || (!design && designConfig))
    {
        utility->printError(tr("The design must be accompanied by a design configuration and vice versa."));
        return;
    }

    bool designGeneration = (design || designConfig);

    HDLComponentParser* componentParser = new HDLComponentParser(utility_->getLibraryInterface(), topComponent_);
    HDLDesignParser* designParser = 0;

    if (designGeneration)
    {
        designParser = new HDLDesignParser(utility_->getLibraryInterface(), design, designConfig);
        connect(designParser, SIGNAL(reportError(const QString&)), 
            this, SLOT(onErrorReport(const QString&)), Qt::UniqueConnection);
    }

	// Try to configure the generation, giving possible views and instantiations as parameters.
    if (!couldConfigure(findPossibleViews(topComponent_, design, designConfig),
		topComponent_->getComponentInstantiations(), topComponent_->getFileSets(), componentParser, designParser))
    {
        utility_->printInfo(tr("Generation aborted."));
        return;
    }

	// Get the resulting configuration, it is obtained with this way to make it compatible with the tests.
    QSharedPointer<GeneratorConfiguration> configuration = getConfiguration();

    utility_->printInfo(tr("Generation started %1.").arg(QDateTime::currentDateTime().toString(Qt::LocalDate)));
        
	VerilogGenerator generator(utility->getLibraryInterface(), configuration->getInterfaceGeneration(), configuration->getMemoryGeneration());
	connect(&generator, SIGNAL(reportError(const QString&)), 
		this, SLOT(onErrorReport(const QString&)), Qt::UniqueConnection);

    if (designGeneration)
    {
        QList<QSharedPointer<GenerationDesign> > designs = designParser->getParsedDesigns();
        generator.parseDesign(configuration->getOutputPath(), designs);
    }
    else
    {
        generator.parseComponent(configuration->getOutputPath(), componentParser->getParsedComponent());
    }

	generator.generate(getVersion(), utility_->getKactusVersion());

    utility_->printInfo(tr("Finished writing the file(s)."));

	// If so desired in the configuration, the resulting file will be added to the file set.
    if (configuration->getViewSelection()->getSaveToFileset())
    {
        addGeneratedFileToFileSet(configuration->getViewSelection(), generator.getDocuments());
    }

    saveChanges();

    utility_->printInfo(tr("Generation complete."));
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::findPossibleViews()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<View> > > VerilogGeneratorPlugin::findPossibleViews(
	QSharedPointer<Component> targetComponent, QSharedPointer<Design> design, QSharedPointer<DesignConfiguration> designConf) const
{
	// If the generation is targeted to a design, return views referring to the design or a design configuration.
    if (designConf && design && designConf->getDesignRef() == design->getVlnv())
    {
        return findReferencingViews(targetComponent, designConf->getVlnv());
    }
    else if (design)
    {
        return findReferencingViews(targetComponent, design->getVlnv());
    }

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
// Function: VerilogGeneratorPlugin::couldConfigure()
//-----------------------------------------------------------------------------
bool VerilogGeneratorPlugin::couldConfigure(QSharedPointer<QList<QSharedPointer<View> > > const possibleViews,
	QSharedPointer<QList<QSharedPointer<ComponentInstantiation> > > possibleInstantiations,
	QSharedPointer<QList<QSharedPointer<FileSet> > > possibleFileSets,
    HDLComponentParser* componentParser,
    HDLDesignParser* designParser)
{
    if (possibleViews->isEmpty())
	{
		utility_->printError(tr("No usable views in the component for generation: Hierarchical views"
			" are only usable for design generation and flat views for component generation."));
		return false;
	}

    QSharedPointer<ViewSelection> viewSelect(
        new ViewSelection("verilog", possibleViews, possibleInstantiations, possibleFileSets));

	viewSelect->setView(possibleViews->first());

    viewSelect->setSaveToFileset(outputFileShouldBeAddedToTopComponent());

    configuration_ = QSharedPointer<GeneratorConfiguration>(
        new GeneratorConfiguration(viewSelect, componentParser, designParser));
    configuration_->setOutputPath(defaultOutputPath());

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
    QString suggestedFile = "";

    QString topComponentPath = utility_->getLibraryInterface()->getPath(topComponent_->getVlnv());
    QString xmlDir =  QFileInfo(topComponentPath).canonicalPath();
    suggestedFile = xmlDir;

    return suggestedFile;
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::outputFileShouldBeAddedToTopComponent()
//-----------------------------------------------------------------------------
bool VerilogGeneratorPlugin::outputFileShouldBeAddedToTopComponent() const
{
    return true;
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
	Q_ASSERT (configuration->getView() && !configuration->getFileSetName().isEmpty());

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
	    // Need path for the file.
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
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::insertFileDescription()
//-----------------------------------------------------------------------------
void VerilogGeneratorPlugin::insertFileDescription(QSharedPointer<File> file)
{
    QString desc = file->getDescription();

    QRegularExpression regExp = QRegularExpression("(Generated at).+(by Kactus2. *)");
    QDateTime generationTime = QDateTime::currentDateTime();

    QRegularExpressionMatch match = regExp.match(desc);
    desc = desc.remove(match.capturedStart(), match.capturedLength());
    QString date = generationTime.date().toString("dd.MM.yyyy");
    QString time = generationTime.time().toString("hh:mm:ss");

    file->setDescription("Generated at " + time + " on " + date + " by Kactus2. " + desc);
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::saveChanges()
//-----------------------------------------------------------------------------
void VerilogGeneratorPlugin::saveChanges() const
{
    QString component = topComponent_->getVlnv().toString();

    bool saveSucceeded = utility_->getLibraryInterface()->writeModelToFile(topComponent_);            
    if (saveSucceeded)
    {
        utility_->printInfo(tr("Saved changes to component %1.").arg(component));
    }    
    else
    {
        QString savePath = utility_->getLibraryInterface()->getPath(topComponent_->getVlnv());
        utility_->printError(tr("Could not write component %1 to file %2.").arg(component, savePath));
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::findReferencingViews()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<View> > > VerilogGeneratorPlugin::
	findReferencingViews(QSharedPointer<Component> containingComponent, VLNV targetReference) const
{
	QSharedPointer<QList<QSharedPointer<View> > > views = QSharedPointer<QList<QSharedPointer<View> > >
		(new QList<QSharedPointer<View> >);
    foreach(QSharedPointer<View> view, *containingComponent->getViews())
    {
		QSharedPointer<DesignConfigurationInstantiation> disg = containingComponent->getModel()->
			findDesignConfigurationInstantiation(view->getDesignConfigurationInstantiationRef());
		QSharedPointer<DesignInstantiation> dis = containingComponent->getModel()->
			findDesignInstantiation(view->getDesignInstantiationRef());

        if (disg && *disg->getDesignConfigurationReference() == targetReference
			|| dis && *dis->getDesignReference() == targetReference)
        {
             views->append(view);
        }
    }

    return views;
}
