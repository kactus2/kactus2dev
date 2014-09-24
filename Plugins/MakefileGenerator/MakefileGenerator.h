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
     *  Generates all make files based on the parsed data.
     *
     *      @param [in] targetPath   The path, where the make files are created.
     */
     void generate(QString targetPath) const;

private:

    /*!
     *  Creates a makefile for a single software instance, and thus for a single executable.
     *
     *      @param [in] basePath    The path, where the make files are created.
     *      @param [in] mfd   The make data associated with the makefile as whole.
     *      @param [in] makeNames   The directory of the created makefile must be added here.
     */
     void generateInstanceMakefile(QString basePath, MakefileParser::MakeFileData &mfd, QStringList &makeNames) const;

    /*!
     *  Creates a makefile calling all the other makefiles associated with the design.
     *
     *      @param [in] basePath   The path, where the make file is created.
     *      @param [in] makeNames   The directories of all other makefiles.
     */
     void generateMainMakefile(QString basePath, QStringList makeNames) const;

    /*!
     *  Writes data used in building the executable to the stream.
     *
     *      @param [in] mfd   The make data associated with the makefile as whole.
     *      @param [in] outStream   The stream where the make file is written.
     */
     void writeFinalFlagsAndBuilder(MakefileParser::MakeFileData &mfd, QTextStream& outStream) const;

    /*!
     *  Writes the list of the built and used objects to the stream.
     *
     *      @param [in] mfd   The make data associated with the makefile as whole.
     *      @param [in] outStream   The stream where the make file is written.
     */
     void writeObjectList(MakefileParser::MakeFileData &mfd, QTextStream& outStream) const;

    /*!
     *  Writes the build command of the executable to the stream.
     *
     *      @param [in] outStream   The stream where the make file is written.
     */
     void writeExeBuild(QTextStream& outStream) const;

    /*!
     *  Writes the build rules for the object files of the make file.
     *
     *      @param [in] mfd  The make data associated with the makefile as whole.
     *      @param [in] outStream   The stream where the make file is written.
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

    //! Collection of data sets, one for each make file.
    QList<MakefileParser::MakeFileData> parsedData_;
};

#endif // MAKEFILEGENERATOR_H
