//-----------------------------------------------------------------------------
// File: MakefileGenerator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 02.09.2014
//
// Description:
// Makefile generator.
//-----------------------------------------------------------------------------

#ifndef MAKEFILEGENERATOR_H
#define MAKEFILEGENERATOR_H

#include "MakefileParser.h"

#include <QString>

#include <IPXACTmodels/design.h>
#include <IPXACTmodels/designconfiguration.h>
#include <IPXACTmodels/file.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/SWView.h>

#include <library/LibraryManager/libraryinterface.h>

class MakefileGenerator
{
public:
    MakefileGenerator( MakefileParser& parser );

    ~MakefileGenerator();

    /*!
     *  Generates all makefiles based on the parsed data.
     *
     *      @param [in] targetPath   The path, where the makefiles are created.
     *      @param [in] targetPath   The path, where the top component is.
     */
    void generate(QString targetPath, QString topPath) const;

private:

    /*!
     *  Creates a makefile for a single software instance, and thus for a single executable.
     *  The makefile is also placed to the instance headers.
     *
     *      @param [in] basePath    The path, where the makefiles are created.
     *      @param [in] mfd   The make data associated with the makefile as whole.
     *      @param [in] makeNames   The directory of the created makefile must be added here.
     */
    void generateInstanceMakefile(QString basePath, QString topPath, MakefileParser::MakeFileData &mfd, QStringList &makeNames) const;

    /*!
     *  Creates a makefile calling all the other makefiles associated with the design.
     *
     *      @param [in] basePath   The path, where the makefile is created.
     *      @param [in] makeNames   The paths of all other makefiles.
     */
    void generateMainMakefile(QString basePath, QString topPath, QStringList makeNames) const;

     /*!
      *  Creates a launcher script starting executables created by the makefiles.
      *
      *      @param [in] basePath   The path, where the launcher is created.
      *      @param [in] makeNames   The paths of all other makefiles.
      */
    void generateLauncher(QString basePath, QString topPath, QStringList makeNames) const;

    /*!
     *  Writes data used in building the executable to the stream.
     *
     *      @param [in] mfd   The make data associated with the makefile as whole.
     *      @param [in] outStream   The stream where the makefile is written.
     */
     void writeFinalFlagsAndBuilder(MakefileParser::MakeFileData &mfd, QTextStream& outStream) const;

    /*!
     *  Writes the list of the built and used objects to the stream.
     *
     *      @param [in] mfd   The make data associated with the makefile as whole.
     *      @param [in] outStream   The stream where the makefile is written.
     */
     void writeObjectList(MakefileParser::MakeFileData &mfd, QTextStream& outStream) const;

    /*!
     *  Writes the build command of the executable to the stream.
     *
     *      @param [in] outStream   The stream where the makefile is written.
     */
     void writeExeBuild(QTextStream& outStream) const;

    /*!
     *  Writes the build rules for the object files of the makefile.
     *
     *      @param [in] mfd  The make data associated with the makefile as whole.
     *      @param [in] outStream   The stream where the makefile is written.
     *      @param [in] objects   The list of the objects written.
     *      @param [in] instancePath   Path of the makefile and thus the path where it is called from.
     */
     void writeMakeObjects(MakefileParser::MakeFileData &mfd, QTextStream& outStream, QList<MakefileParser::MakeObjectData>& objects, QString instancePath) const;

    /*!
     *  Gets the compiler used for the file.
     *
     *      @param [in] mod   The make data associated with the object file.
     *      @param [in] mfd   The make data associated with the makefile as whole.
     */
     QString getFileCompiler(MakefileParser::MakeObjectData &mod, MakefileParser::MakeFileData &mfd) const;

    /*!
     *  Gets concatenates the build flags of the file with those fileSet and software views.
     *
     *      @param [in] mod   The make data associated with the object file.
     *      @param [in] mfd   The make data associated with the makefile as whole.
     */
     QString getFileFlags(MakefileParser::MakeObjectData &mod, MakefileParser::MakeFileData &mfd) const;

     /*!
      *  Writes list of launched executables.
      *
      *      @param [in] outStream   The stream where the launcher is written.
      *      @param [in] basePath   The path, where the launcher is created.
      *      @param [in] makeNames   The paths of all other makefiles.
      */
      void writeProcessList(QTextStream& outStream, QStringList makeNames, QString basePath) const;

     /*!
      *  Writes shell functions called from the script.
      *
      *      @param [in] outStream   The stream where the launcher is written.
      */
      void writeShellFunctions(QTextStream& outStream) const;

     /*!
      *  Writes a loop starting the processes.
      *
      *      @param [in] outStream   The stream where the launcher is written.
      */
      void writeProcessLaunch(QTextStream& outStream) const;

     /*!
      *  Write lines needed to terminate processes and wait call.
      *
      *      @param [in] outStream   The stream where the launcher is written.
      */
      void writeEnding(QTextStream& outStream) const;

      //! Collection of data sets, one for each makefile.
      QList<MakefileParser::MakeFileData> parsedData_;
      //! The fileSet for the main makefile and the launcher.
      QSharedPointer<FileSet> generalFileSet_;
};

#endif // MAKEFILEGENERATOR_H
