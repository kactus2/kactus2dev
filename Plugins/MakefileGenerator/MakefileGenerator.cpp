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

    writeMakeObjects(mfd, outStream, mfd.swObjects, mfd.swBuildCmd, instancePath);
    writeMakeObjects(mfd, outStream, mfd.hwObjects, QSharedPointer<SWBuildCommand>(), instancePath);

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
// Function: MakefileGenerator::writeFinalFlagsAndBuilder()
//-----------------------------------------------------------------------------
void MakefileGenerator::writeFinalFlagsAndBuilder(MakefileParser::MakeFileData &mfd, QTextStream& outStream) const
{
    QString finalFlags = "$(includes)";
    QString finalBuilder;
    bool allowHWFlags = true;

    // If build command of software view of the software instance exist, its properties are used.
    if ( mfd.swBuildCmd != 0 )
    {
        finalBuilder = mfd.swBuildCmd->getCommand();

        allowHWFlags = !mfd.swBuildCmd->getReplaceDefaultFlags();

        finalFlags += " " + mfd.swBuildCmd->getFlags();
    }

    // If build command of software view of the hardware instance exist, its properties are used.
    if ( mfd.hwBuildCmd != 0 )
    {
        // Use the hardware builder only if no software builder exist. Should never happen, though.
        if ( finalBuilder.isEmpty() )
        {
            finalBuilder = mfd.hwBuildCmd->getCommand();
        }

        // If software command disallows use of hardware flags, respect it.
        if ( allowHWFlags )
        {
            finalFlags += " " + mfd.hwBuildCmd->getFlags();
        }
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
    outStream << "\t$(EBUILDER) bin_$(ENAME) $(OBJ) $(EFLAGS)"
        << endl << endl;

    outStream << "clean:\n\trm -f $(ODIR)/*" << endl;
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::writeMakeObjects()
//-----------------------------------------------------------------------------
void MakefileGenerator::writeMakeObjects(MakefileParser::MakeFileData &mfd, QTextStream& outStream, QList<MakefileParser::MakeObjectData>& objects, QSharedPointer<SWBuildCommand>swBuildCmd, QString instancePath) const
{
    foreach(MakefileParser::MakeObjectData mod, objects)
    {
        // Skip the include files.
        if ( mod.file->getIncludeFile() )
        {
            continue;
        }

        // Find out the compiler.
        QString compiler = getFileCompiler(mod, swBuildCmd, mfd);

        // Flags will always include at least the includes.
        QString cFlags = "$(includes) ";

        // The other flags are not hard coded.
        cFlags += mod.fileBuildCmd->getFlags();
        cFlags += getFileFlags(mod, swBuildCmd, mfd);

        QString fileName = mod.fileName;

        // Write the rule for building the individual object file.
        outStream << endl;
        outStream << "$(ODIR)/" << fileName << ".o:" << endl;
        outStream << "\t" << compiler << " $(ODIR)/" << fileName << ".o " <<
            General::getRelativePath(instancePath,mod.path) << "/" << fileName << " " << cFlags << endl;
    }
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::getFileCompiler()
//-----------------------------------------------------------------------------
QString MakefileGenerator::getFileCompiler(MakefileParser::MakeObjectData &mod, QSharedPointer<SWBuildCommand>
    swBuildCmd, MakefileParser::MakeFileData &mfd) const
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
            if ( ( swBuildCmd == 0 || swBuildCmd->getCommand().isEmpty() ) && mfd.hwBuildCmd != 0 )
            {
                compiler = mfd.hwBuildCmd->getCommand();
            }
            else if ( swBuildCmd != 0 )
            {
                compiler = swBuildCmd->getCommand();
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
QString MakefileGenerator::getFileFlags(MakefileParser::MakeObjectData &mod, QSharedPointer<SWBuildCommand>
    swBuildCmd, MakefileParser::MakeFileData &mfd) const
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
            if ( swBuildCmd != 0 )
            {
                cFlags += " " + swBuildCmd->getFlags();
            }   

            if ( ( swBuildCmd == 0 || !swBuildCmd->getReplaceDefaultFlags() ) && mfd.hwBuildCmd != 0 )
            {
                cFlags += " " + mfd.hwBuildCmd->getFlags();
            }
        }
    }

    return cFlags;
}

