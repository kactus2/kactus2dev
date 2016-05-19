//-----------------------------------------------------------------------------
// File: MakefileParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
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
// Function: MakefileParser::findConflicts()
//-----------------------------------------------------------------------------
QVector<QSet<QSharedPointer<MakeObjectData> > > MakefileParser::findConflicts() const
{
	QVector<QSet<QSharedPointer<MakeObjectData> > > conflicts;

	foreach(QSharedPointer<MakeFileData> makefileData, *parsedData_)
	{
		QList<QSharedPointer<MakeObjectData> > objects = makefileData->swObjects;

		// Go on until the list is empty.
		while (!objects.empty())
		{
			// Remove the object, so that there will not be a double entry nor self-evaluation.
			QSharedPointer<MakeObjectData> mod = objects.first();
			objects.removeFirst();

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
					bool conflict = true;

					// First see if the compilers are the same, and the include file setting.
					if (modCompare->compiler == mod->compiler &&
                        modCompare->file->isIncludeFile() == mod->file->isIncludeFile())
					{
						// Create list of flags.
						QStringList flagList = mod->flags.split(" ", QString::SkipEmptyParts);
						QStringList flagListCompare = modCompare->flags.split(" ", QString::SkipEmptyParts);

						if (flagList.size() == flagListCompare.size())
						{
							// Are the same size -> Proceed to check if content is the same.
							flagList.sort(Qt::CaseSensitive);
							flagListCompare.sort(Qt::CaseSensitive);
							conflict = false;

							// Now go through the lists and see if they are the same.
                            int flagCount = flagList.size();
							for (int i = 0; i < flagCount && !conflict; i++)
							{
								if (flagList[i] != flagListCompare[i])
								{
									conflict = true;
								}
							}
						}
					}

					// Conflict confirmed: Add to the list.
					if (conflict)
					{
						conflictingFiles.insert(modCompare);
					}
				}
			}

			// One side of the conflict is the file compared to others, there has to be two conflicting files.
			if (conflictingFiles.size() > 1)
			{
				conflicts.append(conflictingFiles);
			}
		}
	}

	return conflicts;
}

//-----------------------------------------------------------------------------
// Function: MakefileParser::searchSWComponent()
//-----------------------------------------------------------------------------
void MakefileParser::parse(QSharedPointer<Component> topComponent)
{
	foreach (QSharedPointer<MakeFileData> makeData, *swStackParser_.getParsedData())
	{
		// Parse the files of hard view.
		parseMakeObjects(makeData, makeData->hardView, makeData->hwBuildCmd,
			makeData->hardComponent);

		// Path to the top component is needed to construct the absolute path.
		QFileInfo componentQfi = QFileInfo(library_->getPath(topComponent->getVlnv()));
		QString componentPath = componentQfi.absolutePath() + "/";

		// Also parse the associated instance headers.
		parseFileSet(makeData->instanceHeaders, makeData, makeData->hwBuildCmd, componentPath);

		foreach (QSharedPointer<StackPart> stackPart, makeData->parts_)
		{
			// Parse the files of the given software view.
			parseMakeObjects(makeData, stackPart->view, stackPart->swBuildCmd, stackPart->component );
		}

		// Now we may check the compiler and flags of individual files...
		foreach ( QSharedPointer<MakeObjectData> mod, makeData->swObjects )
		{
			mod->compiler = getFileCompiler( mod, makeData->hwBuildCmd );
			mod->flags = getFileFlags(makeData->parts_.first()->component, mod, makeData );
		}

		// Check if software objects found: If none, no need for a makefile.
		if (makeData->swObjects.isEmpty())
		{
			continue;
		}

		// Remove duplicates: They are not needed.
		makeData->includeDirectories.removeDuplicates();
		makeData->softViewFlags.removeDuplicates();

		// Append to the list.
		parsedData_->append(makeData);
	}
}

//-----------------------------------------------------------------------------
// Function: MakefileParser::parseMakeObjects()
//-----------------------------------------------------------------------------
void MakefileParser::parseMakeObjects( QSharedPointer<MakeFileData> makeData, QSharedPointer<SWView> view,
	QSharedPointer<SWFileBuilder> buildCmd, QSharedPointer<Component> component )
{
	// Path to the component is needed to construct the absolute path.
	QFileInfo componentQfi = QFileInfo(library_->getPath(component->getVlnv()));
	QString componentPath = componentQfi.absolutePath() + "/";

    // Go through the fileSets referenced in the software view.
    foreach( QString fsetName, view->getFileSetRefs())
    {
        QSharedPointer<FileSet> fset = component->getFileSet(fsetName);

		// Skip, if no such file set exist!
		if ( fset )
		{
			parseFileSet(fset, makeData, buildCmd, componentPath );
		}
    }
}

//-----------------------------------------------------------------------------
// Function: MakefileParser::parseFileSet()
//-----------------------------------------------------------------------------
void MakefileParser::parseFileSet(QSharedPointer<FileSet> fset, QSharedPointer<MakeFileData> makeData, 
	QSharedPointer<SWFileBuilder> buildCmd, QString& componentPath)
{
	// Go through the files in the fileSet.
	foreach( QSharedPointer<File> file, *fset->getFiles())
	{
		// First of all, the selected build commands must support the file type.
		if ( !file->getFileTypes()->contains( buildCmd->getFileType() ) )
		{
			continue;
		}

		// Initialize the data to the collection associated with the makefile data.
		QSharedPointer<MakeObjectData> objectData( new MakeObjectData );
		makeData->swObjects.append(objectData);

		// Get info about the file.
		QFileInfo fileQfi = QFileInfo( componentPath + file->name() );

		// Set the needed fields.
		objectData->file = file;
		objectData->fileBuildCmd = file->getBuildCommand();
		objectData->fileName = fileQfi.fileName();
		objectData->path = fileQfi.absolutePath();
		objectData->fileSet = fset;
		objectData->swBuildCmd = buildCmd;

		// In case of an include file:
		if ( file->isIncludeFile() )
		{
			// Its path is added to the collection of the include paths.
			makeData->includeDirectories.append(fileQfi.absolutePath());
			// Itself is added to the collection of the include files.
			makeData->includeFiles.append(objectData);
		}

		// A fileSet builder associated with the file type is also a possible field.
		foreach( QSharedPointer<FileBuilder> builder, *fset->getDefaultFileBuilders() )
		{
			if ( file->getFileTypes()->contains( builder->getFileType() ) )
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
	QSharedPointer<SWFileBuilder> hardBuilder) const
{
	QString compiler;
	QSharedPointer<SWFileBuilder> swbc;

	// This mesh does following:
	// 1. No file builder -> use fileSet builder
	// 2. No fileSet builder -> use builder of the software view of the software instance
	// 3. If nothing else, use the builder of the software view of the hardware instance
	if ( mod->fileBuildCmd == 0 || mod->fileBuildCmd->getCommand().isEmpty() )
	{
		if ( mod->fileSetBuildCmd == 0 || mod->fileSetBuildCmd->getCommand().isEmpty() )
		{
			if ( mod->swBuildCmd == 0 || mod->swBuildCmd->getCommand().isEmpty() )
			{
				swbc = hardBuilder;
			}
			else
			{
				swbc = mod->swBuildCmd;
			}
		}
		else if ( mod->fileSetBuildCmd != 0 )
		{
			compiler = mod->fileSetBuildCmd->getCommand();
		}
	}
	else
	{
		compiler = mod->fileBuildCmd->getCommand();
	}

	// Verify that file has a type matching the build command.
	if ( swbc != 0 && mod->file->getFileTypes()->contains( swbc->getFileType() ) )
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
	if ( mod->fileBuildCmd != 0 )
	{
		cFlags += mod->fileBuildCmd->getFlags();
		fileReplaceFlags =
			expressionParser->parseExpression(mod->fileBuildCmd->getReplaceDefaultFlags()).toInt();
	}

	// See if file set flags replace anything.
	if ( mod->fileSetBuildCmd != 0 )
	{
		fileSetReplaceFlags =
			expressionParser->parseExpression(mod->fileSetBuildCmd->getReplaceDefaultFlags()).toInt();
	}

	// This mesh does following:
	// 1. If file does not override flags, may use fileSet flags
	// 2. If fileSet does not override flags, may use software flags
	// 2. If software does not override flags, may use hardware flags
	if ( mod->fileBuildCmd == 0 || fileReplaceFlags != 1 )
	{
		if ( mod->fileSetBuildCmd != 0 )
		{
			cFlags += " " + mod->fileSetBuildCmd->getFlags();
		}

		if ( mod->fileSetBuildCmd == 0 || fileSetReplaceFlags != 1 )
		{
			if ( mod->swBuildCmd != 0 )
			{
				cFlags += " " + mod->swBuildCmd->getFlags();
			}   

			if ( ( mod->swBuildCmd == 0 || mod->swBuildCmd->getReplaceDefaultFlags().toInt() != 1 )
                && makeData->hwBuildCmd != 0 && makeData->hwBuildCmd != mod->swBuildCmd )
			{
				cFlags += " " + makeData->hwBuildCmd->getFlags();
			}
		}
	}

	return cFlags;
}