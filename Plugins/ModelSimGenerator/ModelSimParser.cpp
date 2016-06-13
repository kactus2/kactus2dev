//-----------------------------------------------------------------------------
// File: ModelSimParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 10.06.2016
//
// Description:
// Parses design for ModelSim data.
//-----------------------------------------------------------------------------

#include "ModelSimParser.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/View.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <QFileInfo>

ModelSimParser::FileOptions::FileOptions():
absFilePath_(),
	libraryName_(),
	buildCommand_(),
	flags_(),
	sourceComponent_(),
	sourceFileSet_() {
}

ModelSimParser::FileOptions::FileOptions( const QString& filePath ):
absFilePath_(filePath),
	libraryName_(),
	buildCommand_(),
	flags_(),
	sourceComponent_(),
	sourceFileSet_() {
}

// the copy constructor
ModelSimParser::FileOptions::FileOptions( const FileOptions& other):
absFilePath_(other.absFilePath_.simplified()),
	libraryName_(other.libraryName_.simplified()),
	buildCommand_(other.buildCommand_.simplified()),
	flags_(other.flags_.simplified()),
	sourceComponent_(other.sourceComponent_),
	sourceFileSet_(other.sourceFileSet_) {
}

bool ModelSimParser::FileOptions::operator==( const FileOptions& other ) {

	return ((absFilePath_ == other.absFilePath_) &&
		(libraryName_ == other.libraryName_) &&
		(buildCommand_ == other.buildCommand_) &&
		(flags_ == other.flags_));
}

ModelSimParser::FileOptions& ModelSimParser::FileOptions::operator=(
	const FileOptions& other ) {

		if (this != &other) {
			absFilePath_ = other.absFilePath_.simplified();
			libraryName_ = other.libraryName_.simplified();
			buildCommand_ = other.buildCommand_.simplified();
			flags_ = other.flags_.simplified();
			sourceComponent_ = other.sourceComponent_;
			sourceFileSet_ = other.sourceFileSet_;
		}
		return *this;
}

//-----------------------------------------------------------------------------
// Function: ModelSimParser::ModelSimParser()
//-----------------------------------------------------------------------------
ModelSimParser::ModelSimParser( IPluginUtility* utility, LibraryInterface* handler ) :
	QObject(0), utility_(utility), handler_(handler), 
	sourceFiles_(QSharedPointer<QList<FileOptions> >(new QList<FileOptions>))
{
}

//-----------------------------------------------------------------------------
// Function: ModelSimParser::~ModelSimParser()
//-----------------------------------------------------------------------------
ModelSimParser::~ModelSimParser()
{
}

//-----------------------------------------------------------------------------
// Function: ModelSimParser::getParsedData()
//-----------------------------------------------------------------------------
QSharedPointer<QList<ModelSimParser::FileOptions> > ModelSimParser::getParsedData()
{
    return sourceFiles_;
}

//-----------------------------------------------------------------------------
// Function: ModelSimParser::parseFiles()
//-----------------------------------------------------------------------------
void ModelSimParser::parseFiles( QSharedPointer<Component> component, const QSharedPointer<View> view )
{
	Q_ASSERT_X(component, "ModelSimGenerator::parseFiles",
		"Null component-pointer given as parameter");

	// If it did not exist, emit an error
	if (!view)
	{
		emit errorMessage(tr("View %1 of component %2 does not exist!").arg(view->name(),
			component->getVlnv().toString()));
	}

	// inform user that component is being processed
	emit noticeMessage(tr("Processing view %1 of component %2:%3:%4:%5").arg(
		view->name()).arg(
		component->getVlnv().getVendor()).arg(
		component->getVlnv().getLibrary()).arg(
		component->getVlnv().getName()).arg(
		component->getVlnv().getVersion()));

	// If the view was found but was not hierarchical.
	if (!view->isHierarchical())
	{
		// Find referred component instantiation.
		QSharedPointer<ComponentInstantiation> cimp =
			component->getModel()->findComponentInstantiation(view->getComponentInstantiationRef());

		if ( cimp )
		{
			// Parse the file sets from the component and add them to the list of files
			parseFileSets(component, cimp->getFileSetReferences());
		}
	}

	// View was found and was hierarchical.
	else if (view->isHierarchical())
	{
		VLNV designVLNV;
		QSharedPointer<Design> design;
		QSharedPointer<DesignInstantiation> DI;

		QSharedPointer<VLNV> desConfVLNV;
		QSharedPointer<DesignConfiguration> designConf;
		QSharedPointer<DesignConfigurationInstantiation> DCI;

		// Try to find design VLNV.
		foreach ( QSharedPointer<DesignInstantiation> candidate,
			*component->getDesignInstantiations() )
		{
			if ( candidate->name() == view->getDesignInstantiationRef() )
			{
				DI = candidate;

				if ( DI->getDesignReference() )
				{
					designVLNV = *DI->getDesignReference();
				}
				break;
			}
		}

		// Try to find design configuration VLNV.
		foreach ( QSharedPointer<DesignConfigurationInstantiation> candidate,
			*component->getDesignConfigurationInstantiations() )
		{
			if ( candidate->name() == view->getDesignConfigurationInstantiationRef() )
			{
				DCI = candidate;
				desConfVLNV = DCI->getDesignConfigurationReference();
				break;
			}
		}

		// If no valid design VLNV, but component contained reference to a design configuration.
		if (!designVLNV.isValid() && desConfVLNV)
		{
			// Parse the design configuration.
			QSharedPointer<Document> libComp = handler_->getModel(*desConfVLNV);
			designConf = libComp.staticCast<DesignConfiguration>();

			// If exist, get the design reference of the configuration.
			if ( designConf )
			{
				designVLNV = designConf->getDesignRef();
			}
		}

		// Obtain the design.
		QSharedPointer<Document> libComp = handler_->getModel(designVLNV);
		design = libComp.staticCast<Design>();

		// If design does not exist, return.
		if (!design)
		{
			emit errorMessage(tr("Design %1 referenced within was not found within library."
				"Stopping generation.").arg(designVLNV.toString()));
			return;
		}

		// Read the design and its component instances.
		readDesign(design, designConf);

		// Find if referred component instantiation exists.
		QSharedPointer<ComponentInstantiation> cimp;

		foreach ( QSharedPointer<ComponentInstantiation> currentInsta,
			*component->getComponentInstantiations() )
		{
			if ( currentInsta->name() == view->getComponentInstantiationRef() )
			{
				cimp = currentInsta;
				break;
			}
		}

		// If it does exit, parse its file sets as they are used in the design.
		if (cimp)
		{
			parseFileSets(component, cimp->getFileSetReferences());
		}
	}
	view_ = view;
	sourceXml_ = handler_->getPath(component->getVlnv());
}

//-----------------------------------------------------------------------------
// Function: ModelSimParser::parseFileSets()
//-----------------------------------------------------------------------------
void ModelSimParser::parseFileSets( QSharedPointer<Component> component, 
	QSharedPointer<QStringList> fileSetNames )
{

		Q_ASSERT_X(component, "ModelSimGenerator::parseFileSets",
			"Null component-pointer given as parameter");

		QString basePath = handler_->getPath(component->getVlnv());
		if (basePath.isEmpty()) {
			emit errorMessage(tr("Component %1:%2:%3:%4 was not found within "
				"library, stopping generation").arg(
				component->getVlnv().getVendor()).arg(
				component->getVlnv().getLibrary()).arg(
				component->getVlnv().getName()).arg(
				component->getVlnv().getVersion()));
			return;
		}

		// check each file set
		foreach (QString fileSetName, *fileSetNames) {

			// if the specified file set was not found within component
			if (!component->hasFileSet(fileSetName)) {
				emit errorMessage(tr("Fileset %1 was not found within component "
					"%2:%3:%4:%5").arg(fileSetName).arg(
					component->getVlnv().getVendor()).arg(
					component->getVlnv().getLibrary()).arg(
					component->getVlnv().getName()).arg(
					component->getVlnv().getVersion()));
				continue;
			}

			// get the files in the file set
			QSharedPointer<FileSet> fileSet = component->getFileSet(fileSetName);
			QSharedPointer<QList<QSharedPointer<File> > > fileList = fileSet->getFiles();

			// handle each file
			foreach (QSharedPointer<File> file, *fileList) {

				// get the absolute path to the file
				QString absolutePath = General::getAbsolutePath(basePath,
					file->name());

				// make sure the file exists in the file system
				QFileInfo fileInfo(absolutePath);

				if (!fileInfo.exists()) {
					emit errorMessage(tr("The file %1 needed by component %2:%3:%4:%5"
						" was not found in the file system.").arg(
						absolutePath).arg(
						component->getVlnv().getVendor()).arg(
						component->getVlnv().getLibrary()).arg(
						component->getVlnv().getName()).arg(
						component->getVlnv().getVersion()));

					// move on to next file
					continue;
				}

				// create the file settings
				ModelSimParser::FileOptions fileOpt(absolutePath);

				// set the options for the file
				fileOpt.libraryName_ = file->getLogicalName();
				fileOpt.buildCommand_ = file->getCommand();
				fileOpt.flags_ = file->getFlags();
				fileOpt.sourceComponent_ = component->getVlnv().toString();
				fileOpt.sourceFileSet_ = fileSet->name();

				// if the file with options is not yet on the list, add it
				if (!sourceFiles_->contains(fileOpt))
					sourceFiles_->append(fileOpt);
			}
		}
}

//-----------------------------------------------------------------------------
// Function: ModelSimParser::readDesign()
//-----------------------------------------------------------------------------
void ModelSimParser::readDesign( const QSharedPointer<Design> design, 
	const QSharedPointer<DesignConfiguration> desConf) {
		Q_ASSERT_X(design, "ModelSimGenerator::readDesign",
			"Null Design-pointer given as parameter");

		foreach (QSharedPointer<ComponentInstance> instance, *design->getComponentInstances())
		{
			VLNV vlnv = *instance->getComponentRef();

			// if component can't be found within library
			if (!handler_->contains(vlnv))
			{
				emit errorMessage(tr("Component %1 was not found within library, stopping generation").arg(
					vlnv.toString()));
				continue;
			}

			QSharedPointer<Component> component;
			QSharedPointer<Document> libComp = handler_->getModel(vlnv);

			// if library item is component
			if (libComp->getVlnv().getType() == VLNV::COMPONENT)
			{
				component = libComp.staticCast<Component>();
			}
			else
			{
				emit errorMessage(tr("Referenced item %1 was not a component").arg(vlnv.toString()));
				continue;
			}

			// Select the active view of the target component instance.
			QSharedPointer<View> view;

			if (desConf && desConf->hasActiveView(instance->getInstanceName()))
			{
				QString viewName = desConf->getActiveView(instance->getInstanceName());
				view = component->getModel()->findView(viewName);
			}
			else
			{
				// If there was no active view, emit an error.
				emit errorMessage(tr("Component instance %1 does not have a active view!").
					arg(instance->getInstanceName()));
			}

			// call recursively to handle the component instance
			parseFiles(component, view);
		}
}

//-----------------------------------------------------------------------------
// Function: ModelSimParser::parseBlindFileSet()
//-----------------------------------------------------------------------------
void ModelSimParser::parseBlindFileSet( QSharedPointer<Component> component ) {

	Q_ASSERT_X(component, "ModelSimGenerator::parseBlindFileSet",
		"Null component pointer given as parameter");

	QString basePath = handler_->getPath(component->getVlnv());
	if (basePath.isEmpty()) {
		emit errorMessage(tr("Component %1:%2:%3:%4 was not found in library."));
		return;
	}

	// Go through all RTL files of the component.
	foreach (QSharedPointer<FileSet> fileSet, *component->getFileSets())
	{
		foreach (QSharedPointer<File> file, *fileSet->getFiles())
		{
			if ( !file->isRTLFile() )
			{
				continue;
			}

			// get the absolute path to the file
			QString absolutePath = General::getAbsolutePath(basePath,
				file->name());

			// make sure the file exists in the file system
			QFileInfo fileInfo(absolutePath);

			if (!fileInfo.exists()) {
				emit errorMessage(tr("The file %1 needed by component %2:%3:%4:%5"
					" was not found in the file system.").arg(
					absolutePath).arg(
					component->getVlnv().getVendor()).arg(
					component->getVlnv().getLibrary()).arg(
					component->getVlnv().getName()).arg(
					component->getVlnv().getVersion()));

				// move on to next file
				continue;
			}

			// create the file settings
			ModelSimParser::FileOptions fileOpt(absolutePath);

			// set the options for the file
			fileOpt.libraryName_ = file->getLogicalName();
			fileOpt.buildCommand_ = file->getCommand();
			fileOpt.flags_ = file->getFlags();
			fileOpt.sourceComponent_ = component->getVlnv().toString();
			fileOpt.sourceFileSet_ = fileSet->name();

			// if the file with options is not yet on the list, add it
			if (!sourceFiles_->contains(fileOpt))
				sourceFiles_->append(fileOpt);
		}
	}
}