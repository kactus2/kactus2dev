//-----------------------------------------------------------------------------
// File: MakefileGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 02.09.2014
//
// Description:
// Makefile generator.
//-----------------------------------------------------------------------------

#include "MakefileGenerator.h"

#include <QFile>
#include <qdir>
#include <qtextstream>

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::MakefileGenerator()
//-----------------------------------------------------------------------------
MakefileGenerator::MakefileGenerator( MakefileParser& parser ) : parsedData_( parser.getParsedData() )
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
void MakefileGenerator::generate(QString targetPath) const
{
    // Names of the created directories to be referenced by the master makefile.
    QStringList makeNames;
    // Create base directory for the software.
    QString basePath = targetPath + "/sw/";
    QDir path;
    path.mkpath( basePath );

    foreach(MakefileParser::MakeFileData mfd, parsedData_)
    {
        generateInstanceMakefile(basePath, mfd, makeNames);
    }

    generateMainMakefile(basePath, makeNames);

    generateLauncher(basePath, makeNames);
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::generateInstanceMakefile()
//-----------------------------------------------------------------------------
void MakefileGenerator::generateInstanceMakefile(QString basePath, MakefileParser::MakeFileData &mfd, QStringList &makeNames) const
{
    // Create directory for the object files.
    QString instancePath = basePath + mfd.name;
    QDir path;
    path.mkpath( instancePath + "/obj" );

    // Create the makefile and open a stream to write it.
    QString dir = instancePath + "/Makefile"; 
    QFile makeFile(dir);
    makeFile.open(QIODevice::WriteOnly);
    QTextStream outStream(&makeFile);

    // Add the path to the list of created makefiles for later reference.
    makeNames.append( instancePath );

    // Begin the file with the includes.
    outStream << "includes=";

    foreach( QString path, mfd.includeDirectories )
    {
        outStream << " -I" << General::getRelativePath(instancePath,path);
    }

    outStream << endl << endl;

    // Other stuff is in their own functions.
    writeFinalFlagsAndBuilder(mfd, outStream);

    writeObjectList(mfd, outStream);
    writeExeBuild(outStream);

    writeMakeObjects(mfd, outStream, mfd.swObjects, instancePath);
    writeMakeObjects(mfd, outStream, mfd.hwObjects, instancePath);

    // Close after it is done.
    makeFile.close();
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::generateMainMakefile()
//-----------------------------------------------------------------------------
void MakefileGenerator::generateMainMakefile(QString basePath, QStringList makeNames) const
{
    // Create the master makefile.
    QFile makeFile( basePath + "Makefile" );
    makeFile.open(QIODevice::WriteOnly);
    QTextStream outStream(&makeFile);

    // Default target would make for each directory.
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

    // Close after it is done.
    makeFile.close();
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::generateLauncher()
//-----------------------------------------------------------------------------
void MakefileGenerator::generateLauncher(QString basePath, QStringList makeNames) const
{
    QFile launcherFile( basePath + "launcher.sh" );
    launcherFile.open(QIODevice::WriteOnly);
    QTextStream outStream(&launcherFile);

    writeProcessList(outStream, makeNames, basePath);

    writeShellFunctions(outStream);

    writeProcessLaunch(outStream);

    writeEnding(outStream);

    // Close after it is done.
    launcherFile.close();
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::writeFinalFlagsAndBuilder()
//-----------------------------------------------------------------------------
void MakefileGenerator::writeFinalFlagsAndBuilder(MakefileParser::MakeFileData &mfd, QTextStream& outStream) const
{
    QString finalFlags = "$(includes)";
    QString finalBuilder;

    // If build command of software view of the hardware instance exist, its properties are used.
    if ( mfd.hwBuildCmd != 0 )
    {
        finalBuilder = mfd.hwBuildCmd->getCommand();

        finalFlags += " " + mfd.hwBuildCmd->getFlags();
    }

    // All flags of all software views must be appended to the flags.
    foreach ( QString flag, mfd.softViewFlags )
    {
        finalFlags += " " + flag;
    }

    // Finally, write down what we learned.
    outStream << "ENAME= " << mfd.name << endl;
    outStream << "EFLAGS= " << finalFlags << endl;
    outStream << "EBUILDER= " << finalBuilder << endl;
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::writeObjectList()
//-----------------------------------------------------------------------------
void MakefileGenerator::writeObjectList(MakefileParser::MakeFileData &mfd, QTextStream& outStream) const
{
    // Since hardware and software filesets are different entities, both have to be looped through.
    // Include files are skipped and the object file is simply original filename + ".o".
    outStream << "_OBJ=";

    foreach(MakefileParser::MakeObjectData mod, mfd.swObjects)
    {
        if ( !mod.file->getIncludeFile() )
        {
            outStream << " " << mod.fileName << ".o";
        }
    }

    foreach(MakefileParser::MakeObjectData mod, mfd.hwObjects)
    {
        if ( !mod.file->getIncludeFile() )
        {
            outStream << " " << mod.file->getName() << ".o";
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
    //Rather straight forward: write constant build rule and a cleaner rule.
    outStream << "$(ENAME): $(OBJ)" << endl;
    outStream << "\t$(EBUILDER) -o bin_$(ENAME) $(OBJ) $(EFLAGS)"
        << endl << endl;

    outStream << "clean:\n\trm -f $(ODIR)/*" << endl;
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::writeMakeObjects()
//-----------------------------------------------------------------------------
void MakefileGenerator::writeMakeObjects(MakefileParser::MakeFileData &mfd, QTextStream& outStream, QList<MakefileParser::MakeObjectData>& objects, QString instancePath) const
{
    foreach(MakefileParser::MakeObjectData mod, objects)
    {
        // Skip the include files.
        if ( mod.file->getIncludeFile() )
        {
            continue;
        }

        // Find out the compiler.
        QString compiler = getFileCompiler(mod, mfd);

        // Flags will always include at least the includes.
        QString cFlags = "$(includes) ";

        // The other flags are not hard coded.
        cFlags += mod.fileBuildCmd->getFlags();
        cFlags += getFileFlags(mod, mfd);

        QString fileName = mod.fileName;

        // Write the rule for building the individual object file.
        outStream << endl;
        outStream << "$(ODIR)/" << fileName << ".o:" << endl;
        outStream << "\t" << compiler << " -c -o $(ODIR)/" << fileName << ".o " <<
            General::getRelativePath(instancePath,mod.path) << "/" << fileName << " " << cFlags << endl;
    }
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::getFileCompiler()
//-----------------------------------------------------------------------------
QString MakefileGenerator::getFileCompiler(MakefileParser::MakeObjectData &mod, MakefileParser::MakeFileData &mfd) const
{
    QString compiler;

    // This mesh does following:
    // 1. No file builder -> use fileSet builder
    // 2. No fileSet builder -> use builder of the software view of the software instance
    // 3. If nothing else, use the builder of the software view of the hardware instance
    if ( mod.fileBuildCmd->getCommand().isEmpty() )
    {
        if ( mod.fileSetBuildCmd == 0 || mod.fileSetBuildCmd->getCommand().isEmpty() )
        {
            if ( ( mod.swBuildCmd == 0 || mod.swBuildCmd->getCommand().isEmpty() ) && mfd.hwBuildCmd != 0 )
            {
                compiler = mfd.hwBuildCmd->getCommand();
            }
            else if ( mod.swBuildCmd != 0 )
            {
                compiler = mod.swBuildCmd->getCommand();
            }
        }
        else if ( mod.fileSetBuildCmd != 0 )
        {
            compiler = mod.fileSetBuildCmd->getCommand();
        }
    }
    else
    {
        compiler = mod.fileBuildCmd->getCommand();
    }

    return compiler;
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::getFileFlags()
//-----------------------------------------------------------------------------
QString MakefileGenerator::getFileFlags(MakefileParser::MakeObjectData &mod, MakefileParser::MakeFileData &mfd) const
{
    QString cFlags;

    // This mesh does following:
    // 1. If file does not override flags, may use fileSet flags
    // 2. If fileSet does not override flags, may use software flags
    // 2. If software does not override flags, may use hardware flags
    if ( !mod.fileBuildCmd->getReplaceDefaultFlags() )
    {
        if ( mod.fileSetBuildCmd != 0 )
        {
            cFlags += " " + mod.fileSetBuildCmd->getFlags();
        }

        if ( mod.fileSetBuildCmd == 0 || !mod.fileSetBuildCmd->getReplaceDefaultFlags() )
        {
            if ( mod.swBuildCmd != 0 )
            {
                cFlags += " " + mod.swBuildCmd->getFlags();
            }   

            if ( ( mod.swBuildCmd == 0 || !mod.swBuildCmd->getReplaceDefaultFlags() ) && mfd.hwBuildCmd != 0 )
            {
                cFlags += " " + mfd.hwBuildCmd->getFlags();
            }
        }
    }

    return cFlags;
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

        outStream << General::getRelativePath(basePath,directory) << "/bin_" << qfi.fileName() << " ";
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

