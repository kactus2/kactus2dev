//-----------------------------------------------------------------------------
// File: MakefileParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 22.09.2014
//
// Description:
// Parses makefile data from SW stack.
//-----------------------------------------------------------------------------

#include "MakefileParser.h"

#include <QFileInfo>
#include <QSet>

#include <IPXACTmodels/kactusExtensions/ApiInterface.h>

#include "editors/ComponentEditor/common/ListParameterFinder.h"
#include "editors/ComponentEditor/common/IPXactSystemVerilogParser.h"

#include <Plugins/common/NameGenerationPolicy.h>

//-----------------------------------------------------------------------------
// Function: MakefileParser::MakefileParser()
//-----------------------------------------------------------------------------
MakefileParser::MakefileParser( LibraryInterface* library, SWStackParser& swStackParser ) :
library_( library ), swStackParser_(swStackParser), parsedData_( QSharedPointer<QList<QSharedPointer<MakeFileData> > >
	( new QList<QSharedPointer<MakeFileData> > ) )
{
}

//-----------------------------------------------------------------------------
// Function: MakefileParser::~MakefileParser()
//-----------------------------------------------------------------------------
MakefileParser::~MakefileParser()
{
}

//-----------------------------------------------------------------------------
// Function: MakefileParser::getParsedData()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<MakeFileData> > > MakefileParser::getParsedData()
{
    return parsedData_;
}

//-----------------------------------------------------------------------------
// Function: MakefileParser::searchSWComponent()
//-----------------------------------------------------------------------------
void MakefileParser::parse(QSharedPointer<Component> topComponent)
{
	foreach (QSharedPointer<MakeFileData> makeData, *swStackParser_.getParsedData())
	{
		// Path to the top component is needed to construct the absolute path.
		QFileInfo componentQfi = QFileInfo(library_->getPath(topComponent->getVlnv()));
		QString componentPath = componentQfi.absolutePath() + "/";

		foreach (QSharedPointer<StackPart> stackPart, makeData->parts)
		{
			// Also parse the associated instance headers, if any exist.
			if (stackPart->instanceHeaders)
			{
				parseFileSet(stackPart->instanceHeaders, makeData, stackPart, componentPath);
			}

			// Parse the files of the given software view.
			parseMakeObjects(makeData, stackPart);
		}

		// Now we may check the compiler and flags of individual files...
		foreach (QSharedPointer<MakeObjectData> mod, makeData->swObjects)
		{
			mod->compiler = getFileCompiler(mod, makeData->hardPart->buildCmd);
			mod->flags = getFileFlags(makeData->parts.first()->component, mod, makeData );
		}

		// Check if software objects found: If none, no need for a makefile.
		if (makeData->swObjects.isEmpty())
		{
			continue;
		}

		// Remove duplicates: They are not needed.
		makeData->includeDirectories.removeDuplicates();
		makeData->componentInstantiationFlags.removeDuplicates();

		// Finally, see if there are any conflicts within the potential object files.
		findConflicts(makeData);

		// Append to the list.
		parsedData_->append(makeData);
	}
}

//-----------------------------------------------------------------------------
// Function: MakefileParser::parseMakeObjects()
//-----------------------------------------------------------------------------
void MakefileParser::parseMakeObjects( QSharedPointer<MakeFileData> makeData, 
	QSharedPointer<StackPart> stackPart )
{
	// Path to the component is needed to construct the absolute path.
	QFileInfo componentQfi = QFileInfo(library_->getPath(stackPart->component->getVlnv()));
	QString componentPath = componentQfi.absolutePath() + "/";

    // Go through the fileSets referenced in the software view.
    foreach(QString fsetName, *stackPart->instantiation->getFileSetReferences())
    {
        QSharedPointer<FileSet> fset = stackPart->component->getFileSet(fsetName);

		// Skip, if no such file set exist!
		if (fset)
		{
			parseFileSet(fset, makeData, stackPart, componentPath );
		}
    }
}

//-----------------------------------------------------------------------------
// Function: MakefileParser::parseFileSet()
//-----------------------------------------------------------------------------
void MakefileParser::parseFileSet(QSharedPointer<FileSet> fset, QSharedPointer<MakeFileData> makeData, 
	QSharedPointer<StackPart> stackPart, QString& componentPath)
{
	// Go through the files in the fileSet.
	foreach( QSharedPointer<File> file, *fset->getFiles())
	{
		// First of all, the selected build commands must support the file type.
		if (!file->getFileTypes()->contains(stackPart->buildCmd->getFileType()))
		{
			continue;
		}

		// Get info about the file path.
        QFileInfo fileQfi = QFileInfo(componentPath + file->name());

        // In case of an include file:
        if (file->isIncludeFile())
        {
            // Path is added to the collection of the include paths.
            makeData->includeDirectories.append(fileQfi.absolutePath());
            // They do not yield anything else.
            continue;
        }

        // Initialize the data to the collection associated with the makefile data.
        QSharedPointer<MakeObjectData> objectData( new MakeObjectData );
        makeData->swObjects.append(objectData);

		// Set the needed fields.
		objectData->file = file;
		objectData->fileBuildCmd = file->getBuildCommand();
		objectData->fileName = fileQfi.fileName();
		objectData->path = fileQfi.absolutePath();
		objectData->fileSet = fset;
		objectData->swBuildCmd = stackPart->buildCmd;
		objectData->stackPart = stackPart;
        objectData->isChosen = true;

		// A fileSet builder associated with the file type is also a possible field.
		foreach(QSharedPointer<FileBuilder> builder, *fset->getDefaultFileBuilders())
		{
			if (file->getFileTypes()->contains(builder->getFileType()))
			{
				objectData->fileSetBuildCmd = builder;
				break;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Function: MakefileParser::getFileCompiler()
//-----------------------------------------------------------------------------
QString MakefileParser::getFileCompiler(QSharedPointer<MakeObjectData> mod,
	QSharedPointer<FileBuilder> hardBuilder) const
{
	QString compiler;
	QSharedPointer<FileBuilder> swbc;

	// This mesh does following:
	// 1. No file builder -> use fileSet builder
	// 2. No fileSet builder -> use builder of the software instance
	// 3. If nothing else, use the builder of the hardware instance
	if (mod->fileBuildCmd == 0 || mod->fileBuildCmd->getCommand().isEmpty())
	{
		if (mod->fileSetBuildCmd == 0 || mod->fileSetBuildCmd->getCommand().isEmpty())
		{
			if (mod->swBuildCmd == 0 || mod->swBuildCmd->getCommand().isEmpty())
			{
				swbc = hardBuilder;
			}
			else
			{
				swbc = mod->swBuildCmd;
			}
		}
		else if (mod->fileSetBuildCmd != 0)
		{
			compiler = mod->fileSetBuildCmd->getCommand();
		}
	}
	else
	{
		compiler = mod->fileBuildCmd->getCommand();
	}

	// Verify that file has a type matching the build command.
	if (swbc != 0 && mod->file->getFileTypes()->contains(swbc->getFileType()))
	{
		compiler = swbc->getCommand();
	}

	return compiler;
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::getFileFlags()
//-----------------------------------------------------------------------------
QString MakefileParser::getFileFlags(QSharedPointer<Component> component,
	QSharedPointer<MakeObjectData> &mod, QSharedPointer<MakeFileData> makeData) const
{
	QString cFlags;
	int fileReplaceFlags;
	int fileSetReplaceFlags;

	QSharedPointer<ListParameterFinder> finder (new ListParameterFinder());
	finder->setParameterList(component->getParameters());
	QSharedPointer<IPXactSystemVerilogParser> expressionParser (new IPXactSystemVerilogParser(finder));
	
	// At any rate, the file may have its own flags.
	if (mod->fileBuildCmd != 0)
	{
		cFlags += mod->fileBuildCmd->getFlags();
		fileReplaceFlags =
			expressionParser->parseExpression(mod->fileBuildCmd->getReplaceDefaultFlags()).toInt();
	}

	// See if file set flags replace anything.
	if (mod->fileSetBuildCmd != 0)
	{
		fileSetReplaceFlags =
			expressionParser->parseExpression(mod->fileSetBuildCmd->getReplaceDefaultFlags()).toInt();
	}

	// This mesh does following:
	// 1. If file does not override flags, may use fileSet flags
	// 2. If fileSet does not override flags, may use software flags
	// 2. If software does not override flags, may use hardware flags
	if (mod->fileBuildCmd == 0 || fileReplaceFlags != 1)
	{
		if (mod->fileSetBuildCmd != 0 )
		{
			cFlags += " " + mod->fileSetBuildCmd->getFlags();
		}

		if (mod->fileSetBuildCmd == 0 || fileSetReplaceFlags != 1)
		{
			if (mod->swBuildCmd != 0)
			{
				cFlags += " " + mod->swBuildCmd->getFlags();
			}   

			if ((mod->swBuildCmd == 0 || mod->swBuildCmd->getReplaceDefaultFlags().toInt() != 1)
                && makeData->hardPart->buildCmd != 0 && makeData->hardPart->buildCmd != mod->swBuildCmd)
			{
				cFlags += " " + makeData->hardPart->buildCmd->getFlags();
			}
		}
	}

	return cFlags;
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::findConflicts()
//-----------------------------------------------------------------------------
void MakefileParser::findConflicts(QSharedPointer<MakeFileData> makeData)
{
	// Take a copy of the list, so that they may be removed while evaluating.
	QList<QSharedPointer<MakeObjectData> > objects = makeData->swObjects;

	// Go on until the list is empty.
	while (!objects.empty())
	{
		// Remove the object, so that there will not be a double entry nor self-evaluation.
		QSharedPointer<MakeObjectData> mod = objects.first();
		objects.removeFirst();

		// List of the entires that conflict each other.
		QSet<QSharedPointer<MakeObjectData> > conflictingFiles;

		// The object itself must be one of the sides of the conflict.
		conflictingFiles.insert(mod);

		// Compare it to the others
		foreach (QSharedPointer<MakeObjectData> modCompare, objects)
		{
			// Absolute path is same means that the file is definitely same.
			// Must also consider the filename, as it is omitted from the path.
			if (modCompare->path == mod->path && modCompare->fileName == mod->fileName)
			{
                conflictingFiles.insert(modCompare);
                objects.removeOne(modCompare);
            }
		}

		// One side of the conflict is the file compared to others, there has to be two conflicting files.
		if (conflictingFiles.size() > 1)
		{
			makeData->conflicts.append(conflictingFiles);
		}
	}
}