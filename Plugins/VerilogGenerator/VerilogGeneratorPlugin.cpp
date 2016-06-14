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
VerilogGeneratorPlugin::VerilogGeneratorPlugin(): QObject(0), utility_(0), topComponent_(0), outputFile_(),
    configuration_(new GeneratorConfiguration())
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
    return "1.2";}

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

	// Create a map of instantiations.
	QSharedPointer<QMap<QString,QSharedPointer<ComponentInstantiation> > > instantiations =
		QSharedPointer<QMap<QString,QSharedPointer<ComponentInstantiation> > >
		( new QMap<QString,QSharedPointer<ComponentInstantiation> > );

	// Populate it with all component instantiations within the top component, keyed by their names.
	foreach ( QSharedPointer<ComponentInstantiation> cimp,
		*topComponent_->getComponentInstantiations() )
	{
		instantiations->insert(cimp->name(), cimp);
	}

	// Try to configure the generation, giving possible views and instantiations as parameters.
    if (couldConfigure(findPossibleViews(libComp, libDes, libDesConf),
		topComponent_->getComponentInstantiations()))
    {
		// Get the resulting configuration, it is obtained with this way to make it compatible with the tests.
        QSharedPointer<GeneratorConfiguration> configuration = getConfiguration();

		// The configuration will know the target path for the resulting file.
        outputFile_ = configuration->getOutputPath();

        utility_->printInfo(tr("Generation started %1.").arg(QDateTime::currentDateTime().toString(Qt::LocalDate)));
        
		VerilogGenerator generator(utility->getLibraryInterface());
		connect(&generator, SIGNAL(reportError(const QString&)), 
			this, SLOT(onErrorReport(const QString&)), Qt::UniqueConnection);
        generator.parse(topComponent_, configuration->getActiveView()->name(), outputFile_, libDes.dynamicCast<Design>());
		generator.generate(outputFile_);

        utility_->printInfo(tr("Finished writing file %1.").arg(outputFile_));

		// If so desired in the configuration, the resulting file will be added to the file set.
        if (configuration->getSaveToFileset())
        {
            addGeneratedFileToFileSet(configuration->getActiveView(), configuration->getInstantiation(), 
				configuration->getFileSetRef());
            saveChanges();
        }

        utility_->printInfo(tr("Generation complete."));
    }    
    else
    {
        utility_->printInfo(tr("Generation aborted."));
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::findPossibleViews()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<View> > > VerilogGeneratorPlugin::findPossibleViews(
	QSharedPointer<Document> libComp, QSharedPointer<Document> libDes, QSharedPointer<Document> libDesConf) const
{
    QSharedPointer<Component> topComponent = libComp.dynamicCast<Component>();
    QSharedPointer<DesignConfiguration> designConfig = libDesConf.dynamicCast<DesignConfiguration>();

    QStringList viewNames;
    if (designConfig && libDes && designConfig->getDesignRef() == libDes->getVlnv())
    {
        return findReferencingViews(topComponent, designConfig->getVlnv());
    }
    else if (libDes)
    {
        return findReferencingViews(topComponent, libDes->getVlnv());
    }
    else
    {
        viewNames = topComponent->getFlatViews();
	}

	QSharedPointer<QList<QSharedPointer<View> > > views = QSharedPointer<QList<QSharedPointer<View> > >
		(new QList<QSharedPointer<View> >);

	foreach (QSharedPointer<View> view, *topComponent->getViews())
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
	QSharedPointer<QList<QSharedPointer<ComponentInstantiation> > > possibleInstantiations) const
{
    if (!possibleViews->isEmpty())
    {
        configuration_->setActiveView(possibleViews->first());
	}

    configuration_->setOutputPath(defaultOutputPath());
    configuration_->setSaveToFileset(outputFileAndViewShouldBeAddedToTopComponent());

    GeneratorConfigurationDialog dialog(configuration_, possibleViews, possibleInstantiations, utility_->getParentWidget());
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
void VerilogGeneratorPlugin::addGeneratedFileToFileSet(QSharedPointer<View> activeView,
	QSharedPointer<ComponentInstantiation> instantiation,
	QString fileSetName) const
{
	// Must have name for the file set.
	if (fileSetName.isEmpty())
	{
		utility_->printError("Tried to add generated verilog file to a file set with no name!");
		return;
	}

	// The view and the instantiation are assumed to exist.
	Q_ASSERT (activeView && instantiation);

	// Need path for the file.
	QString filePath = relativePathFromXmlToFile(outputFile_);

	// Connect the file set name to an existing file set.
	QSharedPointer<FileSet> fileSet = topComponent_->getFileSet(fileSetName);

	// If none exist, create a new one with the same name.
	if (!fileSet)
	{
		fileSet = QSharedPointer<FileSet>(new FileSet);
		fileSet->setName(fileSetName);
		topComponent_->getFileSets()->append(fileSet);
	}

	// Add file to the file set.
	QSettings settings;
	fileSet->addFile(filePath, settings);

	// Make sure that the language is verilog.
	instantiation->setLanguage("verilog");

	// Make sure that the instantiation refers to the file set.
	if (!instantiation->getFileSetReferences()->contains(fileSet->name()))
	{
		instantiation->getFileSetReferences()->append(fileSet->name());
	}

	// Make sure that the instantiation is within the component.
	if (!topComponent_->getComponentInstantiations()->contains(instantiation))
	{
		topComponent_->getComponentInstantiations()->append(instantiation);
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
