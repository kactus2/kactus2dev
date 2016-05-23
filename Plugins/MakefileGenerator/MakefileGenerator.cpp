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
#include <QDir>
#include <QTextStream>
#include <QIODevice> 
#include <IPXACTmodels/Component/FileSet.h>

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::MakefileGenerator()
//-----------------------------------------------------------------------------
MakefileGenerator::MakefileGenerator( MakefileParser& parser, IPluginUtility* utility,
	QSharedPointer<FileSet> generalFileSet ) :
	parsedData_( parser.getParsedData() ), utility_( utility ), generalFileSet_( generalFileSet )
{
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::~MakefileGenerator()
//-----------------------------------------------------------------------------
MakefileGenerator::~MakefileGenerator()
{
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::generate()
//-----------------------------------------------------------------------------
int MakefileGenerator::generate(QString targetPath, QString topPath, QString sysViewName)
{
    // Names of the created directories to be referenced by the master makefile.
    QStringList makeNames;
    // Create base directory for the software.
    QString basePath = targetPath + "/sw_" + sysViewName + "/";
    QDir path;
    path.mkpath( basePath );

    foreach(QSharedPointer<MakeFileData> mfd, *parsedData_)
    {
        generateInstanceMakefile(topPath, mfd, makeNames);
    }

	if ( makeNames.count() > 0 )
	{
		generateMainMakefile(basePath, topPath, makeNames);

		generateLauncher(basePath, topPath, makeNames);
	}

	return makeNames.count();
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::generateInstanceMakefile()
//-----------------------------------------------------------------------------
void MakefileGenerator::generateInstanceMakefile(QString topPath,
    QSharedPointer<MakeFileData> makeData, QStringList &makeNames)
{
	// Nothing to make -> no action.
	if ( makeData->swObjects.count() < 1 )
	{
		utility_->printError("No objects for makefile. Top instance: " + makeData->name);
		return;
	}

	// No path -> no action.
	if ( makeData->targetPath.isEmpty() )
	{
		utility_->printError("Empty path for a makefile. Top instance: " + makeData->name);
		return;
	}

    // Construct the path and folder.
    QDir path;
    path.mkpath( makeData->targetPath );

    // Create the makefile.
    QString makePath = makeData->targetPath + "/Makefile"; 
    QFile makeFile(makePath);
    
	// If it cannot be written, then it is too bad.
    if ( !makeFile.open(QIODevice::WriteOnly) )
    {
        utility_->printError("Could not open the makefile at location " + makePath);
		utility_->printError("Reason: " + makeFile.errorString());
		utility_->printError("Top instance: " + makeData->name);
		return;
    }

	// Nicer to handle as a stream.
    QTextStream outStream(&makeFile);

    // Add the path to the list of created makefiles for later reference.
    makeNames.append( makeData->targetPath );

    // Write the paths of directories containing includes.
    outStream << "_INCLUDES=";

    foreach( QString path, makeData->includeDirectories )
    {
        outStream << " " << General::getRelativePath(makeData->targetPath,path);
    }

    outStream << endl;
	// Will make the -I option out of every included directory.
    outStream << "INCLUDES=$(patsubst %, -I%, $(_INCLUDES))" << endl << endl;

    // The common dependency list.
	// Currently features only Makefile, in case a new one is generated.
    outStream << "DEPS= " << MAKEFILE_NAME << endl << endl;

    // Other stuff is in their own functions.
    writeFinalFlagsAndBuilder(makeData, outStream);
    writeObjectList(makeData, outStream);
	writeExeBuild(outStream);
	writeCleanRules(outStream);

	// Create rule for using debugging and profiling options
	outStream << "DEBUG_FLAGS +=" << endl;
	outStream << "debug: DEBUG_FLAGS += -ggdb -fno-omit-frame-pointer -fno-inline-functions "
		"-fno-inline-functions-called-once -fno-optimize-sibling-calls" << endl;
	outStream << "debug: $(ENAME)" << endl << endl;

	outStream << "PROFILE_FLAGS +=" << endl;
	outStream << "profile: PROFILE_FLAGS += -pg -fno-omit-frame-pointer -fno-inline-functions "
		"-fno-inline-functions-called-once -fno-optimize-sibling-calls" << endl;
	outStream << "profile: $(ENAME)" << endl << endl;

	// Rule to include generated dependency files to the compilation.
	outStream << "-include $(OBJ:%.o=%.d)" << endl;

	// Write the compilation rules for the object files.
    writeMakeObjects(outStream, makeData->swObjects, makeData->targetPath);

    // Close after it is done.
    makeFile.close();

    QSharedPointer<FileSet> fileSet = makeData->instanceHeaders;

    // The path in the file set must be relative to the basePath.
    QString relPath = General::getRelativePath(topPath,makePath);

    // Add the file to instance fileSet, if it does not exist there.
    if ( fileSet && !fileSet->contains(relPath) )
    {
        QSharedPointer<File> file;
        QSettings settings;
        file = fileSet->addFile(relPath, settings);
        file->addFileType("makefile");
    }
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::generateMainMakefile()
//-----------------------------------------------------------------------------
void MakefileGenerator::generateMainMakefile(QString basePath, QString topPath, QStringList makeNames) const
{
    QString dir = basePath + MAKEFILE_NAME;
    // Create the master makefile.
    QFile makeFile( basePath + MAKEFILE_NAME );
    makeFile.open(QIODevice::WriteOnly);
    QTextStream outStream(&makeFile);

    // Default target for each directory.
    outStream << "make:";

    foreach( QString directory, makeNames )
    {
        outStream << endl << "\t(cd " << General::getRelativePath(basePath,directory) << "; make)";
    }

    // Needs also cleaner for each directory.
    outStream << endl << endl << "clean:";

    foreach( QString directory, makeNames )
    {
        outStream << endl << "\t(cd " << General::getRelativePath(basePath,directory) << "; make clean)";
	}

	// Debug target for each directory.
	outStream << endl << endl << "debug:";

	foreach( QString directory, makeNames )
	{
		outStream << endl << "\t(cd " << General::getRelativePath(basePath,directory) << "; make debug)";
	}

	// Profiling target for each directory.
	outStream << endl << endl << "profile:";

	foreach( QString directory, makeNames )
	{
		outStream << endl << "\t(cd " << General::getRelativePath(basePath,directory) << "; make profile)";
	}

    // Close after it is done.
    makeFile.close();

    // The path in the fileSet must be relative to the basePath.
    QString relDir = General::getRelativePath(topPath,dir);

    // Add the file to instance fileSet
    if ( generalFileSet_ && !generalFileSet_->contains(relDir) )
    {
        QSharedPointer<File> file;
        QSettings settings;
        file = generalFileSet_->addFile(relDir, settings);
        file->addFileType("makefile");
    }
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::generateLauncher()
//-----------------------------------------------------------------------------
void MakefileGenerator::generateLauncher(QString basePath, QString topPath, QStringList makeNames) const
{
    QString dir = basePath + "launcher.sh";
    QFile launcherFile( dir );
    launcherFile.open(QIODevice::WriteOnly);
    QTextStream outStream(&launcherFile);

    writeProcessList(outStream, makeNames, basePath);

    writeShellFunctions(outStream);

    writeProcessLaunch(outStream);

    writeEnding(outStream);

    // Close after it is done.
    launcherFile.close();

    // The path in the fileSet must be relative to the basePath.
    QString relDir = General::getRelativePath(topPath,dir);

    // Add the file to instance fileSet
    if ( generalFileSet_ && !generalFileSet_->contains(relDir) )
    {
        QSharedPointer<File> file;
        QSettings settings;
        file = generalFileSet_->addFile(relDir, settings);
        file->addFileType( "shellScript" );
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
    if ( mfd->hardPart->buildCmd != 0 )
    {
        finalBuilder = mfd->hardPart->buildCmd->getCommand();

        finalFlags += " " + mfd->hardPart->buildCmd->getFlags();
    }

    // All flags of all software views must be appended to the flags.
    foreach ( QString flag, mfd->softViewFlags )
    {
        finalFlags += " " + flag;
    }

    // Finally, write down what we learned.
    outStream << "ENAME= " << mfd->name << endl;
    outStream << "EFLAGS= " << finalFlags << endl;
    outStream << "EBUILDER= " << finalBuilder << endl;
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::writeObjectList()
//-----------------------------------------------------------------------------
void MakefileGenerator::writeObjectList(QSharedPointer<MakeFileData> mfd, QTextStream& outStream) const
{
    // Include files are skipped and the object file is simply original filename + ".o".
    outStream << "_OBJ=";

    foreach(QSharedPointer<MakeObjectData> mod, mfd->swObjects)
    {
        if ( ( !mod->file || !mod->file->isIncludeFile() ) && !mod->compiler.isEmpty() )
        {
            outStream << " " << mod->fileName << ".o";
        }
    }

    // Finally, write down what we learned.
    outStream << endl;
    outStream << "ODIR= obj" << endl;
    outStream << "OBJ= $(patsubst %,$(ODIR)/%,$(_OBJ))" << endl << endl;
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::writeExeBuild()
//-----------------------------------------------------------------------------
void MakefileGenerator::writeExeBuild(QTextStream& outStream) const
{
    // Rather straight forward: write constant build rule and a cleaner rule.
    outStream << "$(ENAME): $(OBJ)" << endl;
    outStream << "\t$(EBUILDER) -o $(ENAME) $(OBJ) $(EFLAGS)"
        << endl << endl;
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::writeCleanRules()
//-----------------------------------------------------------------------------
void MakefileGenerator::writeCleanRules(QTextStream& outStream) const
{
	// Delete all known object files. May leave renamed files undeleted, but is more secure than deleting all
	// content of the object directory.
	outStream << "clean:" << endl;
	outStream << "\trm -f $(OBJ:%.o=%.d);" << endl;
	outStream << "\trm -f $(OBJ);" << endl << endl;

	// Make a directory for the object files.
	outStream << "all: $(OBJ)" << endl << endl;
	outStream << "$(OBJ): | $(ODIR)" << endl << endl;
	outStream << "$(ODIR):" << endl << "\tmkdir -p $(ODIR)" << endl << endl;
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::writeMakeObjects()
//-----------------------------------------------------------------------------
void MakefileGenerator::writeMakeObjects(QTextStream& outStream, 
    QList<QSharedPointer<MakeObjectData> >& objects, QString instancePath) const
{
    foreach(QSharedPointer<MakeObjectData> mod, objects)
    {
        // Skip the include files, as they do not get object files. Also compilerless files are skipped.
        if ( ( mod->file && mod->file->isIncludeFile() ) || mod->compiler.isEmpty() )
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
        outStream << endl;
        outStream << "$(ODIR)/" << fileName << ".o: $(DEPS) " << relPath << "/" << fileName << endl;
        outStream << "\t" << mod->compiler << " " << DEFAULT_OBJECT_FLAGS << " $(ODIR)/" << fileName << ".o " <<
            relPath << "/" << fileName << " " << cFlags << endl;
    }
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::writeProcessList()
//-----------------------------------------------------------------------------
void MakefileGenerator::writeProcessList(QTextStream& outStream, QStringList makeNames, QString basePath) const
{
    outStream << "PROCESSES=(";

    // The list consists of relative path to each executable.
    foreach( QString directory, makeNames )
    {
        QFileInfo qfi = QFileInfo( directory );

        outStream << General::getRelativePath(basePath,directory) << "/" << qfi.fileName() << " ";
    }

    outStream << ")" << endl;
    outStream << "echo ${PROCESSES[@]}" << endl << endl;
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::writeShellFunctions()
//-----------------------------------------------------------------------------
void MakefileGenerator::writeShellFunctions(QTextStream& outStream) const
{
    // Function called from the signal trap to first kill all processes, then quit the script.
    outStream << "terminate () {" << endl;
    outStream << "\tcommit_hits" << endl;
    outStream << "\texit" << endl;
    outStream << "}" << endl << endl;

    // Function killing all processes, if none exists.
    outStream << "commit_hits () {" << endl;
    outStream << "\thitlist=hitlist.txt" << endl << endl;

    // Check first there is existing hit list.
    outStream << "\tif [ -f $hitlist ]; then" << endl;

    // If does, loop through it and kill by name.
    outStream << "\t\techo \"KILLING PROCESSES\"" << endl << endl;
    outStream << "\t\twhile read line" << endl;
    outStream << "\t\tdo" << endl;
    outStream << "\t\t\tkillall $line &> /dev/null" << endl;
    outStream << "\t\tdone < $hitlist" << endl;

    outStream << "\tfi" << endl;
    outStream << "}" << endl << endl;
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::writeProcessLaunch()
//-----------------------------------------------------------------------------
void MakefileGenerator::writeProcessLaunch(QTextStream& outStream) const
{
    // Before launch proper, kill pre existing processes and remove the hit list.
    outStream << "commit_hits" << endl;
    outStream << "rm $hitlist &> /dev/null" << endl << endl;

    outStream << "echo \"LAUNCHING NEW PROCESSES\"" << endl;

    // Loop throug the list of processes.
    outStream << "for name in \"${PROCESSES[@]}\"" << endl << "do" << endl;
        // Launch it and record the pid for later use.
        outStream << "\t./$name &" << endl;
        outStream << "\tmypid=$!" << endl;
        outStream << "\tpids=$pids\"$mypid \"" << endl;

        // Record the name to hit list.
        outStream << "\techo \"$name\" >> $hitlist" << endl;
    outStream << "done" << endl << endl;
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::writeEnding()
//-----------------------------------------------------------------------------
void MakefileGenerator::writeEnding(QTextStream& outStream) const
{
    // Signal trap to called when calling suspend process.
    outStream << "trap \"terminate\" SIGTSTP" << endl;

    // Wait for the processes to finish before exiting the script execution.
    outStream << "echo \"PROCESSES LAUNCHED\"" << endl;
    outStream << "wait $pids" << endl << endl;

    outStream << "echo \"DONE\"" << endl;
}

