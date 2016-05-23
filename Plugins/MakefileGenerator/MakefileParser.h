//-----------------------------------------------------------------------------
// File: MakefileParser.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 22.09.2014
//
// Description:
// Parses makefile data from SW stack.
//-----------------------------------------------------------------------------

#ifndef MakefileParser_H
#define MakefileParser_H

#include <QString>

#include "SWStackParser.h"
#include "MakeCommon.h"

#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BuildCommand.h>

#include <IPXACTmodels/kactusExtensions/SWView.h>

#include <library/LibraryManager/libraryinterface.h>

class MakefileParser
{
public:
    /*!
     *  The constructor.
     *
	 *      @param [in] library   The library containing all components in the design.
	 *      @param [in] parser   The parsed software stack containing needed references.
	 */
    MakefileParser( LibraryInterface* library, SWStackParser& swStackParser );

    ~MakefileParser();

    // Returns reference to all parsed MakeFileData.
    QSharedPointer<QList<QSharedPointer<MakeFileData> > > getParsedData();

    /*!
     *  Parses all software components from the design for the data needed in makefiles.
     */
    void parse(QSharedPointer<Component> topComponent);

private:

    /*!
     *  Parses all source files in all file sets in the given part of a stack.
	 *
	 *      @param [in] makeData   The make data associated with the makefile as whole.
	 *      @param [in] view       The software view, where the files belong to.
     *      @param [in] buildCmd   The build command of the associated software view.
	 *      @param [in] component  The component of softInstance.
     */
	void parseMakeObjects( QSharedPointer<MakeFileData> makeData, 
		QSharedPointer<StackPart> stackPart );

	/*!
	 *  Parses all files of parameters fset, adding the to parameters objects.
	 *  Parsing involves defining the file as header, and determining its relevant build commands and path.
	 *
	 *      @param [in] fset				The inspected file set
	 *      @param [in] objects			The list of parsed objects, where the file data will be appended.
	 *      @param [in] makeData			The make data associated with the makefile as whole.
	 *      @param [in] buildCmd			The build command 
	 *      @param [in] componentPath	Path to the component where the file sets belong to.
	 */
	 void parseFileSet(QSharedPointer<FileSet> fset, QSharedPointer<MakeFileData> makeData, 
	 QSharedPointer<StackPart> stackPart, QString& componentPath);

    /*!
     *  Gets the compiler used for the file.
     *
     *      @param [in] mod   The make data associated with the object file.
	 *      @param [in] hardView   Software view of the associated hardware component.
	 *
	 *      @return The resolved compiler for the file.
	 */
	 QString getFileCompiler(QSharedPointer<MakeObjectData> mod, QSharedPointer<SWFileBuilder> hardBuilder) const;

    /*!
     *  Gets concatenated the build flags of the file with those fileSet and software views.
     *
     *      @param [in] mod   The make data associated with the object file.
	 *      @param [in] mfd   The make data associated with the makefile as whole.
	 *
	 *      @return The resolved flags for the file.
     */
     QString getFileFlags(QSharedPointer<Component> component, QSharedPointer<MakeObjectData> &mod,
		 QSharedPointer<MakeFileData> makeData) const;
	 
    /*!
	 *  Finds conflicting files within the stack: The same file with different meta data.
     *
     *      @param [in] makeData			The make data associated with the makefile as whole.
     */
	 void findConflicts(QSharedPointer<MakeFileData> makeData);

	 //! The library containing all components in the design.
	 LibraryInterface* library_;
	 SWStackParser& swStackParser_;
     //! Collection of data sets, one for each makefile.
	 QSharedPointer<QList<QSharedPointer<MakeFileData> > > parsedData_;
};

#endif // MakefileParser_H
