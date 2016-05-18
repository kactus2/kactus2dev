//-----------------------------------------------------------------------------
// File: MakefileParser.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
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
#include "Struct.h"

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

	// List of conflicts: Each set contains the parsed metadata of files
	// that are included multiple times.
	QVector<QSet<QSharedPointer<MakeObjectData> > > findConflicts() const;

    /*!
     *  Parses all software components from the design for the data needed in makefiles.
     */
    void parse();

private:

    /*!
     *  Parses all source files in all filesets in the given software view.
     *
     *      @param [in] view   The software view which filesets are parsed.
     *      @param [in] component   The component of softInstance.
     *      @param [in] makeData   The make data associated with the makefile as whole.
     *      @param [in] objects   The collection of the parsed data of the files.
     *      @param [in] pickSWView   True, if flags corresponding file type are culled from the software view.
     */
      void parseMakeObjects(QSharedPointer<SWView> view, QSharedPointer<Component> component,
		  QSharedPointer<MakeFileData> makeData, QList<QSharedPointer<MakeObjectData> >& objects,
		  bool pickSWView);

	/*!
	*  Parses all files of parameters fset, adding the to parameters objects.
	*  Parsing involves defining the file as header, and determining its relevant build commands and path.
	*
	*      @param [in] fset				The inspected file set
	*      @param [in] objects			The list of parsed objects, where the file data will be appended.
	*      @param [in] makeData			The make data associated with the makefile as whole.
	*      @param [in] pickSWView		True, if flags corresponding file type are culled from the software view.
	*      @param [in] view				The software view which file sets are parsed.
	*/
	  void parseFileSet(QSharedPointer<FileSet> fset, QList<QSharedPointer<MakeObjectData> > &objects,
		  QSharedPointer<MakeFileData> makeData, bool pickSWView, QSharedPointer<SWView> view);

    /*!
     *  Gets the compiler used for the file.
     *
     *      @param [in] mod   The make data associated with the object file.
	 *      @param [in] hardView   Software view of the associated hardware component.
	 *
	 *      @return The resolved compiler for the file.
	 */
	 QString getFileCompiler(QSharedPointer<MakeObjectData> mod, QSharedPointer<SWView> hardView) const;

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

	 //! The library containing all components in the design.
	 LibraryInterface* library_;
	 SWStackParser& swStackParser_;
     //! Collection of data sets, one for each makefile.
	 QSharedPointer<QList<QSharedPointer<MakeFileData> > > parsedData_;
};

#endif // MakefileParser_H
