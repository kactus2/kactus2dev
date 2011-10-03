/* 
 *  	Created on: 19.9.2011
 *      Author: Antti Kamppi
 * 		filename: documentgenerator.cpp
 *		Project: Kactus 2
 */

#include "documentgenerator.h"

#include <models/designconfiguration.h>
#include <models/design.h>
#include <models/librarycomponent.h>
#include <models/generaldeclarations.h>
#include <models/parameter.h>
#include <models/modelparameter.h>
#include <models/port.h>
#include <models/businterface.h>
#include <models/fileset.h>
#include <models/filebuilder.h>
#include <models/file.h>
#include <models/buildcommand.h>
#include <models/view.h>
#include <models/design.h>
#include <models/designconfiguration.h>

#include <common/KactusAttribute.h>
#include <common/widgets/componentPreviewBox/ComponentPreviewBox.h>

#include <designwidget/designwidget.h>

#include <QFileDialog>
#include <QString>
#include <QFile>
#include <QDateTime>
#include <QSettings>
#include <QVariant>
#include <QFileInfo>
#include <QMap>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QRectF>
#include <QPointF>
#include <QBrush>

#include <QDebug>

static const QString SPACE = QString("&nbsp;");
static const QString INDENT = QString("&nbsp;&nbsp;&nbsp;");
static const QString TABLE = QString("<table frame=\"box\" rules=\"all\""
									 "border=\"1\" cellPadding=\"3\" title=\"");

static const QString ENCODING = QString("<meta http-equiv=\"Content-Type\""
										"content=\"text/html; charset=utf-8\">");

static const QString DOCTYPE = QString("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" "
									   "\"http://www.w3.org/TR/html4/strict.dtd\">");

static const QString VALID_W3C_STRICT = QString("\t\t<p>\n"
												"\t\t\t<a href=\"http://validator.w3.org/check?uri=referer\">\n"
												"\t\t\t<img src=\"http://www.w3.org/Icons/valid-html401\""
												"alt=\"Valid HTML 4.01 Strict\" height=\"31\""
												"width=\"88\">\n"
												"\t\t\t</a>\n"
												"\t\t</p>\n");

DocumentGenerator::DocumentGenerator(LibraryInterface* handler,
									 QWidget* parent): 
QObject(parent),
handler_(handler),
component_(),
myNumber_(0),
childInstances_(),
targetPath_(), 
parent_(parent) {

	Q_ASSERT(handler_);
	Q_ASSERT(parent);
}

DocumentGenerator::DocumentGenerator( LibraryInterface* handler, 
									 const VLNV& vlnv,
									 QList<VLNV>& objects,
									 DocumentGenerator* parent ):
QObject(parent),
handler_(handler),
component_(),
myNumber_(0),
childInstances_(),
targetPath_(),
parent_(NULL) {

	Q_ASSERT(handler_);
	Q_ASSERT(parent);
	Q_ASSERT(handler_->contains(vlnv));
	Q_ASSERT(handler_->getDocumentType(vlnv) == VLNV::COMPONENT);

	connect(this, SIGNAL(errorMessage(const QString&)),
		parent, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(this, SIGNAL(noticeMessage(const QString&)),
		parent, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	QSharedPointer<LibraryComponent> libComp = handler_->getModel(vlnv);
	component_ = libComp.staticCast<Component>();

	parseChildItems(objects);
}

DocumentGenerator::~DocumentGenerator() {
}

void DocumentGenerator::setComponent( const VLNV& vlnv ) {
	Q_ASSERT(handler_->getDocumentType(vlnv) == VLNV::COMPONENT);
	
	// this function can be called for only the top document generator
	Q_ASSERT(parent_);

	// parse the model for the component
	QSharedPointer<LibraryComponent> libComp = handler_->getModel(vlnv);
	component_ = libComp.staticCast<Component>();

	// list of objects that have already been processed to avoid duplicates
	QList<VLNV> objects;
	objects.append(vlnv);

	parseChildItems(objects);
}

void DocumentGenerator::parseChildItems( QList<VLNV>& objects ) {

	// ask the component for it's hierarchical references
	QList<VLNV> refs = component_->getHierRefs();
	foreach (VLNV ref, refs) {
		QSharedPointer<Design> design = handler_->getDesign(ref);
		if (!design)
			continue;

		QList<Design::ComponentInstance> instances = design->getComponentInstances();
		foreach (Design::ComponentInstance instance, instances) {

			// if the component has already been processed
			if (objects.contains(instance.componentRef)) {
				continue;
			}
			// if the referenced component is not found.
			else if (!handler_->contains(instance.componentRef)) {
				continue;
			}
			// if instance reference is not for a component
			else if (handler_->getDocumentType(instance.componentRef) != VLNV::COMPONENT) {
				continue;
			}

			// create a new instance of document generator and add it to child list
			objects.append(instance.componentRef);
			QSharedPointer<DocumentGenerator> docGenerator(new DocumentGenerator(
				handler_, instance.componentRef, objects, this));
			childInstances_.append(docGenerator);
		}
	}
}

QString DocumentGenerator::writeDocumentation() {
	Q_ASSERT(component_);
	Q_ASSERT(handler_->contains(*component_->getVlnv()));
	Q_ASSERT(handler_->getDocumentType(*component_->getVlnv()) == VLNV::COMPONENT);

	// this function can only be called for the top document generator
	Q_ASSERT(parent_);

	// find the path to the component's xml file
	const QString xmlPath = handler_->getPath(*component_->getVlnv());
	// create the default path for the documentation file
	QFileInfo xmlInfo(xmlPath);
	
	targetPath_ = QFileDialog::getSaveFileName(NULL, 
		tr("Save the documentation to..."), xmlInfo.absolutePath(),
		tr("web pages (*.html)"));

	// if user clicked cancel
	if (targetPath_.isEmpty())
		return QString();

	QFile targetFile(targetPath_);

	// if file can not be opened for writing
	if (!targetFile.open(QFile::WriteOnly)) {
		emit errorMessage(tr("Could not open file %1 for writing.").arg(targetPath_));
		return QString();
	}
	QTextStream stream(&targetFile);

	// contains the username needed to write the top header
	QSettings settings;

	// write the top of the html document
	stream << DOCTYPE << endl;
	stream << "<html>" << endl;
	stream << "\t<head>" << endl;
	stream << "\t" << ENCODING << endl;
	stream << "\t\t<title>Kactus2 generated documentation for component " <<
		component_->getVlnv()->getName() << " " 
		<< component_->getVlnv()->getVersion() << "</title>" << endl;
	stream << "\t</head>" << endl;
	stream << "\t<body>" << endl;
	stream << "\t\t<h6>This document was generated by Kactus2 on " <<
		QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss") << " by user " <<
		settings.value("general/username", getenv("USERNAME")).toString() << "</h6>" << endl;

	// create a running number to create the numbered headers for table of contents
	unsigned int runningNumber = 0;

	stream << "\t\t<p>" << endl;
	stream << "\t\t<strong>Table of contents</strong><br>" << endl;
	// write the table of contents for this top component
	writeTableOfContents(runningNumber, stream);
	stream << "\t\t</p>" << endl;

	QStringList pictureList;

	// write the actual documentation for the top component
	writeDocumentation(stream, targetPath_, pictureList);

	stream << VALID_W3C_STRICT << endl;
	stream << "\t</body>" << endl;
	stream << "</html>" << endl;

	targetFile.close();

	// ask user if he wants to save the generated documentation into 
	// object metadata
	QMessageBox::StandardButton button = QMessageBox::question(parent_, 
		tr("Save generated documentation to metadata?"),
		tr("Would you like to save the generated documentation to IP-Xact"
		" metadata?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

	// if the generated file is saved
	if (button == QMessageBox::Yes) {

		QString xmlPath = handler_->getPath(*component_->getVlnv());

		// get the relative path to add to file set
		QString relativePath = General::getRelativePath(xmlPath, targetPath_);

		QString fileSetName("Documentation");
		FileSet* documentationFileSet = component_->getFileSet(fileSetName);

		// if the documentation file set was not found. Create one
		if (!documentationFileSet) {
			documentationFileSet = new FileSet(fileSetName, QString("documentation"));
			component_->addFileSet(documentationFileSet);
		}

		// create a new file
		File* docFile = new File(relativePath, documentationFileSet);
		docFile->addUserFileType(QString("htmlFile"));
		docFile->setIncludeFile(false);
		docFile->setDescription(tr("Html file that contains the documentation "
			"for this component and subcomponents"));

		// add the new file to the file set
		documentationFileSet->addFile(docFile);

		// add all created pictures to the file set
		foreach (QString pictureName, pictureList) {
			QString relativePicPath = General::getRelativePath(xmlPath, pictureName);

			File* picFile = new File(relativePicPath, documentationFileSet);
			picFile->addUserFileType(tr("picture"));
			picFile->setIncludeFile(false);
			picFile->setDescription(tr("Preview picture needed by the html document."));
			documentationFileSet->addFile(picFile);
		}

		handler_->writeModelToFile(component_);
	}

	return targetPath_;
}

void DocumentGenerator::writeTableOfContents(unsigned int& componentNumber,
											 QTextStream& stream ) {
	// increase the running number and save this instance's number
	++componentNumber;
	myNumber_ = componentNumber;

	stream << "\t\t<a href=\"#" << component_->getVlnv()->toString() << "\">" 
		<< myNumber_ << ". Component" << SPACE << component_->getVlnv()->getVendor() << " - " <<
		component_->getVlnv()->getLibrary() << " - " << 
		component_->getVlnv()->getName() << " - " <<
		component_->getVlnv()->getVersion()
		<< "</a><br>" << endl;
	
	// subHeader is running number that counts the number of sub headers for component
	int subHeader = 1;

	if (component_->hasModelParameters()) {
		stream << "\t\t" << INDENT << "<a href=\"#" << component_->getVlnv()->toString() <<
			".modelParams" << "\">" << myNumber_ << "." << subHeader << 
			". Model parameters</a><br>" << endl;
		++subHeader;
	}

	// component always has kactus parameters
	stream << "\t\t" << INDENT << "<a href=\"#" << component_->getVlnv()->toString() << 
		".kts_params" << "\">" << myNumber_ << "." << subHeader <<
		". Kactus2 attributes</a><br>" << endl;
	++subHeader;

	if (component_->hasParameters()) {
		stream << "\t\t" << INDENT << "<a href=\"#" << component_->getVlnv()->toString() <<
			".parameters" << "\">" << myNumber_ << "." << subHeader <<
			". General parameters</a><br>" << endl;
		++subHeader;
	}

	if (component_->hasPorts()) {
		stream << "\t\t" << INDENT << "<a href=\"#" << component_->getVlnv()->toString() <<
			".ports" << "\">" << myNumber_ << "." << subHeader <<
			". Ports</a><br>" << endl;
		++subHeader;
	}

	if (component_->hasInterfaces()) {
		stream << "\t\t" << INDENT << "<a href=\"#" << component_->getVlnv()->toString() <<
			".interfaces" << "\">" << myNumber_ << "." << subHeader <<
			". Bus interfaces</a><br>" << endl;
		++subHeader;
	}

	if (component_->hasFileSets()) {
		stream << "\t\t" << INDENT << "<a href=\"#" << component_->getVlnv()->toString() <<
			".fileSets" << "\">" << myNumber_ << "." << subHeader <<
			". File sets</a><br>" << endl;
		++subHeader;
	}

	if (component_->hasViews()) {
		stream << "\t\t" << INDENT << "<a href=\"#" << component_->getVlnv()->toString() <<
			".views" << "\">" << myNumber_ << "." << subHeader <<
			". Views</a><br>" << endl;
	}

	// tell each child to write it's table of contents
	foreach (QSharedPointer<DocumentGenerator> generator, childInstances_) {
		generator->writeTableOfContents(componentNumber, stream);
	}
}

void DocumentGenerator::writeDocumentation(QTextStream& stream, const QString& targetPath,
										   QStringList& filesToInclude) {
	
	targetPath_ = targetPath;
	
	// write the component header
	stream << "\t\t<h1><a id=\"" << component_->getVlnv()->toString() << "\">" << myNumber_ <<
		". Component " << component_->getVlnv()->toString(" - ") << "</a></h1>" << endl;

	createComponentPicture(filesToInclude);

	stream << "\t\t<p>" << endl;

	stream << "\t\t<img src=\"" << component_->getVlnv()->toString(".") << ".png\""
		" alt=\"" << component_->getVlnv()->toString(" - ") << " preview picture\"><br>" << endl;

	// if component has description, write it
	if (!component_->getDescription().isEmpty()) {
		stream << "\t\t<strong>Description:</strong> " << component_->getDescription() 
			<< "<br>" << endl;
	}

	// print relative path to the xml file
	QFileInfo compXmlInfo(handler_->getPath(*component_->getVlnv()));
	QString relativeXmlPath = General::getRelativePath(targetPath_, compXmlInfo.absoluteFilePath());
	stream << "\t\t<strong>IP-Xact file: </strong><a href=\"" << 
		relativeXmlPath << "\">" << compXmlInfo.fileName() <<
		"</a><br>" << endl;
	
	stream << "\t\t</p>" << endl;

	int subHeaderNumber = 1;
	writeModelParameters(stream, subHeaderNumber);
	writeParameters(stream, subHeaderNumber);
	writePorts(stream, subHeaderNumber);
	writeInterfaces(stream, subHeaderNumber);
	writeFileSets(stream, subHeaderNumber);
	writeViews(stream, subHeaderNumber, filesToInclude);

	// tell each child to write it's documentation
	foreach (QSharedPointer<DocumentGenerator> generator, childInstances_) {
		generator->writeDocumentation(stream, targetPath, filesToInclude);
	}
}

void DocumentGenerator::writeModelParameters(QTextStream& stream, int& subHeaderNumber) {
	if (component_->hasModelParameters()) {
		
		writeSubHeader(subHeaderNumber, stream, "Model parameters", "modelParams");

		QStringList paramHeaders;
		paramHeaders.append("Name");
		paramHeaders.append("Data type");
		paramHeaders.append("Default value");
		writeTableElement(paramHeaders, 
			"List of model parameters defined for the component",
			stream);
		const QMap<QString, QSharedPointer<ModelParameter> > modelParams = component_->getModelParameters();
		foreach (QSharedPointer<ModelParameter> param, modelParams) {
			stream << "\t\t\t\t<tr>" << endl;
			stream << "\t\t\t\t\t<td>" << param->getName() << "</td>" << endl;
			stream << "\t\t\t\t\t<td>" << param->getDataType() << "</td>" << endl;
			stream << "\t\t\t\t\t<td>" << param->getValue() << "</td>" << endl;
			stream << "\t\t\t\t</tr>" << endl;
		}

		stream << "\t\t\t</table>" << endl;
		++subHeaderNumber;
	}
}

void DocumentGenerator::writeParameters(QTextStream& stream, int& subHeaderNumber) {
	// write the kactus 2 parameters
	
	writeSubHeader(subHeaderNumber, stream, "Kactus2 attributes", "kts_params");
	
	stream << "\t\t<p>" << endl;
	stream << "\t\t\t<strong>" << INDENT << "Product hierarchy: </strong>" <<
		KactusAttribute::valueToString(component_->getComponentHierarchy()) <<
		"<br>" << endl;
	
	stream << "\t\t\t<strong>" << INDENT << "Component implementation: </strong>" <<
		KactusAttribute::valueToString(component_->getComponentImplementation()) <<
		"<br>" << endl;

	stream << "\t\t\t<strong>" << INDENT << "Component firmness: </strong>" <<
		KactusAttribute::valueToString(component_->getComponentFirmness()) <<
		"<br>" << endl;

	stream << "\t\t</p>" << endl;
	++subHeaderNumber;

	if (component_->hasParameters()) {
		writeSubHeader(subHeaderNumber, stream, "General parameters", "parameters");

		QStringList paramHeaders;
		paramHeaders.append("Name");
		paramHeaders.append("Value");
		paramHeaders.append("Description");
		writeTableElement(paramHeaders,
			"List of parameters defined for the component", stream);
		QList<QSharedPointer<Parameter> > params = component_->getParameters();
		foreach (QSharedPointer<Parameter> param, params) {
			stream << "\t\t\t\t<tr>" << endl;
			stream << "\t\t\t\t\t<td>" << param->getName() << "</td>" << endl;
			stream << "\t\t\t\t\t<td>" << param->getValue() << "</td>" << endl;
			stream << "\t\t\t\t\t<td>" << param->getDescription() << "</td>" << endl;
			stream << "\t\t\t\t</tr>" << endl;
		}
		stream << "\t\t\t</table>" << endl;
		++subHeaderNumber;
	}
}

void DocumentGenerator::writePorts(QTextStream& stream, int& subHeaderNumber) {

	if (component_->hasPorts()) {
		writeSubHeader(subHeaderNumber, stream, "Ports", "ports");

		QStringList portHeaders;
		portHeaders.append("Name");
		portHeaders.append("Direction");
		portHeaders.append("Width");
		portHeaders.append("Left bound");
		portHeaders.append("Right bound");
		portHeaders.append("Default value");
		writeTableElement(portHeaders, 
			"List of all ports the component has.", stream);
		const QMap<QString, QSharedPointer<Port> > ports = component_->getPorts();
		foreach (QSharedPointer<Port> port, ports) {
			stream << "\t\t\t\t<tr>" << endl;
			stream << "\t\t\t\t\t<td><a id=\"" << 
				component_->getVlnv()->toString() << ".port." << port->getName() << 
				"\">" <<  port->getName() << "</a></td>" << endl;
			stream << "\t\t\t\t\t<td>" << General::direction2Str(port->getDirection()) << "</td>" << endl;
			stream << "\t\t\t\t\t<td>" << port->getPortSize() << "</td>" << endl;
			stream << "\t\t\t\t\t<td>" << port->getLeftBound() << "</td>" << endl;
			stream << "\t\t\t\t\t<td>" << port->getRightBound() << "</td>" << endl;
			stream << "\t\t\t\t\t<td>" << port->getDefaultValue() << "</td>" << endl;
			stream << "\t\t\t\t</tr>" << endl;
		}

		stream << "\t\t\t</table>" << endl;
		++subHeaderNumber;
	}
}

void DocumentGenerator::writeInterfaces(QTextStream& stream, int& subHeaderNumber) {
	if (component_->hasInterfaces()) {
		
		writeSubHeader(subHeaderNumber, stream, "Bus interfaces", "interfaces");

		const QMap<QString, QSharedPointer<BusInterface> > interfaces = component_->getBusInterfaces();
		int interfaceNumber = 1;
		foreach (QSharedPointer<BusInterface> interface, interfaces) {
			stream << "\t\t\t" << "<h3>" << myNumber_ << "." << subHeaderNumber << "." << interfaceNumber++ <<
				" " << interface->getName() << "</h3>" << endl;
			
			stream << "\t\t\t<p>" << endl;

			// if interface has a description
			if (!interface->getDescription().isEmpty()) {
				stream << "\t\t\t" << INDENT << "<strong>Description:</strong> " << 
					interface->getDescription() << "<br>" << endl;
			}

			// write the interface mode of the interface
			stream << "\t\t\t" << INDENT << "<strong>Interface mode:</strong> " << 
				General::interfaceMode2Str(interface->getInterfaceMode()) << "<br>" << endl;

			stream << "\t\t\t" << INDENT << "<strong>Ports used in this interface:</strong>" << endl;

			stream << "\t\t\t</p>" << endl;

			stream << "\t\t\t<ul>" << endl;
			QStringList physPortList = interface->getPhysicalPortNames();
			foreach (QString physPort, physPortList) {
				stream << "\t\t\t\t<li><a href=\"#" << component_->getVlnv()->toString() 
					<< ".port." << physPort << "\">" << physPort << "</a></li>" << endl;
			}

			stream << "\t\t\t</ul>" << endl;
		}
		++subHeaderNumber;
	}
}

void DocumentGenerator::writeFileSets(QTextStream& stream, int& subHeaderNumber) {
	if (component_->hasFileSets()) {
		
		writeSubHeader(subHeaderNumber, stream, "File sets", "fileSets");

		const QList<QSharedPointer<FileSet> > fileSets = component_->getFileSets();
		int fileSetNumber = 1;
		foreach (QSharedPointer<FileSet> fileSet, fileSets) {
			stream << "\t\t\t" << "<h3><a id=\"" << component_->getVlnv()->toString() 
				<< ".fileSet." << fileSet->getName() << "\">" << myNumber_ << "." << subHeaderNumber << 
				"." << fileSetNumber <<	" " << fileSet->getName() << "</a></h3>" << endl;

			stream << "\t\t\t<p>" << endl;

			// if file set has a description
			if (!fileSet->getDescription().isEmpty()) {
				stream << "\t\t\t" << INDENT << "<strong>Description:</strong> " << 
					fileSet->getDescription() << "<br>" << endl;
			}

			// write the group identifiers
			QStringList groups = fileSet->getGroups();
			stream << "\t\t\t" << INDENT << "<strong>Identifiers:</strong> ";
			for (int i = 0; i < groups.size(); ++i) {
				stream << groups.at(i);

				// if this is not the last group identifier
				if (i != groups.size()-1)
					stream << ", ";
			}
			stream << "<br>" << endl;

			// if file set has specified default file builders
			QList<QSharedPointer<FileBuilder> > fileBuilders = fileSet->getDefaultFileBuilders();
			if (!fileBuilders.isEmpty()) {	
				stream << "\t\t\t" << INDENT << "<strong>Default file builders:</strong>" << endl;

				stream << "\t\t\t</p>" << endl;

				QStringList builderHeaders;
				builderHeaders.append("File type");
				builderHeaders.append("Command");
				builderHeaders.append("Flags");
				builderHeaders.append("Replace default flags");
				writeTableElement(builderHeaders, "The build settings for files in this file set.",
					stream);

				foreach (QSharedPointer<FileBuilder> fileBuilder, fileBuilders) {
					stream << "\t\t\t\t<tr>" << endl;
					stream << "\t\t\t\t\t<td>" << fileBuilder->getFileType() << "</td>" << endl;
					stream << "\t\t\t\t\t<td>" << fileBuilder->getCommand() << "</td>" << endl;
					stream << "\t\t\t\t\t<td>" << fileBuilder->getFlags() << "</td>" << endl;
					stream << "\t\t\t\t\t<td>" << 
						General::bool2Str(fileBuilder->getReplaceDefaultFlags()) << "</td>" << endl;
					stream << "\t\t\t\t</tr>" << endl;
				}
				stream << "\t\t\t</table>" << endl;
			}
			else {
				stream << "\t\t\t</p>" << endl;
			}

			stream << "\t\t\t" << "<h4>" << INDENT << myNumber_ << "." << subHeaderNumber <<
				"." << fileSetNumber << ".1 Files</h4>" << endl;

			QStringList fileHeaders;
			fileHeaders.append("File name");
			fileHeaders.append("Logical name");
			fileHeaders.append("Build command");
			fileHeaders.append("Build flags");
			fileHeaders.append("Specified file types");
			fileHeaders.append("Description");
			writeTableElement(fileHeaders, "List of files contained in this file set.", stream);

			// get the files contained in this file set.
			const QList<QSharedPointer<File> > files = fileSet->getFiles();
			foreach (QSharedPointer<File> file, files) {
				writeFile(file, stream);
			}
			stream << "\t\t\t</table>" << endl;

			++fileSetNumber;
		}
		++subHeaderNumber;
	}
}

void DocumentGenerator::writeViews( QTextStream& stream, int& subHeaderNumber,
								   QStringList& pictureList ) {
	if (component_->hasViews()) {
		stream << "\t\t\t<h2><a id=\"" << component_->getVlnv()->toString() << ".views\">"
			<< myNumber_ << "." << subHeaderNumber << " Views</a></h2>" << endl;
		
		const QList<QSharedPointer<View> > views = component_->getViews();
		int viewNumber = 1;
		foreach (QSharedPointer<View> view, views) {
			
			stream << "\t\t\t<h3>" << myNumber_ << "." << subHeaderNumber <<
				"." << viewNumber << " View: " << view->getName() << "</h3>" << endl;

			stream << "\t\t\t<p>" << endl;

			bool isHierarchicalView = view->isHierarchical();
			if (isHierarchicalView) {
				createDesignPicture(pictureList, view->getName());

				stream << "\t\t\t<img src=\"" << component_->getVlnv()->toString(".")
					<< "." << view->getName() << ".png\" alt=\"" << "View: " <<
					view->getName() << " preview picture\"><br>" << endl;
			}

			stream << "\t\t\t<strong>" << INDENT << "Type: </strong>";
			
			if (isHierarchicalView) {
				stream << "hierarchical" << "<br>" << endl;
			}
			else {
				stream << "non-hierarchical" << "<br>" << endl;
			}

			// if view has description
			if (!view->getDescription().isEmpty()) {
				stream << "\t\t\t<strong>Description: </strong>" << view->getDescription() 
					<< "<br>" << endl;
			}

			// if view is hierarchical
			if (isHierarchicalView) {
				stream << "\t\t\t</p>" << endl;

				writeView(view, stream, subHeaderNumber, viewNumber);
			}

			// if is flat view
			else {
				QStringList fileSetRefs = view->getFileSetRefs();
				if (!fileSetRefs.isEmpty()) {
					stream << "\t\t\t<strong>" << INDENT << "File sets contained in this view: </strong>" << endl;
					
					stream << "\t\t\t</p>" << endl;
					
					stream << "\t\t\t<ul>" << endl;
					foreach (QString fileSetRef, fileSetRefs) {
						stream << "\t\t\t\t<li><a href=\"#" << 
							component_->getVlnv()->toString() << ".fileSet." << fileSetRef
							<< "\">" << fileSetRef << "</a></li>" << endl;
					}
					stream << "\t\t\t</ul>" << endl;
				}
			}
			++viewNumber;
		}
		++subHeaderNumber;
	}
}

void DocumentGenerator::writeView( QSharedPointer<View> view, 
								  QTextStream& stream, 
								  const int subheaderNumber, 
								  const int viewNumber ) {
	
	VLNV vlnv = view->getHierarchyRef();
	
	// if vlnv was not found in library
	if (!handler_->contains(vlnv)) {
		QString errorMsg(tr("VLNV: %1:%2:%3:%4 was not found in library.").arg(
			vlnv.getVendor()).arg(
			vlnv.getLibrary()).arg(
			vlnv.getName()).arg(
			vlnv.getVersion()));

		stream << "\t\t\t<p>" << endl;
		stream << "\t\t\t<strong>" << errorMsg << "</strong><br>" << endl;
		stream << "\t\t\t</p>" << endl;
		emit errorMessage(errorMsg);
		return;
	}

	QSharedPointer<Design> design;
	QSharedPointer<DesignConfiguration> desConf;
	VLNV designVLNV;
	VLNV desConfVLNV;

	QSharedPointer<LibraryComponent> libComp = handler_->getModel(vlnv);
	
	// if the hierarchical reference was for design configuration
	if (handler_->getDocumentType(vlnv) == VLNV::DESIGNCONFIGURATION) {
		desConf = libComp.staticCast<DesignConfiguration>();
		desConfVLNV = *desConf->getVlnv();

		designVLNV = desConf->getDesignRef();
		// if design vlnv was not found in library
		if (!handler_->contains(designVLNV)) {
			QString errorMsg(tr("VLNV: %1:%2:%3:%4 was not found in library.").arg(
				designVLNV.getVendor()).arg(designVLNV.getLibrary()).arg(designVLNV.getName()).arg(
				designVLNV.getVersion()));

			stream << "\t\t\t<p>" << endl;
			stream << "\t\t\t<strong>" << errorMsg << "</strong><br>" << endl;
			stream << "\t\t\t</p>" << endl;
			emit errorMessage(errorMsg);
			return;
		}
		// if the vlnv is not for design
		else if (handler_->getDocumentType(designVLNV) != VLNV::DESIGN) {
			QString errorMsg(tr("VLNV: %1:%2:%3:%4 was not for design.").arg(
				designVLNV.getVendor()).arg(designVLNV.getLibrary()).arg(designVLNV.getName()).arg(
				designVLNV.getVersion()));

			stream << "\t\t\t<p>" << endl;
			stream << "\t\t\t<strong>" << errorMsg << "</strong><br>" << endl;
			stream << "\t\t\t</p>" << endl;
			emit errorMessage(errorMsg);
			return;
		}
		
		libComp = handler_->getModel(designVLNV);
		design = libComp.staticCast<Design>();
		designVLNV = *design->getVlnv();
	}

	// if the hierarchical reference was directly for a design
	else if (handler_->getDocumentType(vlnv) == VLNV::DESIGN) {
		libComp = handler_->getModel(vlnv);
		design = libComp.staticCast<Design>();
		designVLNV = *design->getVlnv();
	}
	
	// if hierarchical reference was for wrong type
	else {
		QString errorMsg(tr("VLNV: %1:%2:%3:%4 was not for design or design configuration.").arg(
			vlnv.getVendor()).arg(vlnv.getLibrary()).arg(vlnv.getName()).arg(
			vlnv.getVersion()));

		stream << "\t\t\t<p>" << endl;
		stream << "\t\t\t<strong>" << errorMsg << "</strong><br>" << endl;
		stream << "\t\t\t</p>" << endl;
		emit errorMessage(errorMsg);
		return;
	}

	// if design configuration was used
	if (desConf) {
		stream << "\t\t\t<h4>" << myNumber_ << "." << subheaderNumber << "." <<
			viewNumber << ".1 Configuration " << desConfVLNV.toString(" - ") << 
			"</h4>" << endl;
		
		stream << "\t\t\t<p>" << endl;
		// if configuration has a description
		if (!desConf->getDescription().isEmpty()) {
			stream << "\t\t\t<strong>" << INDENT << "Description: </strong>" << desConf->getDescription() <<
				"<br>" << endl;
		}

		// print relative path to the configuration xml file
		QFileInfo confXmlInfo(handler_->getPath(desConfVLNV));
		QString relativeXmlPath = General::getRelativePath(targetPath_, confXmlInfo.absoluteFilePath());
		stream << "\t\t\t<strong>" << INDENT << "IP-Xact file: </strong><a href=\"" << 
			relativeXmlPath << "\">" << confXmlInfo.fileName() <<
			"</a><br>" << endl;
		stream << "\t\t\t<br>" << endl;
		
		stream << "\t\t\t</p>" << endl;
	}

	Q_ASSERT(design);

	stream << "\t\t\t<h4>" << myNumber_ << "." << subheaderNumber << "." <<
		viewNumber << ".2 Design " << designVLNV.toString(" - ") << "</h4>" << endl;
	
	stream << "\t\t\t<p>" << endl;

	// if design contains a description
	if (!design->getDescription().isEmpty()) {
		stream << "\t\t\t<strong>" << INDENT << "Description: </strong>" << design->getDescription() <<
			"<br>" << endl;
	}

	// print relative path to the design xml file
	QFileInfo designXmlInfo(handler_->getPath(designVLNV));
	QString relativeXmlPath = General::getRelativePath(targetPath_, designXmlInfo.absoluteFilePath());
	stream << "\t\t\t<strong>" << INDENT << "IP-Xact file: </strong><a href=\"" << 
		relativeXmlPath << "\">" << designXmlInfo.fileName() <<
		"</a><br>" << endl;
	stream << "\t\t\t<br>" << endl;

	stream << "\t\t\t</p>" << endl;

	const QList<Design::ComponentInstance> instances = design->getComponentInstances();
	
	// if design does not contain instances
	if (instances.isEmpty()) {
		return;
	}

	QStringList instanceHeaders;
	instanceHeaders.append("Instance name");
	instanceHeaders.append("Component type");
	instanceHeaders.append("Configurable values");
	instanceHeaders.append("Active view");
	instanceHeaders.append("Description");
	writeTableElement(instanceHeaders, "Component instantiations within this design", stream);

	foreach (Design::ComponentInstance instance, instances) {
		stream << "\t\t\t\t<tr>" << endl;

		stream << "\t\t\t\t\t<td>" << instance.instanceName << "</td>" << endl;
		stream << "\t\t\t\t\t<td><a href=\"#" << instance.componentRef.toString(":") 
			<< "\">" << instance.componentRef.toString(" - ") << "</a></td>" << endl;
		
		// write the configurable element values of the instance
		QMap<QString, QString> confElements = instance.configurableElementValues;
		stream << "\t\t\t\t\t<td>" << endl;
		for (QMap<QString, QString>::iterator i = confElements.begin();
			i != confElements.end(); ++i) {
				stream << "\t\t\t\t\t" << i.key() << " = " << i.value();
				
				// if this is not the last configurable value
				if (i != --confElements.end()) {
					stream << "<br>";
				}
				stream << endl;
		}
		stream << "\t\t\t\t\t</td>" << endl;

		stream << "\t\t\t\t\t<td>";
		if (desConf) {
			stream << desConf->getActiveView(instance.instanceName);
		}
		stream << "</td>" << endl;

		stream << "\t\t\t\t\t<td>" << instance.description << "</td>" << endl;

		stream << "\t\t\t\t</tr>" << endl;
	}

	stream << "\t\t\t</table>" << endl; 
}

void DocumentGenerator::writeSubHeader( const int headerNumber, 
									   QTextStream& stream, 
									   const QString& text,
									   const QString& headerID) {

   stream << "\t\t<h2><a id=\"" << component_->getVlnv()->toString() << "." <<
	   headerID << "\">" << myNumber_ << "." << headerNumber << " " << text <<
	   "</a></h2>" << endl;
}

void DocumentGenerator::writeTableElement( const QStringList& headers, 
										  const QString& title,
										  QTextStream& stream ) {
	stream << "\t\t\t" << TABLE << title << "\">" << endl; 
	stream << "\t\t\t\t<tr>" << endl;
	foreach (QString header, headers) {
		stream << "\t\t\t\t\t<th>" << header << "</th>" << endl;
	}
	stream << "\t\t\t\t</tr>";
}

void DocumentGenerator::writeFile( QSharedPointer<File> file,
								  QTextStream& stream ) {
	
	// get absolute path to the file
	QString relativeFilePath = file->getName();
	QString absFilePath = General::getAbsolutePath(handler_->getPath(*component_->getVlnv()), relativeFilePath);
	QFileInfo fileInfo(absFilePath);

	// get relative path from html file to the file
	QFileInfo htmlInfo(targetPath_);
	QString pathFromhtmlToFile = General::getRelativePath(targetPath_, absFilePath);

	BuildCommand* buildCommand = file->getBuildcommand();

	QStringList fileTypes = file->getFileTypes();
	fileTypes += file->getUserFileTypes();

	stream << "\t\t\t\t<tr>" << endl;
	stream << "\t\t\t\t\t<td><a href=\"" << pathFromhtmlToFile << "\">" << 
		fileInfo.fileName() << "</a></td>" << endl;
	stream << "\t\t\t\t\t<td>" << file->getLogicalName() << "</td>" << endl;
	
	stream << "\t\t\t\t\t<td>";
	if (buildCommand) {
		stream << buildCommand->getCommand();
	}
	stream << "</td>" << endl;

	stream << "\t\t\t\t\t<td>";
	if (buildCommand) {
		stream << buildCommand->getFlags();
	}
	stream << "</td>" << endl;
	
	stream << "\t\t\t\t\t<td>";
	for (int i = 0; i < fileTypes.size(); ++i) {
		stream << fileTypes.at(i);
		if (i != fileTypes.size() - 1) {
			stream << ",<br>";
		}
	}
	stream << "</td>" << endl;
	
	stream << "\t\t\t\t\t<td>" << file->getDescription() << "</td>" << endl;
	stream << "\t\t\t\t</tr>" << endl;
}

void DocumentGenerator::createComponentPicture(QStringList& pictureList) {
	ComponentPreviewBox compBox(handler_);
	compBox.hide();
	compBox.setComponent(component_);

	QFileInfo htmlInfo(targetPath_);
	QString compPicPath = htmlInfo.absolutePath(); 
	compPicPath += "/";
	compPicPath += component_->getVlnv()->toString(".");
	compPicPath += ".png";

	QFile compPicFile(compPicPath);

	// if there exists a previous file
	if (compPicFile.exists()) {
		compPicFile.remove();
	}

	// get the rect that bounds all items on box
	QRectF boundingRect = compBox.itemsBoundingRect();
	boundingRect.setHeight(boundingRect.height() + 2);
	boundingRect.setWidth(boundingRect.width() + 2);

	// set the size of the picture
	QPixmap compPic(boundingRect.size().toSize());

	// create the picture for the component
	QPainter painter(&compPic);
	painter.fillRect(compPic.rect(), QBrush(Qt::white));
	compBox.scene()->render(&painter, compPic.rect(), boundingRect.toRect());
	if (!compPic.save(&compPicFile, "PNG")) {
		emit errorMessage(tr("Could not save picture %1").arg(compPicPath));
	}
	else {
		pictureList.append(compPicPath);
	}
	
}

void DocumentGenerator::createDesignPicture( QStringList& pictureList, 
											const QString& viewName ) {

	DesignWidget designWidget(handler_);
	designWidget.hide();
	designWidget.setDesign(component_->getVlnv(), viewName);

	QFileInfo htmlInfo(targetPath_);
	QString designPicPath = htmlInfo.absolutePath(); 
	designPicPath += "/";
	designPicPath += component_->getVlnv()->toString(".");
	designPicPath += ".";
	designPicPath += viewName;
	designPicPath += ".png";

	QFile designPicFile(designPicPath);

	// if there exists a previous file
	if (designPicFile.exists()) {
		designPicFile.remove();
	}

	// get the rect that bounds all items on box
	QRectF boundingRect = designWidget.itemsBoundingRect();
	boundingRect.setHeight(boundingRect.height() + 2);
	boundingRect.setWidth(boundingRect.width() + 2);

	// set the size of the picture
	QPixmap designPic(boundingRect.size().toSize());

	// create the picture for the component
	QPainter painter(&designPic);
	painter.fillRect(designPic.rect(), QBrush(Qt::white));
	designWidget.scene()->render(&painter, designPic.rect(), boundingRect.toRect());
	if (!designPic.save(&designPicFile, "PNG")) {
		emit errorMessage(tr("Could not save picture %1").arg(designPicPath));
	}
	else {
		pictureList.append(designPicPath);
	}
}
