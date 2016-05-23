//-----------------------------------------------------------------------------
// File: MakefileGenerator.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 02.09.2014
//
// Description:
// Makefile generator.
//-----------------------------------------------------------------------------

#ifndef MAKEFILEGENERATOR_H
#define MAKEFILEGENERATOR_H

#include "MakefileParser.h"
#include "MakeCommon.h"

#include <Plugins/PluginSystem/IPluginUtility.h>

class MakefileGenerator
{
public:
    /*!
     *  The constructor.
     *
	 *      @param [in] generalFileSet  The file set, where design configuration specific things belong to.
	*/
	MakefileGenerator( MakefileParser& parser, IPluginUtility* utility, QSharedPointer<FileSet> generalFileSet );

    ~MakefileGenerator();

    /*!
     *  Generates all makefiles based on the parsed data.
     *
     *      @param [in] targetPath		The path, where the makefiles are created.
	 *      @param [in] topPath			The path, where the top component is.
	 *      @param [in] sysViewName		Name of the system view pointing to used design configuration.
	 *
	 *      @return How many executables got a makefile.
	 */
	int generate(QString targetPath, QString topPath, QString sysViewName);

private:

    /*!
     *  Creates a makefile for a single software instance, and thus for a single executable.
     *  The makefile is also placed to the instance headers.
     *
     *      @param [in] basePath    The path, where the makefiles are created.
     *      @param [in] mfd   The make data associated with the makefile as whole.
     *      @param [in] makeNames   The directory of the created makefile must be added here.
     */
    void generateInstanceMakefile(QString topPath,
		QSharedPointer<MakeFileData> makeData, QStringList &makeNames);

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
     void writeFinalFlagsAndBuilder(QSharedPointer<MakeFileData> mfd, QTextStream& outStream) const;

    /*!
     *  Writes the list of the built and used objects to the stream.
     *
     *      @param [in] mfd   The make data associated with the makefile as whole.
     *      @param [in] outStream   The stream where the makefile is written.
     */
     void writeObjectList(QSharedPointer<MakeFileData> mfd, QTextStream& outStream) const;

    /*!
     *  Writes the build command of the executable to the stream.
     *
     *      @param [in] outStream   The stream where the makefile is written.
     */
     void writeExeBuild(QTextStream& outStream) const;
	 
    /*!
     *  Writes the cleaning rules for makefile, which removes both object and dependency files.
     *
     *      @param [in] outStream   The stream where the makefile is written.
     */
	 void writeCleanRules(QTextStream& outStream) const;

    /*!
     *  Writes the build rules for the object files of the makefile.
     *
     *      @param [in] mfd  The make data associated with the makefile as whole.
     *      @param [in] outStream   The stream where the makefile is written.
     *      @param [in] objects   The list of the objects written.
     *      @param [in] instancePath   Path of the makefile and thus the path where it is called from.
     */
     void writeMakeObjects(QTextStream& outStream,
		 QList<QSharedPointer<MakeObjectData> >& objects, QString instancePath) const;

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
      QSharedPointer<QList<QSharedPointer<MakeFileData> > > parsedData_;
      //! The fileSet for the main makefile and the launcher.
      QSharedPointer<FileSet> generalFileSet_;
      //! The utility used to print message and etc.
      IPluginUtility* utility_;
};

#endif // MAKEFILEGENERATOR_H
