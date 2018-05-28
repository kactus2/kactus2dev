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
     *      @param [in] parser          The parser which parsed the meta data to data structures directly usable by the generator.
     *      @param [in] utility         The provided plugin utility.
     *      @param [in] generalFileSet  The file set, where design configuration specific things belong to.
     */
    MakefileGenerator( MakefileParser& parser, IPluginUtility* utility, 
        QSharedPointer<FileSet> generalFileSet );
    
    //! Disable copying.
    MakefileGenerator(MakefileGenerator const& other) = delete;
    MakefileGenerator& operator=(MakefileGenerator const& other) = delete;

    /*!
     *  The destructor.
     */
    ~MakefileGenerator() = default;

    /*!
     *  Generates all makefiles based on the parsed data.
     *
     *      @param [in] targetPath		The path, where the makefiles are created.
	 *      @param [in] topPath			The path, where the top component is.
	 *      @param [in] sysViewName		Name of the system view pointing to used design configuration.
	 *
	 *      @return How many executables got a makefile.
	 */
	int generate(QString const& targetPath, QString const& topPath);


private:

    /*!
     *  Creates a makefile for a single software instance, and thus for a single executable.
     *  The makefile is also placed to the instance headers. Will add the name to makeNames.
     *
     *      @param [in] targetPath  The path, where the makefiles are created.
     *      @param [in] componentPath     The path of the component which will be associated with the created make file.
     *      @param [in] makeData    The make data associated with the makefile as whole.
     *      @param [in/out] makeNames   The directory of the created makefile must be added here.
     */
    void generateInstanceMakefile(QString const& targetPath, QString const& componentPath,
		QSharedPointer<MakeFileData> makeData, QStringList& makeNames);

    /*!
     *  Creates a makefile calling all the other makefiles associated with the design.
     *  The makefile is also placed to the instance headers.
     *
     *      @param [in] basePath        The path, where the makefile is created.
     *      @param [in] componentPath   The path of the component which will be associated with the created make file.
     *      @param [in] makeNames   The names of all created make files so far.
     */
    void generateMainMakefile(QString const& targetPath, QString const& componentPath, QStringList& makeNames) const;

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
     *      @param [in] outStream   The stream where the makefile is written.
     *      @param [in] objects   The list of the objects written.
     *      @param [in] instancePath   Path of the makefile and thus the path where it is called from.
     */
     void writeMakeObjects(QTextStream& outStream,
		 QList<QSharedPointer<MakeObjectData> >& objects, QString instancePath) const;

     //-----------------------------------------------------------------------------
     // Data.
     //-----------------------------------------------------------------------------

      //! Collection of data sets, one for each makefile.
      QSharedPointer<QList<QSharedPointer<MakeFileData> > > parsedData_;

      //! The utility used to print message and etc.
      IPluginUtility* utility_;

      //! The fileSet for the main makefile and the launcher.
      QSharedPointer<FileSet> generalFileSet_;

      QString mainMakeName_;

};

#endif // MAKEFILEGENERATOR_H
