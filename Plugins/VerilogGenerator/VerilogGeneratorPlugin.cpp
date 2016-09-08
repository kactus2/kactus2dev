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

#include "VerilogGenerator/VerilogGenerator.h"

#include <Plugins/common/NameGenerationPolicy.h>

#include <Plugins/PluginSystem/IPluginUtility.h>
#include <Plugins/PluginSystem/GeneratorPlugin/GeneratorConfiguration.h>
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
VerilogGeneratorPlugin::VerilogGeneratorPlugin(): QObject(0), utility_(0), topComponent_(0), outputFile_()
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
    return "1.4";
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getDescription()
//-----------------------------------------------------------------------------
QString VerilogGeneratorPlugin::getDescription() const
{
    return tr("Generates a top-level Verilog module for a HW design or component.");
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
    utility_ = utility;
    topComponent_ = libComp.dynamicCast<Component>();

    utility->printInfo(tr("Running %1 %2.").arg(getName(), getVersion()));

    if (!topComponent_)
    {
        utility->printInfo(tr("Could not cast the top component to the proper type!"));
    }

	// Try to configure the generation, giving possible views and instantiations as parameters.
    if (couldConfigure(findPossibleViews(topComponent_, libDes, libDesConf),
		topComponent_->getComponentInstantiations(), topComponent_->getFileSets()))
    {
		// Get the resulting configuration, it is obtained with this way to make it compatible with the tests.
        QSharedPointer<GeneratorConfiguration> configuration = getConfiguration();

		// The configuration will know the target path for the resulting file.
        outputFile_ = configuration->getOutputPath();

        utility_->printInfo(tr("Generation started %1.").arg(QDateTime::currentDateTime().toString(Qt::LocalDate)));
        
		VerilogGenerator generator(utility->getLibraryInterface(), configuration->getInterfaceGeneration());
		connect(&generator, SIGNAL(reportError(const QString&)), 
			this, SLOT(onErrorReport(const QString&)), Qt::UniqueConnection);

        generator.parse(topComponent_, configuration->getView(), outputFile_,
			libDes.dynamicCast<Design>(), libDesConf.dynamicCast<DesignConfiguration>());

		generator.generate(outputFile_, getVersion(), utility_->getKactusVersion());

        utility_->printInfo(tr("Finished writing file %1.").arg(outputFile_));

		// If so desired in the configuration, the resulting file will be added to the file set.
        if (configuration->getSaveToFileset())
        {
            addGeneratedFileToFileSet(configuration);
        }

        insertFileDescription();


        saveChanges();

        utility_->printInfo(tr("Generation complete."));
    }    
    else
    {
        utility_->printInfo(tr("Generation aborted."));
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::insertFileDescription()
//-----------------------------------------------------------------------------
void VerilogGeneratorPlugin::insertFileDescription()
{
    QList<QSharedPointer<File> > filecands = topComponent_->getFiles(relativePathFromXmlToFile(outputFile_));

    if (filecands.size() > 0)
    {
        QSharedPointer<File> file = filecands.first();

        QString desc = file->getDescription();

        QRegularExpression regExp = QRegularExpression("(Generated at).+(by Kactus2. *)");
        QDateTime generationTime = QDateTime::currentDateTime();

        QRegularExpressionMatch match = regExp.match(desc);
        desc = desc.remove(match.capturedStart(), match.capturedLength());
        QString date = generationTime.date().toString("dd.MM.yyyy");
        QString time = generationTime.time().toString("hh:mm:ss");

        file->setDescription("Generated at " + time + " on " + date + " by Kactus2. " + desc);
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::findPossibleViews()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<View> > > VerilogGeneratorPlugin::findPossibleViews(
	QSharedPointer<Component> targetComponent, QSharedPointer<Document> libDes, QSharedPointer<Document> libDesConf) const
{
    QSharedPointer<DesignConfiguration> designConfig = libDesConf.dynamicCast<DesignConfiguration>();

	// If the generation is targeted to a design, return views referring to the design or a design configuration.
    if (designConfig && libDes && designConfig->getDesignRef() == libDes->getVlnv())
    {
        return findReferencingViews(targetComponent, designConfig->getVlnv());
    }
    else if (libDes)
    {
        return findReferencingViews(targetComponent, libDes->getVlnv());
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
	QSharedPointer<QList<QSharedPointer<FileSet> > > possibleFileSets)
{
    if (possibleViews->isEmpty())
	{
		utility_->printError(tr("No usable views in the component for generation: Hierarchical views"
			" are only usable for design generation and flat views for component generation."));
		return false;
	}

	configuration_ = QSharedPointer<GeneratorConfiguration>(new GeneratorConfiguration(
		"verilog", possibleViews,possibleInstantiations,possibleFileSets));

	configuration_->setView(possibleViews->first());

    configuration_->setOutputPath(defaultOutputPath());
    configuration_->setSaveToFileset(outputFileAndViewShouldBeAddedToTopComponent());

    GeneratorConfigurationDialog dialog(configuration_, "verilog", utility_->getParentWidget());
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
    suggestedFile = xmlDir + "/" + topComponent_->getVlnv().getName() + ".v";

    return suggestedFile;
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::fileShouldBeAddedToFileset()
//-----------------------------------------------------------------------------
bool VerilogGeneratorPlugin::outputFileAndViewShouldBeAddedToTopComponent() const
{
    QString filePath = relativePathFromXmlToFile(outputFile_);
    return !topComponent_->hasFile(filePath);
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
void VerilogGeneratorPlugin::addGeneratedFileToFileSet(QSharedPointer<GeneratorConfiguration> configuration) const
{
	// The view and the names of the instantiation and file set are assumed to exist.
	Q_ASSERT (configuration->getView() && !configuration->getFileSetName().isEmpty() && 
		!configuration->getInstantiationName().isEmpty());

	QSharedPointer<View> activeView = configuration->getView();
	QSharedPointer<ComponentInstantiation> instantiation = configuration->getInstantiation();
	QSharedPointer<FileSet> fileSet = configuration->getFileSet();
	
	// If the component instantiation does not exist, create a new one with the same name.
	if (!instantiation)
	{
		instantiation = QSharedPointer<ComponentInstantiation>(new ComponentInstantiation);
		instantiation->setName(configuration->getInstantiationName());
		topComponent_->getComponentInstantiations()->append(instantiation);
	}

	// If the file set does not exist, create a new one with the same name.
	if (!fileSet)
	{
		fileSet = QSharedPointer<FileSet>(new FileSet);
		fileSet->setName(configuration->getFileSetName());
		topComponent_->getFileSets()->append(fileSet);
	}

	// Need path for the file.
	QString filePath = relativePathFromXmlToFile(outputFile_);
	// Add the new file to the file set.
	QSettings settings;
	fileSet->addFile(filePath, settings);

	// Make sure that the instantiation language is verilog.
	instantiation->setLanguage("verilog");

	// Make sure that the instantiation refers to the file set.
	if (!instantiation->getFileSetReferences()->contains(fileSet->name()))
	{
		instantiation->getFileSetReferences()->append(fileSet->name());
	}

	// Make sure that the view refers to component instantiation.
	activeView->setComponentInstantiationRef(instantiation->name());
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
			findDesignConfigurationInstantiation( view->getDesignConfigurationInstantiationRef() );
		QSharedPointer<DesignInstantiation> dis = containingComponent->getModel()->
			findDesignInstantiation( view->getDesignInstantiationRef() );

        if (disg && *disg->getDesignConfigurationReference() == targetReference
			|| dis && *dis->getDesignReference() == targetReference )
        {
             views->append(view);
        }
    }

    return views;
}
