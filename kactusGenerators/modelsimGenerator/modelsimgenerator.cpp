/* 
 *
 *  Created on: 7.12.2010
 *      Author: Antti Kamppi
 */

#include "modelsimgenerator.h"

#include <IPXACTmodels/vlnv.h>
#include <library/LibraryManager/libraryhandler.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/DesignInstantiation.h>
#include <IPXACTmodels/Component/DesignConfigurationInstantiation.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>
#include <IPXACTmodels/librarycomponent.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <common/dialogs/comboSelector/comboselector.h>

#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QList>
#include <QTextStream>
#include <QSharedPointer>
#include "IPXACTmodels/Component/ComponentInstantiation.h"

ModelsimGenerator::FileOptions::FileOptions():
absFilePath_(),
libraryName_(),
buildCommand_(),
flags_(),
sourceComponent_(),
sourceFileSet_() {
}

ModelsimGenerator::FileOptions::FileOptions( const QString& filePath ):
absFilePath_(filePath),
libraryName_(),
buildCommand_(),
flags_(),
sourceComponent_(),
sourceFileSet_() {
}

// the copy constructor
ModelsimGenerator::FileOptions::FileOptions( const FileOptions& other):
absFilePath_(other.absFilePath_.simplified()),
libraryName_(other.libraryName_.simplified()),
buildCommand_(other.buildCommand_.simplified()),
flags_(other.flags_.simplified()),
sourceComponent_(other.sourceComponent_),
sourceFileSet_(other.sourceFileSet_) {
}

bool ModelsimGenerator::FileOptions::operator==( const FileOptions& other ) {

	return ((absFilePath_ == other.absFilePath_) &&
		(libraryName_ == other.libraryName_) &&
		(buildCommand_ == other.buildCommand_) &&
		(flags_ == other.flags_));
}

ModelsimGenerator::FileOptions& ModelsimGenerator::FileOptions::operator=(
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


ModelsimGenerator::ModelsimGenerator(LibraryInterface *handler, QWidget *parent): 
QObject(parent),
parent_(parent),
handler_(handler),
sourceFiles_(),
sourceXml_() {

	Q_ASSERT_X(handler, "ModelsimGenerator constructor",
			"Invalid LibraryHandler pointer given as parameter");
}

ModelsimGenerator::~ModelsimGenerator() {
}

void ModelsimGenerator::generateMakefile(QString outputPath) {

	Q_ASSERT_X(handler_, "ModelsimGenerator::generateMakefile()",
			"Invalid LibraryHandler pointer");

	// if the outputPath is not defined
	if (outputPath.isEmpty()) {
		emit errorMessage(tr("No output path specified"));
		return;
	}

	QFile file(outputPath);

	// if previous file exists then remove it.
	if (file.exists()) {
		file.remove();
	}

	// open the file and erase all old contents if any exists

	// if file could not be opened
	if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly)) {

		// throw an exception that file couldn't be opened
		QString message(tr("File: "));
		message += outputPath;
		message += tr(" couldn't be opened for writing");
		emit errorMessage(message);

		return;
	}
	// now file has been opened for writing

	QTextStream stream(&file);

	writeMakefile(stream);

	return;

}

bool ModelsimGenerator::addMakefile2IPXact(QSharedPointer<Component> component, 
										   const QString& makefilePath,
										   const QString& basePath) {
	Q_ASSERT_X(component, "ModelsimGenerator::addMakefile2IPXact",
		"Null component pointer given as parameter");
	Q_ASSERT_X(!makefilePath.isEmpty(), "ModelsimGenerator::addMakefile2IPXact",
		"Empty makefile path given as parameter");
	Q_ASSERT_X(!basePath.isEmpty(), "ModelsimGenerator::addMakefile2IPXact",
		"Empty base path given as parameter");

	// get the relative path to add to file set
	QString relativePath = General::getRelativePath(basePath, makefilePath);

	QString fileSetName("ModelsimScripts");
	QSharedPointer<FileSet> modelsimFileSet = component->getFileSet(fileSetName);

	// if the top vhdl file set was not found. Create one
	if (!modelsimFileSet) {
		modelsimFileSet = QSharedPointer<FileSet>(new FileSet(fileSetName, QString("simulation")));
		component->getFileSets()->append(modelsimFileSet);
	}

	QSettings settings;

	// create a new file
	QSharedPointer<File> scriptFile = modelsimFileSet->addFile(relativePath, settings);
	scriptFile->setIncludeFile(false);
	scriptFile->setDescription(tr(
		"Script file for Modelsim that compiles all files for view %1.").arg(viewName_));

	return true;
}

void ModelsimGenerator::writeMakefile(QTextStream& stream) {

	stream << tr("# Script compiles all vhdl-files and generates a makefile for"
			" them") << endl;
	stream << tr("# This script is tested for Modelsim version 6.6a ") << endl;
	stream << tr("# Based on file %1").arg(sourceXml_);
	stream << tr(" and its view \"%1\"").arg(viewName_) << endl << endl;

	stream << ".main clear" << endl << endl;

	// inform user when executing the script
	stream << "echo " << tr("\" Generating libraries for files\"") << endl;

	// the top component's library is already written
	QStringList processedLibraries;
	QString activeComponent;
	QString activeFileSet;
	foreach (ModelsimGenerator::FileOptions fileOpt, sourceFiles_) {
		
		// if this is new component
		if (fileOpt.sourceComponent_ != activeComponent) {
			stream << endl;
			activeComponent = fileOpt.sourceComponent_;
			stream << "echo " << tr("\"Processing component %1\"").arg(activeComponent) << endl;

			// component changed so set active file set to null
			activeFileSet.clear();
		}
		// if this is new file set on the component
		if (fileOpt.sourceFileSet_ != activeFileSet) {
			activeFileSet = fileOpt.sourceFileSet_;
			stream << "echo " << tr("\"Processing file set %1 of component %2.\"").arg(
				activeFileSet).arg(activeComponent) << endl;
		}

		// if library has not been used before and it's name is not empty
		if (!processedLibraries.contains(fileOpt.libraryName_) &&
			!fileOpt.libraryName_.isEmpty()) {

			stream << "echo " << tr("\" Adding library %1\"").arg(fileOpt.libraryName_) << endl;
			processedLibraries.append(fileOpt.libraryName_);

			stream << "vlib " << fileOpt.libraryName_ << endl;
		}

		// write the compile command
		stream << fileOpt.buildCommand_ << " " << fileOpt.flags_ << " " <<
			fileOpt.absFilePath_ << endl;
	}

	stream << endl;
	stream << "echo " << tr("\" Creating a new Makefile\"") << endl << endl;

	stream << tr("# remove the old makefile") << endl;
	stream << "rm -f Makefile" << endl;
	stream << "vmake work > Makefile" << endl;

	// inform user that script has been successfully executed
	stream << "echo " << tr("\" Script has been executed \"") << endl;
}

void ModelsimGenerator::parseFiles( QSharedPointer<Component> component, const QString& viewName ) {

	Q_ASSERT_X(component, "ModelsimGenerator::parseFiles",
		"Null component-pointer given as parameter");

	// inform user that component is being processed
	emit noticeMessage(tr("Processing view %1 of component %2:%3:%4:%5").arg(
		viewName).arg(
		component->getVlnv().getVendor()).arg(
		component->getVlnv().getLibrary()).arg(
		component->getVlnv().getName()).arg(
		component->getVlnv().getVersion()));

	// pointer to the named view under given component
	QSharedPointer<View> view;
	
	foreach ( QSharedPointer<View> currentView, *component->getViews() )
	{
		if ( currentView->name() == viewName_ )
		{
			view = currentView;
			break;
		}
	}

	// if view was not found
	if (!view) {

		// if component does not contain any views
		if (component->getViews()->size() == 0) {
			emit noticeMessage(tr("Component %1 didn't contain any views, "
				"adding all found RTL-files from component file sets.").arg(
				component->getVlnv().getName()));

			parseBlindFileSet(component);
			viewName_ = viewName;
			return;
		}

		// component did contain views but not the specified view
		else {
			emit errorMessage(tr("View %1 not found within component %2").arg(
				viewName).arg(component->getVlnv().getName()));
			return;
		}
	}

	// if the view was found but was not hierarchical
	if (view && !view->isHierarchical())
	{
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

		if ( cimp )
		{
			// Parse the file sets from the component and add them to the list of files
			parseFileSets(component, cimp->getFileSetReferences());
		}
	}

	// view was found and was hierarchical
	else if (view && view->isHierarchical())
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
	viewName_ = viewName;
	sourceXml_ = handler_->getPath(component->getVlnv());
}

void ModelsimGenerator::parseFileSets( QSharedPointer<Component> component, 
									  QSharedPointer<QStringList> fileSetNames ) {

	Q_ASSERT_X(component, "ModelsimGenerator::parseFileSets",
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
			ModelsimGenerator::FileOptions fileOpt(absolutePath);

			// set the options for the file
			fileOpt.libraryName_ = file->getLogicalName();
			fileOpt.buildCommand_ = file->getCommand();
			fileOpt.flags_ = file->getFlags();
			fileOpt.sourceComponent_ = component->getVlnv().toString();
			fileOpt.sourceFileSet_ = fileSet->name();

			// if the file with options is not yet on the list, add it
			if (!sourceFiles_.contains(fileOpt))
				sourceFiles_.append(fileOpt);
		}
	}
}

void ModelsimGenerator::readDesign( const QSharedPointer<Design> design, 
									const QSharedPointer<DesignConfiguration> desConf) {
	Q_ASSERT_X(design, "ModelsimGenerator::readDesign",
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

		QString viewName;
		
		// if design configuration is used
		if (desConf && desConf->hasActiveView(instance->getInstanceName()))
        {
			viewName = desConf->getActiveView(instance->getInstanceName());
		}
		// if design configuration is not used or view was not found
		else
        {
			// if component only has one view
			if (component->getViews()->size() == 1)
            {
				viewName = component->getViews()->first()->name();
            }
			// if component has multiple views
			else if (component->getViews()->size() > 1)
            {				
				// ask user to select the view
				viewName = ComboSelector::selectView(component, parent_);

				if (viewName.isEmpty())
                {
					emit noticeMessage(tr("No view selected for component %1, skipping").arg(vlnv.toString()));
					continue;
				}
			}
			
		}
		
		// call recursively to handle the component instance
		parseFiles(component, viewName);
	}
}

void ModelsimGenerator::parseBlindFileSet( QSharedPointer<Component> component ) {

	Q_ASSERT_X(component, "ModelsimGenerator::parseBlindFileSet",
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
			ModelsimGenerator::FileOptions fileOpt(absolutePath);

			// set the options for the file
			fileOpt.libraryName_ = file->getLogicalName();
			fileOpt.buildCommand_ = file->getCommand();
			fileOpt.flags_ = file->getFlags();
			fileOpt.sourceComponent_ = component->getVlnv().toString();
			fileOpt.sourceFileSet_ = fileSet->name();

			// if the file with options is not yet on the list, add it
			if (!sourceFiles_.contains(fileOpt))
				sourceFiles_.append(fileOpt);
		}
	}
}

