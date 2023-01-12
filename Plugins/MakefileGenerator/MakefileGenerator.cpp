//-----------------------------------------------------------------------------
// File: MakefileGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 02.09.2014
//
// Description:
// Makefile generator.
//-----------------------------------------------------------------------------

#include "MakefileGenerator.h"

#include <QFile>
#include <QTextStream>
#include <QIODevice> 
#include <QFileInfo>
#include <QDir>

#include <IPXACTmodels/Component/FileSet.h>

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::MakefileGenerator()
//-----------------------------------------------------------------------------
MakefileGenerator::MakefileGenerator(MakefileParser& parser, IPluginUtility* utility,
	QSharedPointer<FileSet> generalFileSet) :
	parsedData_(parser.getParsedData()),
    utility_(utility),
    generalFileSet_(generalFileSet)
{
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::generate()
//-----------------------------------------------------------------------------
int MakefileGenerator::generate(QString const& targetPath, QString const& topPath)
{
    // Names of the created directories to be referenced by the master makefile.
    QStringList makeNames;

    // Try to create a make file out of each parsed makefile data.
    foreach(QSharedPointer<MakeFileData> mfd, *parsedData_)
    {
        generateInstanceMakefile(targetPath, topPath, mfd, makeNames);
    }

    // Generate the main makefile only if there was any other created makefiles.
	if (makeNames.count() > 0)
	{
		generateMainMakefile(targetPath, topPath, makeNames);
	}

	return makeNames.count();
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::generateInstanceMakefile()
//-----------------------------------------------------------------------------
void MakefileGenerator::generateInstanceMakefile(QString const& targetPath, QString const& componentPath,
    QSharedPointer<MakeFileData> makeData, QStringList& makeNames)
{
	// Nothing to make -> no action.
	if (makeData->swObjects.count() < 1)
	{
		utility_->printError("No objects for makefile. Top instance: " + makeData->name);
		return;
	}

	// No path -> no action.
	if (makeData->makeName.isEmpty())
	{
		utility_->printError("Empty path for a makefile. Top instance: " + makeData->name);
		return;
	}

    // Form the absolute bath for the makefile.
    QString pathString = targetPath + "/" + makeData->makeName;
    QFileInfo qfi(pathString);
    QString absolutePathString = qfi.absolutePath() + "/";
    // Create path as needed.
    QDir makeDir(qfi.absolutePath());

    if (!makeDir.exists() && !makeDir.mkdir(absolutePathString))
    {
        utility_->printError("Could not create path for the makefile: " + absolutePathString);
        return;
    }

    // Create the makefile.
    QFile makeFile(pathString);
    
	// If it cannot be written, then it is too bad.
    if (!makeFile.open(QIODevice::WriteOnly))
    {
        utility_->printError("Could not open the makefile at location " + pathString);
		utility_->printError("Reason: " + makeFile.errorString());
		utility_->printError("Top instance: " + makeData->name);
		return;
    }

	// Nicer to handle as a stream.
    QTextStream outStream(&makeFile);

    // Write the paths of directories containing includes.
    outStream << "_INCLUDES=";

    foreach(QString includePath, makeData->includeDirectories)
    {
        outStream << " " << General::getRelativePath(absolutePathString, includePath);
    }

    outStream << Qt::endl;
	// Will make the -I option out of every included directory.
    outStream << "INCLUDES=$(patsubst %, -I%, $(_INCLUDES))" << Qt::endl << Qt::endl;

    // The common dependency list.
	// Currently features only Makefile, in case a new one is generated.
    outStream << "DEPS= " << qfi.fileName() << Qt::endl << Qt::endl;

    // Other stuff is in their own functions.
    writeFinalFlagsAndBuilder(makeData, outStream);
    writeObjectList(makeData, outStream);
	writeExeBuild(outStream);
	writeCleanRules(outStream);

	// Create rule for using debugging and profiling options
	outStream << "DEBUG_FLAGS +=" << Qt::endl;
	outStream << "debug: DEBUG_FLAGS += -ggdb -fno-omit-frame-pointer -fno-inline-functions "
		"-fno-inline-functions-called-once -fno-optimize-sibling-calls" << Qt::endl;
	outStream << "debug: $(ENAME)" << Qt::endl << Qt::endl;

	outStream << "PROFILE_FLAGS +=" << Qt::endl;
	outStream << "profile: PROFILE_FLAGS += -pg -fno-omit-frame-pointer -fno-inline-functions "
		"-fno-inline-functions-called-once -fno-optimize-sibling-calls" << Qt::endl;
	outStream << "profile: $(ENAME)" << Qt::endl << Qt::endl;

	// Rule to include generated dependency files to the compilation.
	outStream << "-include $(OBJ:%.o=%.d)";

	// Write the compilation rules for the object files.
    writeMakeObjects(outStream, makeData->swObjects, absolutePathString);

    // Close after it is done.
    makeFile.close();

    // The file set is the instance file set earlier defined for the make file.
    QSharedPointer<FileSet> fileSet = makeData->instanceFileSet;

    // Must create a new file with path relative to the component path.
    QString relPath = General::getRelativePath(componentPath, absolutePathString);

    // Add the file to instance file set, if it does not exist there.
    if (fileSet && !fileSet->contains(relPath))
    {
        QSharedPointer<File> file;
        QSettings settings;
        file = fileSet->addFile(relPath, settings);
        file->addFileType("makefile");
    }

    // Add to the list of created makefiles for later reference.
    makeNames.append(makeData->makeName);
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::generateMainMakefile()
//-----------------------------------------------------------------------------
void MakefileGenerator::generateMainMakefile(QString const& targetPath, QString const& componentPath, 
    QStringList& makeNames) const
{
    QString pathString = targetPath + "/" + MakeCommon::MAKEFILE_MASTER_NAME;
    // Create the master makefile.
    QFile makeFile(pathString);

    // If it cannot be written, then it is too bad.
    if (!makeFile.open(QIODevice::WriteOnly))
    {
        utility_->printError("Could not open the makefile at location " + pathString);
        utility_->printError("Reason: " + makeFile.errorString());
        return;
    }

    // Nicer to handle as a stream.
    QTextStream outStream(&makeFile);

    // Default target for each directory.
    outStream << "make:";

    foreach(QString directory, makeNames)
    {
        QFileInfo qfi(targetPath + "/" + directory);
        outStream << Qt::endl << "\t(cd " << General::getRelativePath(targetPath, qfi.absolutePath()) << "; make -f " << qfi.fileName() << ")";
    }

    // Needs also cleaner for each directory.
    outStream << Qt::endl << Qt::endl << "clean:";

    foreach(QString directory, makeNames)
    {
        QFileInfo qfi(targetPath + "/" + directory);
        outStream << Qt::endl << "\t(cd " << General::getRelativePath(targetPath, qfi.absolutePath()) << "; make clean -f " << qfi.fileName() << ")";
	}

	// Debug target for each directory.
	outStream << Qt::endl << Qt::endl << "debug:";

	foreach(QString directory, makeNames)
    {
        QFileInfo qfi(targetPath + "/" + directory);
        outStream << Qt::endl << "\t(cd " << General::getRelativePath(targetPath, qfi.absolutePath()) << "; make debug -f " << qfi.fileName() << ")";
	}

	// Profiling target for each directory.
	outStream << Qt::endl << Qt::endl << "profile:";

	foreach(QString directory, makeNames)
    {
        QFileInfo qfi(targetPath + "/" + directory);
		outStream << Qt::endl << "\t(cd " << General::getRelativePath(targetPath, qfi.absolutePath()) << "; make profile -f " << qfi.fileName() << ")";
	}

    // Close after it is done.
    makeFile.close();

    // The path in the fileSet must be relative to the basePath.
    QString relDir = General::getRelativePath(componentPath, pathString);

    // Add the file to instance fileSet
    if (generalFileSet_ && !generalFileSet_->contains(relDir))
    {
        QSharedPointer<File> file;
        QSettings settings;
        file = generalFileSet_->addFile(relDir, settings);
        file->addFileType("makefile");
    }
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::writeFinalFlagsAndBuilder()
//-----------------------------------------------------------------------------
void MakefileGenerator::writeFinalFlagsAndBuilder(QSharedPointer<MakeFileData> mfd, QTextStream& outStream) const
{
    QString finalFlags = "$(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS)";
    QString finalBuilder;

    // If build command of software view of the hardware instance exist, its properties are used.
    if (mfd->hardPart->buildCmd != 0)
    {
        finalBuilder = mfd->hardPart->buildCmd->getCommand();

        finalFlags += " " + mfd->hardPart->buildCmd->getFlags();
    }

    // All flags of all software views must be appended to the flags.
    foreach (QString flag, mfd->componentInstantiationFlags)
    {
        finalFlags += " " + flag;
    }

    // Finally, write down what we learned.
    outStream << "ENAME= " << mfd->name << Qt::endl;
    outStream << "EFLAGS= " << finalFlags << Qt::endl;
    outStream << "EBUILDER= " << finalBuilder << Qt::endl;
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::writeObjectList()
//-----------------------------------------------------------------------------
void MakefileGenerator::writeObjectList(QSharedPointer<MakeFileData> mfd, QTextStream& outStream) const
{
    // Include files are skipped and the object file is simply original filename + ".o".
    outStream << "_OBJ=";

    foreach (QSharedPointer<MakeObjectData> mod, mfd->swObjects)
    {
        if (!mod->compiler.isEmpty())
        {
            outStream << " " << mod->fileName << ".o";
        }
    }

    // Finally, write down what we learned.
    outStream << Qt::endl;
    outStream << "ODIR= obj" << Qt::endl;
    outStream << "OBJ= $(patsubst %,$(ODIR)/%,$(_OBJ))" << Qt::endl << Qt::endl;
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::writeExeBuild()
//-----------------------------------------------------------------------------
void MakefileGenerator::writeExeBuild(QTextStream& outStream) const
{
    // Rather straight forward: write constant build rule and a cleaner rule.
    outStream << "$(ENAME): $(OBJ)" << Qt::endl;
    outStream << "\t$(EBUILDER) -o $(ENAME) $(OBJ) $(EFLAGS)"
        << Qt::endl << Qt::endl;
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::writeCleanRules()
//-----------------------------------------------------------------------------
void MakefileGenerator::writeCleanRules(QTextStream& outStream) const
{
	// Delete all known object files. May leave renamed files undeleted, but is more secure than deleting all
	// content of the object directory.
	outStream << "clean:" << Qt::endl;
	outStream << "\trm -f $(OBJ:%.o=%.d);" << Qt::endl;
	outStream << "\trm -f $(OBJ);" << Qt::endl << Qt::endl;

	// Make a directory for the object files.
	outStream << "all: $(OBJ)" << Qt::endl << Qt::endl;
	outStream << "$(OBJ): | $(ODIR)" << Qt::endl << Qt::endl;
	outStream << "$(ODIR):" << Qt::endl << "\tmkdir -p $(ODIR)" << Qt::endl << Qt::endl;
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::writeMakeObjects()
//-----------------------------------------------------------------------------
void MakefileGenerator::writeMakeObjects(QTextStream& outStream, 
    QList<QSharedPointer<MakeObjectData> >& objects, QString instancePath) const
{
    foreach (QSharedPointer<MakeObjectData> mod, objects)
    {
        // Compilerless files are skipped. Moreover, use may have opted for exclusion.
        if (!mod->isChosen || mod->compiler.isEmpty())
        {
            continue;
        }

        // Flags will always include at least the includes, and possibility for debug and profiling flags.
        QString cFlags = "$(INCLUDES) $(DEBUG_FLAGS) $(PROFILE_FLAGS) ";

        // More flags may come from the parsed data.
        cFlags += mod->flags;

        // The relative path is needed by the make and the builder to access the source file.
        QString relPath = General::getRelativePath(instancePath,mod->path);
        QString fileName = mod->fileName;

        // Write the rule for building the individual object file, including dependencies.
        outStream << Qt::endl << Qt::endl;
        outStream << "$(ODIR)/" << fileName << ".o: $(DEPS) " << relPath << "/" << fileName << Qt::endl;
        outStream << "\t" << mod->compiler << " " << DEFAULT_OBJECT_FLAGS << " $(ODIR)/" << fileName << ".o " <<
            relPath << "/" << fileName << " " << cFlags;
    }
}
