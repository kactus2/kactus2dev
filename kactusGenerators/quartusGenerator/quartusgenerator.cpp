/* 
 *
 *  Created on: 24.11.2010
 *      Author: Antti Kamppi
 */

#include "quartusgenerator.h"

#include <library/LibraryManager/vlnv.h>
#include <library/LibraryManager/libraryhandler.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/designconfiguration.h>
#include <IPXACTmodels/librarycomponent.h>
#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/file.h>
#include <IPXACTmodels/fileset.h>
#include <IPXACTmodels/view.h>

#include <common/dialogs/comboSelector/comboselector.h>

#include <QObject>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QList>
#include <QDateTime>
#include <QIODevice>
#include <stdexcept>
#include <QFileInfo>

QuartusGenerator::QuartusGenerator(LibraryInterface *handler, QWidget *parent):
QObject(parent), 
files_(), 
assignments_(),  
handler_(handler),
parent_(parent)
{
	Q_ASSERT_X(handler, "QuartusGenerator constructor",
			"Null LibraryHandler pointer given for generator");
}

QuartusGenerator::~QuartusGenerator() {
}

void QuartusGenerator::setBoardName(QSharedPointer<Component> component) {
	// clear the old assignments
	assignments_.clear();

	// find the file wrapper that holds the assignments
	File* fileWrapper = component->getQuartusPinmap();

	// if theres no fileset for quartus projects
	if (!fileWrapper) {
		emit noticeMessage(tr("No pin map found on the component"));
		return;
	}

	QString pinmapPath = General::getAbsolutePath(handler_->getPath(
			*component->getVlnv()), fileWrapper->getName());
	
	// if the file did not exist
	QFileInfo pinmapInfo(pinmapPath);
	if (!pinmapInfo.exists()) {
		emit errorMessage(tr("The pin map file %1 was not found in file system").arg(
			pinmapPath));
		return;
	}

	// create a text stream to read the pin mappings
	QFile mappingsFile(pinmapPath);

	// if the file can not be opened for reading
	if (!mappingsFile.open(QIODevice::ReadOnly)) {
		emit errorMessage(tr("File %1 could not be opened for reading").arg(
			pinmapPath));
		return;
	}
	QTextStream stream(&mappingsFile);

	// read the assignments from the stream
	readAssignments(stream);

	mappingsFile.close();
}

void QuartusGenerator::readAssignments(QTextStream& stream) {

	QString line;

	// as long as theres stuff to read left
	while (!stream.atEnd()) {

		// read a line from the stream
		line = stream.readLine();

		// split the line wherever there is whitespace
		QStringList lineParameters = line.split(QString(" "),
				QString::SkipEmptyParts);

		// get all assignments from the quartus settings file
		//if (lineParameters.at(0) == QString("set_location_assignment")) {
			assignments_.append(line);
		//}
	}
}

void QuartusGenerator::generateProject(QString outputPath, const QString& top_entity) {

	// make sure the outputPath is a directory
	QFileInfo outPutInfo(outputPath);

	// if was not a directory
	if (!outPutInfo.isDir())
		outputPath = outPutInfo.absolutePath();

	// if the output dir doesn't exist then create it
	QDir outputDir(outputPath);
	if (!outputDir.exists()) {
		outputDir.mkpath(outputPath);
	}

	// write the project file
	writeQPF(outputPath, top_entity);

	// write the settings file
	writeQSF(outputPath, top_entity);
}

void QuartusGenerator::writeQPF(const QString& outPutDir, const QString& top_entity) {

	// save previous working directory
	QDir savedCurrentDir = QDir::current();

	// set the working directory
	QDir::setCurrent(outPutDir);

	// create file instance for the project file
	QString fileName = top_entity;
	fileName.remove(".comp");
	fileName += QString(".qpf");
	QFile qpfFile(fileName);

	// if file can't be opened for writing
	if (!qpfFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		QString msg(tr("Can't open file "));
		msg += qpfFile.fileName();
		msg += tr(" for writing");
		throw std::runtime_error(msg.toStdString());
	}

	QTextStream stream(&qpfFile);

	// write the header for project file
	writeHeader(stream);

	QString entityName = top_entity;
	entityName.remove(".comp");

	// write project info
	stream << "QUARTUS_VERSION = \"10.0\"" << endl << endl;
	stream << "# Revisions" << endl << endl;
	stream << "PROJECT_REVISION = \"";
	stream << entityName;
	stream << "\"" << endl;

	qpfFile.close();

	// restore previous working directory
	QDir::setCurrent(savedCurrentDir.path());
}

void QuartusGenerator::writeQSF(const QString& outputDir, const QString& top_entity) {
	// save previous working directory
	QDir savedCurrentDir = QDir::current();

	// set the working directory
	QDir::setCurrent(outputDir);

	// create file instance for the project file
	QString fileName = top_entity;
	fileName.remove(".comp");
	fileName += QString(".qsf");
	QFile qsfFile(fileName);

	// if file can't be opened for writing
	if (!qsfFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		QString msg(tr("Can't open file "));
		msg += qsfFile.fileName();
		msg += tr(" for writing");
		emit errorMessage(msg);
		return;
	}

	QTextStream stream(&qsfFile);

	// write the header for the file
	writeHeader(stream);

	// write the top level entity
	stream << "set_global_assignment -name TOP_LEVEL_ENTITY " << top_entity << endl;

	// write the Quartus version
	stream << "set_global_assignment -name ORIGINAL_QUARTUS_VERSION "
		"\"10.0 SP1\"" << endl;

	// write the files
	for (int i = 0; i < files_.size(); ++i) {

		QFileInfo fileInfo(files_.at(i));

		// if file is vhdl-file
		if (fileInfo.suffix() == QString("vhd")
            || fileInfo.suffix() == QString("vhdl") ) {
			stream << "set_global_assignment -name VHDL_FILE " << files_.at(i)
				<< endl;
		}
		// if file is verilog file
		else if (fileInfo.suffix() == QString("v")) {
			stream << "set_global_assignment -name VERILOG_FILE " << files_.at(i)
				<< endl;
		}
		
		// if file is quartus ip file (.qip)
		else if (fileInfo.suffix() == QString("qip")) {
			stream << "set_global_assignment -name QIP_FILE " << files_.at(i)
				<< endl;
		}

		//if file is synopsys design constraints file (.sdc)
		else if (fileInfo.suffix() == QString("sdc")) {
			stream << "set_global_assignment -name SDC_FILE " << files_.at(i)
				<< endl;
		}	

	}

	// write the pin assignments
	for (int i = 0; i < assignments_.size(); ++i) {
		stream << assignments_.at(i) << endl;
	}

	qsfFile.close();
}

void QuartusGenerator::writeHeader(QTextStream& stream) {
	stream << "# ----------------------------------------------------------- #"
			<< endl << endl;
	stream << tr("# Quartus project generated by Kactus2") << endl;
	stream << tr("# Date created ") << QDateTime::currentDateTime().toString(
			QString("hh:mm:ss dd.MM.yyyy")) << endl << endl;
	stream << "# ----------------------------------------------------------- #"
			<< endl << endl;
}


void QuartusGenerator::parseFiles(QSharedPointer<Component> component, const QString& viewName) {

	Q_ASSERT_X(component, "QuartusGenerator::parseFiles",
		"Null component-pointer given as parameter");

	// inform user that component is being processed
	emit noticeMessage(tr("Processing view %1 of component %2:%3:%4:%5").arg(
		viewName).arg(
		component->getVlnv()->getVendor()).arg(
		component->getVlnv()->getLibrary()).arg(
		component->getVlnv()->getName()).arg(
		component->getVlnv()->getVersion()));

	// pointer to the named view under given component
	View* view = component->findView(viewName);

	// if view was not found
	if (!view) {

		// if component does not contain any views
		if (component->viewCount() == 0) {
			emit noticeMessage(tr("Component %1 didn't contain any views, "
				"adding all found RTL-files from component file sets.").arg(
				component->getVlnv()->getName()));

			parseBlindFileSet(component);
			return;
		}

		// component did contain views but not the specified view
		else {
			emit errorMessage(tr("View %1 not found within component %2").arg(
				viewName).arg(component->getVlnv()->getName()));
			return;
		}
	}

	// if the view was found but was not hierarchical
	if (view && !view->isHierarchical()) {

		// ask which file sets are included in the view
		QStringList fileSets = view->getFileSetRefs();

		// parse the file sets from the component and add them to the list of files
		parseFileSets(component, fileSets);
	}

	// view was found and was hierarchical
	else if (view && view->isHierarchical()) {

		VLNV designVLNV;
		QSharedPointer<Design> design;

		VLNV desConfVLNV;
		QSharedPointer<DesignConfiguration> designConf;

		VLNV vlnv = view->getHierarchyRef();

		// if the specified vlnv is not found within library
		if (!handler_->contains(vlnv)) {
			emit errorMessage(tr("Hierarchy reference %1:%2:%3:%4 referenced "
				"within view %5 in component %6 was not found in the library."
				" Stopping generation.").arg(
				vlnv.getVendor()).arg(
				vlnv.getLibrary()).arg(
				vlnv.getName()).arg(
				vlnv.getVersion()).arg(
				viewName).arg(
				component->getVlnv()->getName()));
			return;
		}

		// get the real type of the referenced document
		vlnv.setType(handler_->getDocumentType(vlnv));

		// if component contained reference to a design configuration
		if (vlnv.getType() == VLNV::DESIGNCONFIGURATION) {

			// parse the design configuration
			desConfVLNV = vlnv;
			QSharedPointer<LibraryComponent> libComp = handler_->getModel(desConfVLNV);
			designConf = libComp.staticCast<DesignConfiguration>();

			// find the referenced design
			designVLNV = designConf->getDesignRef();
		}

		// if component contained direct reference to a design 
		else if (vlnv.getType() == VLNV::DESIGN) {
			designVLNV = vlnv;
		}
		// if item was some other type
		else {
			emit errorMessage(tr("Item %1:%2:%3:%4 was not neither design or "
				"design configuration, stopping generation.").arg(
				vlnv.getVendor()).arg(
				vlnv.getLibrary()).arg(
				vlnv.getName()).arg(
				vlnv.getVersion()));
			return;
		}

		// if design does not exist
		if (!handler_->contains(designVLNV)) {
			emit errorMessage(tr("Design %1:%2:%3:%4 referenced within "
				"design configuration %5 was not found within library. "
				"Stopping generation.").arg(
				designVLNV.getVendor()).arg(
				designVLNV.getLibrary()).arg(
				designVLNV.getName()).arg(
				designVLNV.getVersion()).arg(
				desConfVLNV.getName()));
			return;
		}

		// parse the design
		QSharedPointer<LibraryComponent> libComp = handler_->getModel(designVLNV);
		design = libComp.staticCast<Design>();

		// read the design and it's component instances
		readDesign(design, designConf);

		// if this view contains a reference to a view that contains component's
		// top level implementation
		QString topLevelView = view->getTopLevelView();
		if (!topLevelView.isEmpty()) {
			parseFiles(component, topLevelView);
		}
	}
}

void QuartusGenerator::parseFileSets(QSharedPointer<Component> component, 
									 const QStringList& fileSetNames) {
	Q_ASSERT_X(component, "ModelsimGenerator::parseFileSets",
		"Null component-pointer given as parameter");


	QString basePath = handler_->getPath(*component->getVlnv());
	if (basePath.isEmpty()) {
		emit errorMessage(tr("Component %1:%2:%3:%4 was not found within "
			"library, stopping generation").arg(
			component->getVlnv()->getVendor()).arg(
			component->getVlnv()->getLibrary()).arg(
			component->getVlnv()->getName()).arg(
			component->getVlnv()->getVersion()));
		return;
	}

	// check each file set
	foreach (QString fileSetName, fileSetNames) {

		// if the specified file set was not found within component
		if (!component->hasFileSet(fileSetName)) {
			emit errorMessage(tr("Fileset %1 was not found within component "
				"%2:%3:%4:%5").arg(fileSetName).arg(
				component->getVlnv()->getVendor()).arg(
				component->getVlnv()->getLibrary()).arg(
				component->getVlnv()->getName()).arg(
				component->getVlnv()->getVersion()));
			continue;
		}

		// get the files in the file set
		QSharedPointer<FileSet> fileSet = component->getFileSet(fileSetName);
		QList<QSharedPointer<File> > fileList = fileSet->getFiles();

		// handle each file
		foreach (QSharedPointer<File> file, fileList) {

			// get the absolute path to the file
			QString absolutePath = General::getAbsolutePath(basePath,
				file->getName());

			if (absolutePath.isEmpty()) {
				emit errorMessage(tr("The file %1 was not found within %2").arg(
					file->getName()).arg(basePath));
				continue;
			}

			// make sure the file exists in the file system
			QFileInfo fileInfo(absolutePath);
			if (!fileInfo.exists()) {
				emit errorMessage(tr("The file %1 needed by component %2:%3:%4:%5"
					" was not found in the file system.").arg(
					absolutePath).arg(
					component->getVlnv()->getVendor()).arg(
					component->getVlnv()->getLibrary()).arg(
					component->getVlnv()->getName()).arg(
					component->getVlnv()->getVersion()));

				// move on to next file
				continue;
			}

			// if the file with options is not yet on the list, add it
			if (!files_.contains(absolutePath))
				files_.append(absolutePath);
		}
	}
}

void QuartusGenerator::readDesign(const QSharedPointer<Design> design,
								  const QSharedPointer<DesignConfiguration> desConf) {
	Q_ASSERT_X(design, "ModelsimGenerator::readDesign",
		"Null Design-pointer given as parameter");

	// read each component
	QList<ComponentInstance> instances = design->getComponentInstances();
	foreach (ComponentInstance const& instance, instances) {

		VLNV vlnv = instance.getComponentRef();

		// if component can't be found within library
		if (!handler_->contains(vlnv)) {
			emit errorMessage(tr("Component %1:%2:%3:%4 was not found within "
				"library, stopping generation").arg(
				vlnv.getVendor()).arg(
				vlnv.getLibrary()).arg(
				vlnv.getName()).arg(
				vlnv.getVersion()));
			continue;
		}

		QSharedPointer<Component> component;
		QSharedPointer<LibraryComponent> libComp = handler_->getModel(vlnv);

		// if library item is component
		if (libComp->getVlnv()->getType() == VLNV::COMPONENT)
			component = libComp.staticCast<Component>();

		// if item was not component
		else {
			emit errorMessage(tr("Referenced item %1:%2:%3:%4 was not a component").arg(
				vlnv.getVendor()).arg(
				vlnv.getLibrary()).arg(
				vlnv.getName()).arg(
				vlnv.getVersion()));
			continue;
		}

		QString viewName;

		// if design configuration is used
		if (desConf && desConf->hasActiveView(instance.getInstanceName())) {
			viewName = desConf->getActiveView(instance.getInstanceName());
		}
		// if design configuration is not used or view was not found
		else {

			// if component only has one view
			if (component->viewCount() == 1)
				viewName = component->getViews().first()->getName();

			// if component has multiple views
			else if (component->viewCount() > 1) {

				// ask user to select the view
				viewName = ComboSelector::selectView(component, parent_);

				if (viewName.isEmpty()) {
					emit noticeMessage(tr("No view selected for component "
						"%1:%2:%3:%4, skipping").arg(
						vlnv.getVendor()).arg(
						vlnv.getLibrary()).arg(
						vlnv.getName()).arg(
						vlnv.getVersion()));
					continue;
				}
			}
		}

		// call recursively to handle the component instance
		parseFiles(component, viewName);
	}
}

void QuartusGenerator::parseBlindFileSet(QSharedPointer<Component> component) {
	Q_ASSERT_X(component, "ModelsimGenerator::parseBlindFileSet",
		"Null component pointer given as parameter");

	QString basePath = handler_->getPath(*component->getVlnv());
	if (basePath.isEmpty()) {
		emit errorMessage(tr("Component %1:%2:%3:%4 was not found in library."));
		return;
	}

	// get all rtl files of the component
	QList<QSharedPointer<File> > fileList = component->getRTLFiles();
	foreach (QSharedPointer<File> file, fileList) {

		// get the absolute path to the file
		QString absolutePath = General::getAbsolutePath(basePath,
			file->getName());

		// make sure the file exists in the file system
		QFileInfo fileInfo(absolutePath);

		if (!fileInfo.exists()) {
			emit errorMessage(tr("The file %1 needed by component %2:%3:%4:%5"
				" was not found in the file system.").arg(
				absolutePath).arg(
				component->getVlnv()->getVendor()).arg(
				component->getVlnv()->getLibrary()).arg(
				component->getVlnv()->getName()).arg(
				component->getVlnv()->getVersion()));

			// move on to next file
			continue;
		}
		// if the file with options is not yet on the list, add it
		if (!files_.contains(absolutePath))
			files_.append(absolutePath);
	}
}
