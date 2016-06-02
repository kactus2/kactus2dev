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

#include <Plugins/PluginSystem/IPluginUtility.h>
#include <Plugins/PluginSystem/GeneratorPlugin/GeneratorConfiguration.h>
#include <Plugins/PluginSystem/GeneratorPlugin/GeneratorConfigurationDialog.h>

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/Component/FileSet.h>

#include <IPXACTmodels/Design/Design.h>

#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>
#include "IPXACTmodels/Component/View.h"
#include "IPXACTmodels/Component/ComponentInstantiation.h"
#include "../common/NameGenerationPolicy.h"

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
    static QString name(tr("Verilog Generator"));
    return name;
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getVersion()
//-----------------------------------------------------------------------------
QString VerilogGeneratorPlugin::getVersion() const
{
    static QString version(tr("1.1"));
    return version;
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getDescription()
//-----------------------------------------------------------------------------
QString VerilogGeneratorPlugin::getDescription() const
{
    static QString description(tr("Generates a top-level Verilog module for a HW design or component."));
    return description;
}
//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getVendor()
//-----------------------------------------------------------------------------
QString VerilogGeneratorPlugin::getVendor() const
{
    static QString vendor(tr("TUT"));
    return vendor;
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getLicence()
//-----------------------------------------------------------------------------
QString VerilogGeneratorPlugin::getLicence() const
{
    static QString licence(tr("GPL2"));
    return licence;
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getLicenceHolder()
//-----------------------------------------------------------------------------
QString VerilogGeneratorPlugin::getLicenceHolder() const
{
    static QString holder(tr("Public"));
    return holder;
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

    if (couldConfigure(findPossibleViewNames(libComp, libDes, libDesConf)))
    {
        QSharedPointer<GeneratorConfiguration> configuration = getConfiguration();

        outputFile_ = configuration->getOutputPath();

        utility_->printInfo(tr("Generation started %1.").arg(QDateTime::currentDateTime().toString(Qt::LocalDate)));
        
        VerilogGenerator generator(utility->getLibraryInterface());
        generator.parse(topComponent_, configuration_->getActiveViewName(), libDes.dynamicCast<Design>());
        generator.generate(outputFile_);

        utility_->printInfo(tr("Finished writing file %1.").arg(outputFile_));

        if (configuration->getSaveToFileset())
        {          
            QString viewName = configuration->getActiveViewName();
            addGeneratedFileToFileSet(viewName);
            addRTLViewToTopComponent(viewName);
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
// Function: VerilogGeneratorPlugin::findPossibleViewNames()
//-----------------------------------------------------------------------------
QStringList VerilogGeneratorPlugin::findPossibleViewNames(QSharedPointer<Document> libComp,
    QSharedPointer<Document> libDes, QSharedPointer<Document> libDesConf) const
{
    QSharedPointer<Component> topComponent = libComp.dynamicCast<Component>();
    QSharedPointer<DesignConfiguration> designConfig = libDesConf.dynamicCast<DesignConfiguration>();

    QStringList viewNames;
    if (designConfig && libDes && designConfig->getDesignRef() == libDes->getVlnv())
    {
        viewNames = findReferencingViews(topComponent, designConfig->getVlnv());
    }
    else if (libDes)
    {
        viewNames = findReferencingViews(topComponent, libDes->getVlnv());
    }
    else
    {
        viewNames = topComponent->getFlatViews();
    }

    return viewNames;
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::couldConfigure()
//-----------------------------------------------------------------------------
bool VerilogGeneratorPlugin::couldConfigure(QStringList const& possibleViewNames) const
{
    if (!possibleViewNames.isEmpty())
    {
        configuration_->setActiveView(possibleViewNames.first());
    }

    configuration_->setOutputPath(defaultOutputPath());
    configuration_->setSaveToFileset(outputFileAndViewShouldBeAddedToTopComponent());

    GeneratorConfigurationDialog dialog(configuration_, utility_->getParentWidget());
    dialog.setViewNames(possibleViewNames);
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
void VerilogGeneratorPlugin::addGeneratedFileToFileSet(QString const& activeViewName) const
{
    QString filePath = relativePathFromXmlToFile(outputFile_);

    QSettings settings;
	QString fileSetName = fileSetNameForActiveView(activeViewName);
    QSharedPointer<FileSet> fileSet = topComponent_->getFileSet(fileSetName);

	if ( !fileSet )
	{
		fileSet = QSharedPointer<FileSet>( new FileSet );
		fileSet->setName(fileSetName);
		topComponent_->getFileSets()->append(fileSet);

		QSharedPointer<ComponentInstantiation> cimp( new ComponentInstantiation );
		cimp->setName( fileSetName + "_instantiation" );
		topComponent_->getComponentInstantiations()->append(cimp);
		cimp->getFileSetReferences()->append(fileSet->name());

		QSharedPointer<View> activeView = topComponent_->getModel()->findView( activeViewName );

		if ( activeView )
		{
			activeView->setComponentInstantiationRef( cimp->name() );
		}
	}

    fileSet->addFile(filePath, settings);
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::fileSetNameForActiveView()
//-----------------------------------------------------------------------------
QString VerilogGeneratorPlugin::fileSetNameForActiveView(QString const& activeViewName) const
{
    QSharedPointer<View> activeView = topComponent_->getModel()->findView(activeViewName);
    if (activeView && activeView->isHierarchical())
    {
        return activeViewName + "_verilogSource";
    }
    else
    {
        return "verilogSource";
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::addRTLViewToTopComponent()
//-----------------------------------------------------------------------------
void VerilogGeneratorPlugin::addRTLViewToTopComponent(QString const& activeViewName) const
{    
    QSharedPointer<View> rtlView;

    QSharedPointer<View> activeView = topComponent_->getModel()->findView(activeViewName);

    if (activeView)
    {    
        if (activeView->isHierarchical())
		{
			rtlView = QSharedPointer<View>( new View() );    
			QString structuralViewName = NameGenerationPolicy::verilogStructuralViewName( activeViewName );
            rtlView->setName(structuralViewName);

            topComponent_->getViews()->append(rtlView);
        }
        else
        {
            rtlView = activeView;
        }
    }
    else
    {
        rtlView = QSharedPointer<View>( new View() );
        rtlView->setName("rtl");
        topComponent_->getViews()->append(rtlView);
	}

	// Needs a component instantiation.
	QSharedPointer<ComponentInstantiation> cimp =
		topComponent_->getModel()->findComponentInstantiation( rtlView->getComponentInstantiationRef() );

	if ( !cimp )
	{
		// If the view has no component instantiation, make new one.
		cimp = QSharedPointer<ComponentInstantiation>( new ComponentInstantiation );
		cimp->setName(rtlView->name() + "_instantiation");
		rtlView->setComponentInstantiationRef( cimp->name() );
		topComponent_->getComponentInstantiations()->append(cimp);
	}

	// Set language
	cimp->setLanguage("verilog");

	// Form name for the file set reference.
	QString fileSetRef = fileSetNameForActiveView(activeViewName);

	// Append to the list, if does not exist.
	if ( !cimp->getFileSetReferences()->contains( fileSetRef ) )
	{
		cimp->getFileSetReferences()->append( fileSetRef );
	}

	// This is environment identifiers.
	QString envId = QString("verilog:Kactus2:");

	QStringList envIds = rtlView->getEnvIdentifiers();

	// If does not exist within the view, append to the list.
	if ( !envIds.contains(envId) )
	{
		envIds.append(envId);
		rtlView->setEnvIdentifiers(envIds);
	}
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
QStringList VerilogGeneratorPlugin::findReferencingViews(QSharedPointer<Component> containingComponent, 
    VLNV targetReference) const
{
    QStringList hierViews;
    foreach(QSharedPointer<View> view, *containingComponent->getViews())
    {
		QSharedPointer<DesignConfigurationInstantiation> disg = containingComponent->getModel()->
			findDesignConfigurationInstantiation( view->getDesignConfigurationInstantiationRef() );
		QSharedPointer<DesignInstantiation> dis = containingComponent->getModel()->
			findDesignInstantiation( view->getDesignInstantiationRef() );

        if (disg && *disg->getDesignConfigurationReference() == targetReference
			|| dis && *dis->getDesignReference() == targetReference )
        {
             hierViews.append(view->name());
        }
    }

    return hierViews;
}
