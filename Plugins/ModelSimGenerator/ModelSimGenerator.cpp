//-----------------------------------------------------------------------------
// File: ModelSimGeneratorPlugin.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 10.06.2016
//
// Description:
// ModelSim generator.
//-----------------------------------------------------------------------------

#include "ModelSimGenerator.h"

#include <IPXACTmodels/common/VLNV.h>
#include <library/LibraryManager/libraryhandler.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/DesignInstantiation.h>
#include <IPXACTmodels/Component/DesignConfigurationInstantiation.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>

#include <IPXACTmodels/generaldeclarations.h>

#include <QDir>
#include <QFile>
#include <QList>
#include <QTextStream>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
// Function: ModelSimGenerator::ModelSimGenerator()
//-----------------------------------------------------------------------------
ModelSimGenerator::ModelSimGenerator( ModelSimParser& parsedData, IPluginUtility* utility,
	LibraryInterface* handler ) : QObject(0), parsedData_( parsedData ), utility_( utility ),
	handler_(handler)
{
}

//-----------------------------------------------------------------------------
// Function: ModelSimGenerator::~ModelSimGenerator()
//-----------------------------------------------------------------------------
ModelSimGenerator::~ModelSimGenerator()
{
}

void ModelSimGenerator::generateMakefile(QString outputPath) {

	Q_ASSERT_X(handler_, "ModelSimGenerator::generateMakefile()",
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

bool ModelSimGenerator::addMakefile2IPXact(QSharedPointer<Component> component, 
	const QString& makefilePath, const QString& basePath)
{
		Q_ASSERT_X(component, "ModelSimGenerator::addMakefile2IPXact",
			"Null component pointer given as parameter");
		Q_ASSERT_X(!makefilePath.isEmpty(), "ModelSimGenerator::addMakefile2IPXact",
			"Empty makefile path given as parameter");
		Q_ASSERT_X(!basePath.isEmpty(), "ModelSimGenerator::addMakefile2IPXact",
			"Empty base path given as parameter");

		// get the relative path to add to file set
		QString relativePath = General::getRelativePath(basePath, makefilePath);

		QString fileSetName("ModelSimScripts");
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
			"Script file for ModelSim that compiles all files for view %1.").arg(parsedData_.getView()->name()));

		return true;
}

//-----------------------------------------------------------------------------
// Function: ModelSimGenerator::writeMakefile()
//-----------------------------------------------------------------------------
void ModelSimGenerator::writeMakefile(QTextStream& stream)
{
	stream << tr("# Script compiles all vhdl-files and generates a makefile for"
		" them") << endl;
	stream << tr("# This script is tested for ModelSim version 6.6a ") << endl;
	stream << tr("# Based on file %1").arg(parsedData_.getSourceXml());
	stream << tr(" and its view \"%1\"").arg(parsedData_.getView()->name()) << endl << endl;

	stream << ".main clear" << endl << endl;

	// inform user when executing the script
	stream << "echo " << tr("\" Generating libraries for files\"") << endl;

	// the top component's library is already written
	QStringList processedLibraries;
	QString activeComponent;
	QString activeFileSet;
	foreach (ModelSimParser::FileOptions fileOpt, *parsedData_.getParsedData())
	{
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

