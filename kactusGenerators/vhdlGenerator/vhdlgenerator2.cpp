/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlgenerator2.cpp
 *		Project: Kactus 2
 *
 *      Creates a structural VHDL file with following structure
 *      1. File header 
 *      2. Libraries
 *      3. Entity declaration
 *      4. Architecture
 *         - Signals (+ user's own code)
 *         - Component declarations
 *         - Component instantiations (+ user's own code)
 */

#include "vhdlgenerator2.h"

#include "vhdlgeneral.h"

#include <library/LibraryManager/libraryinterface.h>
#include <kactusGenerators/HDLGenerator/vhdlconnectionendpoint.h>
#include <kactusGenerators/HDLGenerator/HDLUtils.h>

#include <IPXACTmodels/view.h>
#include <IPXACTmodels/modelparameter.h>
#include <IPXACTmodels/port.h>
#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/fileset.h>
#include <IPXACTmodels/file.h>

#include <common/utils.h>

#include <QTime>
#include <QDate>
#include <QSettings>

VhdlGenerator2::VhdlGenerator2( LibraryInterface* handler, QObject* parent ):
HDLGeneratorTemplate(handler, parent)
{
	
}

VhdlGenerator2::~VhdlGenerator2()
{

}

bool VhdlGenerator2::addRTLView( QString const& vhdlFileName ) {
	// ipDir represents the directory where the IP-Xact file is located in.
	QString ipDir(handler()->getPath(*component_->getVlnv()));
	if (ipDir.isEmpty()) {
		emit errorMessage(tr("Path to top-component was not found."));
		return false;
	}

	// get the relative path to add to file set
	QString relativePath = General::getRelativePath(ipDir, vhdlFileName);
	if (relativePath.isEmpty()) {
		emit errorMessage(tr("Could not create relative path to vhdl file."));
		return false;
	}

	QString fileSetName;
	if (!viewName_.isEmpty()) {
		fileSetName = QString("%1_vhdlSource").arg(viewName_);
	}
	else {
		fileSetName = QString("vhdlSource");
	}
	QSharedPointer<FileSet> topFileSet = component_->getFileSet(fileSetName);

	// if the top vhdl file set was not found. Create one
	if (!topFileSet) {
		topFileSet = QSharedPointer<FileSet>(new FileSet(fileSetName, QString("sourceFiles")));
		topFileSet->useDefaultVhdlBuilders();
		component_->addFileSet(topFileSet);
	}

	topFileSet->clearFiles();

	QSettings settings;

	// create a new file
	QSharedPointer<File> topVhdlFile = topFileSet->addFile(relativePath, settings);
	topVhdlFile->setIncludeFile(true);
	topVhdlFile->setLogicalName("work");
	topVhdlFile->setCommand(QString("vcom"));
	topVhdlFile->setBuildFlags(QString("-quiet -check_synthesis -work work"), true);

	QString viewName;
	if (!viewName_.isEmpty()) {
		viewName = QString("%1_vhd").arg(viewName_);
	}
	else {
		viewName = "rtl";
	}
	View* rtlView = new View(viewName);

	// add the spirit:envIdentifier. Only language is defined, not tool
	// specific settings
	rtlView->addEnvIdentifier(QString("VHDL:Kactus2:"));

	// set the language of the view.
	rtlView->setLanguage(QString("vhdl"));

	// set the model name to be the top_level architecture of the top-level
	// entity
	QString archName;
	if (viewName_.isEmpty()) {
		archName = "rtl";
	}
	else {
		archName = viewName_;
	}
	QString topEntity(QString("%1(%2)").arg(topLevelEntity_).arg(archName));
	rtlView->setModelName(topEntity);

	// set a reference to a file set
	rtlView->addFileSetRef(fileSetName);

	// add the view to the component's model
	component_->addView(rtlView);

	// find the active view used to generate the vhdl
	View* activeView = component_->findView(viewName_);
	
	// if the view does not exist or it is not hierarchical
	if (!activeView || !activeView->isHierarchical()) {
		return true;
	}

	activeView->setTopLevelView(rtlView->getName());

	return true;
}

//-----------------------------------------------------------------------------
// Function: vhdlgenerator2::writeFileHeader()
//-----------------------------------------------------------------------------
void VhdlGenerator2::writeFileHeader( QTextStream& vhdlStream, QString const& fileName ) {

    vhdlStream << "-- ***************************************************" << endl;
    vhdlStream << "-- File         : " << fileName << endl;
    vhdlStream << "-- Creation date: " << QDate::currentDate().toString(QString("dd.MM.yyyy")) << endl;
    vhdlStream << "-- Creation time: " << QTime::currentTime().toString(QString("hh:mm:ss")) << endl;
    vhdlStream << "-- Description  : " << endl;
    HDLUtils::writeDescription(component_->getDescription(), vhdlStream, commentTag(), QString(""));
    vhdlStream << "-- " << endl;

    QSettings settings;
    QString userName = settings.value("General/Username", Utils::getCurrentUser()).toString();
    vhdlStream << "-- Created by   : " << userName << endl; 
    vhdlStream << "-- This file was generated with Kactus2 vhdl generator" << endl;
    VLNV* vlnv = component_->getVlnv();
    vhdlStream << "-- based on IP-XACT component " << vlnv->toString() << endl;
    vhdlStream << "-- whose XML file is " << handler()->getPath(*vlnv) << endl;
    vhdlStream << "-- ***************************************************" << endl;
}

//-----------------------------------------------------------------------------
// Function: VhdlGenerator2::writeLibraryIncludes()
//-----------------------------------------------------------------------------
void VhdlGenerator2::writeLibraryIncludes(QTextStream& fileStream)
{
    // always add IEEE library
    fileStream << "library IEEE;" << endl;

    // write the libraries needed 
    libraries_.append("work");
    libraries_.removeDuplicates();

    // declare the libraries used.
    foreach (QString library, libraries_) {
        if (!library.isEmpty()) {
            fileStream << "library " << library <<";" << endl;

            typeDefinitions_.append(QString("%1.all").arg(library));
        }
    }

    // always add the library for std_logic and std_logic_vector
    typeDefinitions_.append("IEEE.std_logic_1164.all");
    typeDefinitions_.removeDuplicates();

    // write all type defs needed
    foreach (QString portTypeDef, typeDefinitions_) {
        if (!portTypeDef.isEmpty()) {
            fileStream << "use " << portTypeDef << ";" << endl;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VhdlGenerator2::writeEntityStartTag()
//-----------------------------------------------------------------------------
void VhdlGenerator2::writeEntityStartTag(QTextStream& vhdlStream)
{
    vhdlStream << "entity " << topLevelEntity_ << " is" << endl;
}

//-----------------------------------------------------------------------------
// Function: VhdlGenerator2::writeEntityEndTag()
//-----------------------------------------------------------------------------
void VhdlGenerator2::writeEntityEndTag(QTextStream& vhdlStream)
{
    vhdlStream << "end " << topLevelEntity_ << ";" << endl << endl;
}

//-----------------------------------------------------------------------------
// Function: VhdlGenerator2::writeStructuralStartTag()
//-----------------------------------------------------------------------------
void VhdlGenerator2::writeStructuralStartTag(QTextStream& vhdlStream)
{
    // if view has description
    QString viewDescription = component_->getViewDescription(viewName_);
    HDLUtils::writeDescription(viewDescription, vhdlStream, commentTag());

    QString archName = viewName_;

    // if view name is not specified then "structural" is used
    if (archName.isEmpty())
    {
        archName = "structural";
    }

    vhdlStream << "architecture " << archName << " of " << topLevelEntity_ << " is" << endl;
}

//-----------------------------------------------------------------------------
// Function: VhdlGenerator2::writeComponentInstantiationStartTag()
//-----------------------------------------------------------------------------
void VhdlGenerator2::writeComponentInstantiationStartTag(QTextStream& vhdlStream)
{
    vhdlStream << "begin" << endl;
}

//-----------------------------------------------------------------------------
// Function: VhdlGenerator2::writePortsBegin()
//-----------------------------------------------------------------------------
void VhdlGenerator2::writePortsStartTag(QTextStream& vhdlStream)
{
    vhdlStream << "  " << "port (" << endl;
}

//-----------------------------------------------------------------------------
// Function: VhdlGenerator2::writePortsEnd()
//-----------------------------------------------------------------------------
void VhdlGenerator2::writePortsEndTag(QTextStream& vhdlStream)
{
    vhdlStream << "  " << ");" << endl;
}

//-----------------------------------------------------------------------------
// Function: VhdlGenerator2::writeModelParametersStartTag()
//-----------------------------------------------------------------------------
void VhdlGenerator2::writeModelParametersStartTag(QTextStream& vhdlStream)
{
    vhdlStream << "  " << "generic (" << endl;
}

//-----------------------------------------------------------------------------
// Function: VhdlGenerator2::writeModelParameterEndTag()
//-----------------------------------------------------------------------------
void VhdlGenerator2::writeModelParameterEndTag(QTextStream& vhdlStream)
{
    vhdlStream << "  " << ");" << endl;
}


//-----------------------------------------------------------------------------
// Function: VhdlGenerator2::commentTag()
//-----------------------------------------------------------------------------
QString VhdlGenerator2::commentTag() const
{
    return "--";
}

//-----------------------------------------------------------------------------
// Function: VhdlGenerator2::writeStructuralEndTag()
//-----------------------------------------------------------------------------
void VhdlGenerator2::writeStructuralEndTag(QTextStream& vhdlStream)
{
    QString archName = viewName_;

    // if view name is not specified then "structural" is used
    if (archName.isEmpty())
    {
        archName = "structural";
    }

    vhdlStream << "end " << archName << ";" << endl;
}

//-----------------------------------------------------------------------------
// Function: VhdlGenerator2::getLibrariesForComponent()
//-----------------------------------------------------------------------------
QStringList VhdlGenerator2::getLibrariesForComponent(QSharedPointer<HDLComponentInstance> compInstance, QString instanceActiveView)
{
    return compInstance->componentModel()->getVhdlLibraries(instanceActiveView);
}

//-----------------------------------------------------------------------------
// Function: VhdlGenerator2::checkPortTypeMatch()
//-----------------------------------------------------------------------------
bool VhdlGenerator2::checkPortTypeMatch(QString type, QString connectionType)
{
    return VhdlGeneral::checkVhdlTypeMatch(type, connectionType);
}

//-----------------------------------------------------------------------------
// Function: VhdlGenerator2::scalarSignalType()
//-----------------------------------------------------------------------------
QString VhdlGenerator2::scalarSignalType()
{
    return "std_logic";
}

//-----------------------------------------------------------------------------
// Function: VhdlGenerator2::vectorSignalType()
//-----------------------------------------------------------------------------
QString VhdlGenerator2::vectorSignalType()
{
    return "std_logic_vector";
}

//-----------------------------------------------------------------------------
// Function: VhdlGenerator2::createSignalObject()
//-----------------------------------------------------------------------------
VhdlSignal* VhdlGenerator2::createSignalObject(QString const& connectionName, QString type, int left, int right, QString const& description)
{
    return new VhdlSignal(this, connectionName, type, left, right, description);
}

//-----------------------------------------------------------------------------
// Function: VhdlGenerator2::isScalarPort()
//-----------------------------------------------------------------------------
bool VhdlGenerator2::isScalarType(QString const& type) const
{
    return VhdlGeneral::isScalarType(type);
}

//-----------------------------------------------------------------------------
// Function: VhdlGenerator2::createModelParameterObject()
//-----------------------------------------------------------------------------
HDLModelParameter* VhdlGenerator2::createModelParameterObject(QSharedPointer<ModelParameter> &modelParam)
{
    return new VhdlGeneric(modelParam.data(), this);
}

//-----------------------------------------------------------------------------
// Function: VhdlGenerator2::createPortObject()
//-----------------------------------------------------------------------------
HDLPort* VhdlGenerator2::createPortObject(QSharedPointer<Port> &port)
{
    return new VhdlPort(port.data(), this);
}

//-----------------------------------------------------------------------------
// Function: VhdlGenerator2::createComponentDeclarationObject()
//-----------------------------------------------------------------------------
HDLComponentDeclaration* VhdlGenerator2::createComponentDeclarationObject(QSharedPointer<Component> component)
{
    return new VhdlComponentDeclaration(component, this);
}

//-----------------------------------------------------------------------------
// Function: VhdlGenerator2::createComponentInstanceObject()
//-----------------------------------------------------------------------------
HDLComponentInstance* VhdlGenerator2::createComponentInstanceObject(QSharedPointer<Component> component, ComponentInstance const &instance, QString instanceActiveView)
{
    return new VhdlComponentInstance(this, handler(), component,
        instance.getInstanceName(), instanceActiveView, instance.getDescription());
}

