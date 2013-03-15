/* 
 *	Created on: 15.2.2013
 *	Author:		Antti Kamppi
 * 	File name:	memorymapheadergenerator.cpp
 * 	Project:	Kactus 2
*/

#include "memorymapheadergenerator.h"
#include <LibraryManager/libraryinterface.h>
#include <PluginSystem/IPluginUtility.h>
#include <common/dialogs/fileSaveDialog/filesavedialog.h>
#include "localheadersavemodel.h"
#include <models/memorymap.h>
#include <models/generaldeclarations.h>
#include <models/fileset.h>
#include <models/file.h>
#include <models/design.h>
#include <models/businterface.h>
#include <models/masterinterface.h>
#include <models/mirroredslaveinterface.h>
#include <models/slaveinterface.h>
#include <models/designconfiguration.h>
#include <common/KactusAttribute.h>
#include <common/utils.h>

#include <QtPlugin>
#include <QFileInfo>
#include <QFile>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDate>
#include <QSettings>
#include <QDir>
#include <QString>
#include <QDesktopServices>
#include <QUrl>

#include <QDebug>

const QString MemoryMapHeaderGenerator::DEFAULT_TARGET_FILESET = QString("cSources");

MemoryMapHeaderGenerator::MemoryMapHeaderGenerator():
utility_(NULL),
design_(),
operatedInterfaces_() {
}

MemoryMapHeaderGenerator::~MemoryMapHeaderGenerator() {
}

QString const& MemoryMapHeaderGenerator::getName() const {
	static QString name(tr("Memory Map Header Generator"));
	return name;
}

QString const& MemoryMapHeaderGenerator::getVersion() const {
	static QString version(tr("1.0"));
	return version;
}

QString const& MemoryMapHeaderGenerator::getDescription() const {
	static QString description(tr("Generates C-headers for memory maps of a component"));
	return description;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::getSettingsWidget()
//-----------------------------------------------------------------------------
PluginSettingsWidget* MemoryMapHeaderGenerator::getSettingsWidget()
{
    return new PluginSettingsWidget();
}


//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::getIcon()
//-----------------------------------------------------------------------------
QIcon MemoryMapHeaderGenerator::getIcon() const
{
    return QIcon(":icons/headerGenerator24.png");
}

bool MemoryMapHeaderGenerator::checkGeneratorSupport( QSharedPointer<LibraryComponent const> libComp,
	QSharedPointer<LibraryComponent const> libDesConf /*= QSharedPointer<LibraryComponent const>()*/,
	QSharedPointer<LibraryComponent const> libDes /*= QSharedPointer<LibraryComponent const>()*/ ) const {
	
	// make sure the object is a component
	QSharedPointer<Component const> comp = libComp.dynamicCast<Component const>();
	if (!comp) {
		return false;
	}

	// if there is no design then header is generated for local memory maps
	if (!libDesConf) {
		return comp->hasLocalMemoryMaps();
	}

	// make sure the second parameter is for a design configuration object
	QSharedPointer<DesignConfiguration const> designConf = libDesConf.dynamicCast<DesignConfiguration const>();
	// the design configuration must be for HW or system
	if (designConf) {
		return comp->getComponentImplementation() == KactusAttribute::KTS_HW;
	}
	else {
		return false;
	}
}

void MemoryMapHeaderGenerator::runGenerator( IPluginUtility* utility,
	QSharedPointer<LibraryComponent> libComp,
	QSharedPointer<LibraryComponent> libDesConf /*= QSharedPointer<LibraryComponent>()*/,
	QSharedPointer<LibraryComponent> libDes /*= QSharedPointer<LibraryComponent>()*/ ) {

	utility_ = utility;

	QSharedPointer<Component> comp = libComp.dynamicCast<Component>();
	Q_ASSERT(comp);

	QSharedPointer<Design> design = libDes.dynamicCast<Design>();

	// if there is no design object then create headers for local memory maps
	if (!design) {
		generateLocalMemMapHeaders(comp);
	}
	// if there is a design configuration
	else if (libDesConf) {
		Q_ASSERT(design);

		// the component knows the implementation of the view
		KactusAttribute::Implementation implementation = comp->getViewType(*libDesConf->getVlnv());

		// if the generator is run on a hierarchical HW component
		if (implementation == KactusAttribute::KTS_HW) {
			generateGlobalHeaders(comp, design);
		}

		// the generator is run on a system component
		else {
			qDebug() << "System generator with design conf";
		}
	}

	// if there is a design but no design configuration
	else {
		Q_ASSERT(design);

		KactusAttribute::Implementation implementation = comp->getViewType(*design->getVlnv());

		// if the generator is run on a hierarchical HW component
		if (implementation == KactusAttribute::KTS_HW) {
			generateGlobalHeaders(comp, design);
		}

		// the generator is run on a system component
		else {
			qDebug() << "System generator without design conf";
		}
	}
}

void MemoryMapHeaderGenerator::addHeaderFile( QSharedPointer<Component> component, 
	const QFileInfo& fileInfo, 
	const QString& filesetName ) const {
	
	QString xmlDir = utility_->getLibraryInterface()->getDirectoryPath(*component->getVlnv());
	
	// if the directory does not exist
	QDir ipXactDir(xmlDir);
	Q_ASSERT(ipXactDir.exists());
	Q_ASSERT(fileInfo.exists());

	// calculate the relative path 
	QString relPath =  ipXactDir.relativeFilePath(fileInfo.absoluteFilePath());

	// file set where the file is added to
	QSharedPointer<FileSet> fileSet = component->getFileSet(filesetName);
	Q_ASSERT(fileSet);

	fileSet->setGroups("generatedFiles");
	fileSet->setDescription(tr("Contains header files generated for the component."));

	// the file
	QSharedPointer<File> file = fileSet->addFile(relPath);
	Q_ASSERT(file);

	file->clearFileTypes();
	file->addFileType("cSource");
	file->addFileType("cppSource");
	file->setDescription(tr("A header file generated by Kactus2.\n"
		"This file contains the register and memory addresses defined in the memory map(s)"));
	file->setIncludeFile(true);

	// show the generated file to user
	QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
}

void MemoryMapHeaderGenerator::generateLocalMemMapHeaders( QSharedPointer<Component> comp ) {
	// the model which displays the local memory maps and their header paths
	LocalHeaderSaveModel model(utility_->getLibraryInterface(), this);
	model.setComponent(comp);

	FileSaveDialog dialog(utility_->getParentWidget());
	dialog.setModel(&model);

	int result = dialog.exec();

	// if user clicked cancel
	if (result == QDialog::Rejected) {
		return;
	}

	// if there are no files to add then there is no need to save the component
	bool changed = false;

	// get list of the selected paths and generate each header
	const QList<LocalHeaderSaveModel::SaveFileOptions*>& options = model.getHeaderOptions();
	foreach (LocalHeaderSaveModel::SaveFileOptions* headerOpt, options) {

		QFile file(headerOpt->fileInfo_.absoluteFilePath());

		// make sure the directory structure exists for the file
		QDir dir(headerOpt->fileInfo_.dir());
		QString dirName(dir.dirName());
		dir.cdUp();
		dir.mkpath(dirName);

		// open the file and erase all old contents if any exists

		// if file could not be opened
		if (!file.open(QFile::Truncate | QFile::WriteOnly)) {

			QString message(tr("File: %1 could not be opened for writing.").arg(headerOpt->fileInfo_.absoluteFilePath()));
			QMessageBox::critical(utility_->getParentWidget(), QCoreApplication::applicationName(), message);
			break;
		}
		// now file has been opened for writing

		// open file stream to write to
		QTextStream stream(&file);

		// write header comments of the file
		stream << "/*" << endl;
		stream << " * File: " << headerOpt->fileInfo_.fileName() << endl;
		stream << " * Created on: " << QDate::currentDate().toString("dd.MM.yyyy") << endl;

		QSettings settings;
		QString userName = settings.value("General/Username", getenv("USERNAME")).toString();

		stream << " * Generated by: " << userName << endl;
		stream << " *" << endl;
		stream << " * Description:" << endl;
		stream << " * Header file generated by Kactus2 from local memory map \"" << headerOpt->localMemMap_->getName() << "\"." << endl;
		stream << " * This file contains addresses of the memories and registers defined in the local memory map." << endl;
		stream << " * Source component: " << comp->getVlnv()->toString() << "." << endl;
		stream << "*/" << endl;
		stream << endl;

		// write the preprocessor guard
		QString headerGuard("__");
		headerGuard.append(comp->getVlnv()->toString("_").toUpper());
		headerGuard.append(QString("_%1_H").arg(headerOpt->localMemMap_->getName().toUpper()));
		stream << "#ifndef " << headerGuard << endl;
		stream << "#define " << headerGuard << endl << endl;

		// write the memory addresses
		headerOpt->localMemMap_->writeMemoryAddresses(stream, 0);

		// if the register names are unique then there is no need to add address block name
		QStringList regNames;
		if (headerOpt->localMemMap_->uniqueRegisterNames(regNames)) {
			headerOpt->localMemMap_->writeRegisters(stream, 0, false);
		}
		// if there are registers with same names then address block names must be
		// included in the defines
		else {
			headerOpt->localMemMap_->writeRegisters(stream, 0, true);
		}

		// end the preprocessor guard
		stream << "#endif /* " << headerGuard << " */" << endl << endl;

		// close the file after writing
		file.close();

		// add the file to the component's file sets
		addHeaderFile(comp, headerOpt->fileInfo_, QString("%1_header").arg(headerOpt->localMemMap_->getName()));

		// a header file was added
		changed = true;
	}

	if (changed) {
		// save the changes to the file sets
		utility_->getLibraryInterface()->writeModelToFile(comp);
	}
}

void MemoryMapHeaderGenerator::generateGlobalHeaders( QSharedPointer<Component> comp, QSharedPointer<Design> design ) {
	
	design_ = design;
	operatedInterfaces_.clear();

	// the model which manages the dialog contents
	GlobalHeaderSaveModel model(utility_->getLibraryInterface(), this);
	model.setDesign(comp, design);

	// create the dialog to display the headers to be generated
	FileSaveDialog dialog(utility_->getParentWidget());
	dialog.setModel(&model);

	int result = dialog.exec();

	// if user clicked cancel
	if (result == QDialog::Rejected) {
		return;
	}

	// if there are no files to add then there is no need to save the component
	bool changed = false;

	const QList<GlobalHeaderSaveModel::SaveFileOptions*>& options = model.getHeaderOptions();
	foreach (GlobalHeaderSaveModel::SaveFileOptions* headerOpt, options) {
		QFile file(headerOpt->fileInfo_.absoluteFilePath());

		// make sure the directory structure exists for the file
		QDir dir(headerOpt->fileInfo_.dir());
		if (!dir.exists()) {
			QString dirName(dir.dirName());
			dir.cdUp();
			dir.mkpath(dirName);
		}

		// open the file and erase all old contents if any exists

		// if file could not be opened
		if (!file.open(QFile::Truncate | QFile::WriteOnly)) {

			QString message(tr("File: %1 could not be opened for writing.").arg(headerOpt->fileInfo_.absoluteFilePath()));
			QMessageBox::critical(utility_->getParentWidget(), QCoreApplication::applicationName(), message);
			break;
		}
		// now file has been opened for writing

		// open file stream to write to
		QTextStream stream(&file);

		// write header comments of the file
		stream << "/*" << endl;
		stream << " * File: " << headerOpt->fileInfo_.fileName() << endl;
		stream << " * Created on: " << QDate::currentDate().toString("dd.MM.yyyy") << endl;

		QSettings settings;
		QString userName = settings.value("General/Username", getenv("USERNAME")).toString();

		stream << " * Generated by: " << userName << endl;
		stream << " *" << endl;
		stream << " * Description:" << endl;
		stream << " * Header file generated by Kactus2 for instance \"" << headerOpt->instance_ << 
			"\" interface \"" << headerOpt->interface_ << "\"." << endl;
		stream << " * This file contains addresses of the memories and registers defined in the memory maps of connected components." << endl;
		stream << " * Source component: " << headerOpt->comp_.toString() << "." << endl;
		stream << "*/" << endl;
		stream << endl;

		// write the preprocessor guard
		QString headerGuard(QString("__%1_%2_H").arg(headerOpt->instance_.toUpper()).arg(headerOpt->interface_.toUpper()));
		stream << "#ifndef " << headerGuard << endl;
		stream << "#define " << headerGuard << endl << endl;

		// add the starting point to the list of operated interfaces
		Design::Interface cpuMasterInterface(headerOpt->instance_, headerOpt->interface_);
		operatedInterfaces_.append(cpuMasterInterface);

		// start the address parsing from the cpu's interface
		parseInterface(0, stream, cpuMasterInterface);

		// end the preprocessor guard
		stream << "#endif /* " << headerGuard << " */" << endl << endl;
		
		// close the file after writing
		file.close();

		// add the file to the component's file sets
		addHeaderFile(comp, headerOpt->fileInfo_, headerOpt->instance_);

		// a header file was added
		changed = true;

		// the list must be cleared when moving to completely new master interface
		// so each header generation starts from scratch.
		operatedInterfaces_.clear();
	}

	if (changed) {
		// save the changes to the file sets
		utility_->getLibraryInterface()->writeModelToFile(comp);
	}

	// clear the members for next generation run
	design_.clear();
	operatedInterfaces_.clear();
}

void MemoryMapHeaderGenerator::parseInterface( qint64 offset,
	QTextStream& stream,
	const Design::Interface& interface ) {

	Q_ASSERT(design_);
	Q_ASSERT(design_->containsHWInstance(interface.componentRef));

	// parse the component containing the interface
	VLNV compVLNV = design_->getHWComponentVLNV(interface.componentRef);
	QSharedPointer<const LibraryComponent> libComp = utility_->getLibraryInterface()->getModelReadOnly(compVLNV);
	QSharedPointer<const Component> comp = libComp.dynamicCast<const Component>();
	Q_ASSERT(comp);

	switch (comp->getInterfaceMode(interface.busRef)) {
	case General::MASTER: {
		// update the offset
		offset += Utils::str2Int(comp->getBusInterface(interface.busRef)->getMaster()->getBaseAddress());

		// ask the design for interfaces that are connected to this interface
		QList<Design::Interface> connected = design_->getConnectedInterfaces(interface);
		
		// all connected interfaces are processed
		foreach (Design::Interface targetInterface, connected) {

			// if the connected interface has already been processed before
			if (operatedInterfaces_.contains(targetInterface)) {
				continue;
			}

			// add the interface to the list to avoid processing it again
			operatedInterfaces_.append(targetInterface);

			parseInterface(offset, stream, targetInterface);
		}
		break;
								 }
	case General::SLAVE: {

		// the slave contains the slave-specific data
		QSharedPointer<SlaveInterface> slave = comp->getBusInterface(interface.busRef)->getSlave();
		Q_ASSERT(slave);

		QSharedPointer<MemoryMap> memMap = comp->getMemoryMap(slave->getMemoryMapRef());

		// if the memory map exists and contains at least something
		if (memMap && memMap->containsSubItems()) {
			// write the identifier comment for the instance
			stream << "/*" << endl;
			stream << " * Instance: " << interface.componentRef << " Interface: " << interface.busRef << endl;
			stream << " * Instance base address: 0x" << QString::number(offset, 16) << endl;
			stream << " * Source component: " << comp->getVlnv()->toString() << endl;

			// if there is a description for the component instance
			QString instanceDesc = design_->getHWInstanceDescription(interface.componentRef);
			if (!instanceDesc.isEmpty()) {
				stream << " * Description:" << endl;
				stream << " * " << instanceDesc << endl;
			}
			
			stream << " * The defines for the memory map \"" << memMap->getName() << "\":" << endl;
			stream << "*/" << endl << endl;

			memMap->writeMemoryAddresses(stream, offset, interface.componentRef);

			// if the registers within the instance are unique then do not concatenate with address block name
			QStringList regNames;
			if (memMap->uniqueRegisterNames(regNames)) {
				memMap->writeRegisters(stream, offset, false, interface.componentRef);
			}
			else {
				memMap->writeRegisters(stream, offset, true, interface.componentRef);
			}
		}

		// if the slave contains a bridge to a master interface
		if (slave->hasBridge()) {

			// process each connected master-interface
			QStringList masterNames = slave->getMasterReferences();
			foreach (QString masterRef, masterNames) {
				
				// if the interface reference is not valid
				if (!comp->hasInterface(masterRef)) {
					continue;
				}

				// the interface for the connected master-bus interface
				Design::Interface masterIF(interface.componentRef, masterRef);

				// if the connected interface has already been processed before
				if (operatedInterfaces_.contains(masterIF)) {
					continue;
				}

				// add the interface to the list to avoid processing it again
				operatedInterfaces_.append(masterIF);

				parseInterface(offset, stream, masterIF);
			}
		}

		break;
								}
	case General::MIRROREDSLAVE: {
		// increase the offset by the remap address of the mirrored slave interface
		QString remapStr = comp->getBusInterface(interface.busRef)->getMirroredSlave()->getRemapAddress();

		// if the remap address is directly a number
		if (Utils::isNumber(remapStr)) {
			offset += Utils::str2Int(remapStr);
		}
		// if the remap address refers to a configurable element value
		else {

			// if the configurable element value is specified
			if (design_->hasConfElementValue(interface.componentRef, remapStr)) {

				// increase the offset by the value set in the configurable elements
				QString confValue = design_->getConfElementValue(interface.componentRef, remapStr);
				offset += Utils::str2Int(confValue);
			}
			// if the value is not set then use the default value from the component
			else {

				// increase the offset by the default value
				QString defValue = comp->getAllParametersDefaultValue(remapStr);
				offset += Utils::str2Int(defValue);
			}
		}

		// ask the design for interfaces that are connected to this interface
		QList<Design::Interface> connected = design_->getConnectedInterfaces(interface);

		// all connected interfaces are processed
		foreach (Design::Interface targetInterface, connected) {

			// if the connected interface has already been processed before
			if (operatedInterfaces_.contains(targetInterface)) {
				continue;
			}

			// add the interface to the list to avoid processing it again
			operatedInterfaces_.append(targetInterface);

			parseInterface(offset, stream, targetInterface);
		}

		break;
										  }
	case General::MIRROREDMASTER: {
		// mirrored master interfaces are connected via channels
		// find the interfaces connected to the specified mirrored master interface
		QList<QSharedPointer<const BusInterface> > connectedInterfaces = comp->getChannelConnectedInterfaces(interface.busRef);

		// all interfaces that are connected via channel are processed
		foreach (QSharedPointer<const BusInterface> busif, connectedInterfaces) {
			Design::Interface connectedInterface(interface.componentRef, busif->getName());

			// if the interface connected via channel has already been processed before
			if (operatedInterfaces_.contains(connectedInterface)) {
				continue;
			}

			// add the interface to the list to avoid processing it again
			operatedInterfaces_.append(connectedInterface);

			parseInterface(offset, stream, connectedInterface);
		}
		break;
											}
	default: {
		qDebug() << "Instance " << interface.componentRef << " Interface " << interface.busRef << " was not supported type";
		return;
				}
	}
}
