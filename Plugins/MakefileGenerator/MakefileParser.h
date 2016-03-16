//-----------------------------------------------------------------------------
// File: MakefileParser.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 22.09.2014
//
// Description:
// Parses design and its configuration for make file generation data.
//-----------------------------------------------------------------------------

#ifndef MakefileParser_H
#define MakefileParser_H

#include <QString>

#include <IPXACTmodels/Design/Design.h>

#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BuildCommand.h>

#include <IPXACTmodels/kactusExtensions/SWView.h>

#include <library/LibraryManager/libraryinterface.h>

class MakefileParser
{
public:
    // A struct containing needed data of a single file parsed to be used in a makefile.
	// Provided that it is not a header file, it likely yields an object file.
    struct MakeObjectData
    {
        // The file which data is parsed here.
        QSharedPointer<File> file;
        // The build command directly associated with the file.
        QSharedPointer<BuildCommand> fileBuildCmd;
        // The build command of the file set where file belongs to.
        QSharedPointer<FileBuilder> fileSetBuildCmd;
        // The build command of the active software view of the software instance.
        QSharedPointer<SWFileBuilder> swBuildCmd;
		// The resolved compiler for the object file.
		QString compiler;
		// The resolved flags for the object file.
		QString flags;
        // The absolute path of the file.
        QString path;
        // The name of the file.
        QString fileName;
		// The software instance where the file is included.
		QString instanceName;
		// File set where the file is included.
		QSharedPointer<FileSet> fileSet;
    };

    // A struct containing needed data of a single make file.
    struct MakeFileData
    {
        // Parsed files found in software views of software components.
        QList<QSharedPointer<MakeObjectData> > swObjects;
        // The build command of the active software view of the hardware instance.
        QSharedPointer<SWFileBuilder> hwBuildCmd;
        // Flags passed down from software views.
        QStringList softViewFlags;
        // The name of the instance.
        QString name;
        // The list of all included directories.
        QStringList includeDirectories;
        // The list of all include files.
        QList<QSharedPointer<MakeObjectData> > includeFiles;
        // The list of parsed software instances. Tracked to avoid re-parsing a dependency.
        QStringList parsedInstances;
        // Instance specific fileSet, which is to contain the makefile generated for the instance.
        QSharedPointer<FileSet> fileSet;
    };

    MakefileParser();

    ~MakefileParser();

    // Returns reference to all parsed MakeFileData.
    QList<MakeFileData>& getParsedData();

    // Return the general file set.
    const QSharedPointer<FileSet>& getGeneralFileSet();

    // Returns list of all files to be replaced.
    const QStringList& getReplacedFiles();

	// List of conflicts: Each set contains the parsed metadata of files
	// that are included multiple times.
	QVector<QSet<QSharedPointer<MakeObjectData> > > findConflicts() const;

    /*!
     *  Parses all software components from the design for the data needed in makefiles.
     *
     *      @param [in] library   The library containing all components in the design.
     *      @param [in] topComponent   The top component associated with the design.
     *      @param [in] desgConf   The design configuration object associated with the design.
	 *      @param [in] design   The design object.
	 *      @param [in] sysViewName  Name of the system view pointing to used design configuration.
	 *      @param [in] targetPath  Path where the generated files will be placed.
     */
    void parse( LibraryInterface* library, QSharedPointer<Component> topComponent,
        QSharedPointer<DesignConfiguration const> desgConf, QSharedPointer<const Design> design,
        QString sysViewName, QString targetPath = "" );

private:

    /*!
     *  Checks if softInstance is the topmost instance in its stack.
     *
     *      @param [in] design   The design object.
     *      @param [in] softInstance   The software instance to be checked.
     *      @param [in] softComponent   The software component of the instance.
     *
     *      @return True, if the softInstance is the topmost instance in its stack. Otherwise false.
     */
     bool isTopOfStack(QSharedPointer<const Design> design, QSharedPointer<SWInstance> softInstance,
         QSharedPointer<Component> softComponent);

    /*!
     *  Finds all files in the instance header fileSet if none exist, and add their directories as includes.
     *
     *      @param [in] library   The library containing all components in the design.
     *      @param [in] topComponent   The top component associated with the design.
     *      @param [in] desgConf   The design configuration object associated with the design.
     *      @param [in] sysViewName   The name of the system view associated with the design.
     *      @param [in] softInstance   The software instance which instance headers are to be found.
     *      @param [in] makeData   The make data associated with the makefile as whole.
     */
     void findInstanceHeaders(LibraryInterface* library, QSharedPointer<Component> topComponent,
         QSharedPointer<DesignConfiguration const> desgConf, QString sysViewName, QSharedPointer<SWInstance> softInstance,
         MakeFileData &makeData);

    /*!
     *  Finds active software view of the given hardware instance and parses its files.
     *
     *      @param [in] design   The design object.
     *      @param [in] hwName   Name of parsed hardware instance.
     *      @param [in] desgConf   The design configuration object associated with the design.
     *      @param [in] library   The library containing all components in the design.
     *      @param [in] makeData   The make data associated with the makefile as whole.
     *
     *      @return The active software view of the hardware instance.
     */
     QSharedPointer<SWView> parseHWMakeObjects(QSharedPointer<const Design> design, QString hwName,
         QSharedPointer<DesignConfiguration const> desgConf, LibraryInterface* library, MakeFileData &makeData);

    /*!
     *  Recursively parses source files for the whole stack fow the given software instance.
     *
     *      @param [in] softComponent   The component of softInstance.
     *      @param [in] softView   The active view of the softIntance.
     *      @param [in] softInstance   The software instance which active view is to be parsed.
     *      @param [in] design   The design object.
     *      @param [in] library   The library containing all components in the design.
     *      @param [in] desgConf   The design configuration object associated with the design.
     *      @param [in] makeData   The make data associated with the makefile as whole.
     */
     void parseStackObjects(QSharedPointer<Component> softComponent, QSharedPointer<SWView> softView,
         QSharedPointer<SWInstance> softInstance, QSharedPointer<const Design> design, LibraryInterface* library,
         QSharedPointer<DesignConfiguration const> desgConf, MakeFileData& makeData);

    /*!
     *  Parses all source files in all filesets in the given software view.
     *
     *      @param [in] library   The library containing all components in the design.
     *      @param [in] view   The software view which filesets are parsed.
     *      @param [in] component   The component of softInstance.
     *      @param [in] makeData   The make data associated with the makefile as whole.
     *      @param [in] objects   The collection of the parsed data of the files.
     *      @param [in] pickSWView   True, if flags corresponding file type are culled from the software view.
     */
      void parseMakeObjects(LibraryInterface* library, QSharedPointer<SWView> view, QString instanceName,
          QSharedPointer<Component> component, MakeFileData &makeData, QList<QSharedPointer<MakeObjectData> >& objects,
          bool pickSWView);

	/*!
	*  Parses all files of parameters fset, adding the to parameters objects.
	*  Parsing involves defining the file as header, and determining its relevant build commands and path.
	*
	*      @param [in] fset				The inspected file set
	*      @param [in] objects			The list of parsed objects, where the file data will be appended.
	*      @param [in] library			The library containing all components in the design.
	*      @param [in] component		The component of softInstance.
	*      @param [in] makeData			The make data associated with the makefile as whole.
	*      @param [in] pickSWView		True, if flags corresponding file type are culled from the software view.
	*      @param [in] view				The software view which filesets are parsed.
	*/
	  void parseFileSet(QSharedPointer<FileSet> fset, QString instanceName, QList<QSharedPointer<MakeObjectData> > &objects,
		  LibraryInterface* library, QSharedPointer<Component> component, MakeFileData &makeData, bool pickSWView,
		  QSharedPointer<SWView> view);

    /*!
     *  Tries to find a software instance by name and the component associated with it.
     *
     *      @param [in] library   The library containing all components in the design.
     *      @param [in] design   The design object.
     *      @param [in] instanceName   Name of the searched software instance.
     *      @param [in] targetInstance   The found software instance.
     *
     *      @return The component of the found instance.
     */
     QSharedPointer<Component> searchSWComponent(LibraryInterface* library, QSharedPointer<const Design> design,
         QString instanceName, QSharedPointer<SWInstance>& targetInstance);

    /*!
     *  Tries to find build commands corresponding parsed files in the makeData.
     *
     *      @param [in] makeData   The make data associated with the makefile as whole.
     *      @param [in] softView   Software view of the software component.
     *      @param [in] hardView   Software view of the hardware component.
     */
     void findHardwareBuildCommand(MakeFileData &makeData, QSharedPointer<SWView> softView, QSharedPointer<SWView> hardView);

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
     QString getFileFlags(QSharedPointer<Component> component, QSharedPointer<MakeObjectData> &mod, MakeFileData &mfd) const;

     //! Collection of data sets, one for each make file.
	 QList<MakeFileData> parsedData_;
     //! The fileSet for the main makefile and the launcher.
     QSharedPointer<FileSet> generalFileSet_;
     //! List of files that may be replaced
     QStringList replacedFiles_;
};

#endif // MakefileParser_H
