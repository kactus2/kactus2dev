//-----------------------------------------------------------------------------
// File: MakefilParser.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 22.09.2014
//
// Description:
// Makefile parser.
//-----------------------------------------------------------------------------

#ifndef MakefileParser_H
#define MakefileParser_H

#include <QString>

#include <IPXACTmodels/design.h>
#include <IPXACTmodels/designconfiguration.h>
#include <IPXACTmodels/file.h>
#include <IPXACTmodels/fileset.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/SWView.h>

#include <library/LibraryManager/libraryinterface.h>

class MakefileParser
{
public:

    // A struct containing needed data of a single file parsed to be used in a makefile.
    struct MakeObjectData
    {
        // The file which data is parsed here.
        QSharedPointer<File> file;
        // The build command directly associated with the file.
        BuildCommand* fileBuildCmd;
        // The build command of the file set where file belong to.
        QSharedPointer<FileBuilder> fileSetBuildCmd;
        // The build command of the active software view of the software instance.
        QSharedPointer<SWBuildCommand> swBuildCmd;
        // The absolute path of the file.
        QString path;
        // The name of the file.
        QString fileName;
    };

    // A struct containing needed data of a single make file.
    struct MakeFileData
    {
        // Parsed files found in software views of software components.
        QList<MakeObjectData> swObjects;
        // Parsed files found in software views of hardware components.
        QList<MakeObjectData> hwObjects;
        // The build command of the active software view of the hardware instance.
        QSharedPointer<SWBuildCommand> hwBuildCmd;
        // Flags passed down from software views.
        QStringList softViewFlags;
        // The name of the instance.
        QString name;
        // The list of all included directories.
        QStringList includeDirectories;
        // The list of all include files.
        QList<MakeObjectData> includeFiles;
        // The list of parsed software instances. Tracked to avoid re-parsing a dependency.
        QStringList parsedInstances;
        // Instance specific fileSet, which is to contain the makefile generated for the instance.
        QSharedPointer<FileSet> fileSet;
    };

    MakefileParser();

    ~MakefileParser();

    // Returns reference to all parsed MakeFileData.
    const QList<MakeFileData>& getParsedData();

    // Return the general file set.
    const QSharedPointer<FileSet>& getGeneralFileSet();

    // Returns list of all files to be replaced.
    const QStringList& getReplacedFiles();

    // Returns a list of all file names which may be generated but does exist

    /*!
     *  Parses all software components from the design for the data needed in makefiles.
     *
     *      @param [in] library   The library containing all components in the design.
     *      @param [in] topComponent   The top component associated with the design.
     *      @param [in] desgConf   The design configuration object associated with the design.
     *      @param [in] design   The design object.
     */
    void parse( LibraryInterface* library, QSharedPointer<Component> topComponent,
        QSharedPointer<DesignConfiguration const> desgConf, QSharedPointer<const Design> design,
        QString targetPath = "");

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
     bool isTopOfStack(QSharedPointer<const Design> design, SWInstance &softInstance,
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
         QSharedPointer<DesignConfiguration const> desgConf, QString sysViewName, SWInstance &softInstance,
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
         SWInstance &softInstance, QSharedPointer<const Design> design, LibraryInterface* library,
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
      void parseMakeObjects(LibraryInterface* library, QSharedPointer<SWView> view,
          QSharedPointer<Component> component, MakeFileData &makeData, QList<MakeObjectData>& objects,
          bool pickSWView);

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
         QString instanceName, SWInstance& targetInstance);

    /*!
     *  Tries to find build commands corresponding parsed files in the makeData.
     *
     *      @param [in] makeData   The make data associated with the makefile as whole.
     *      @param [in] softView   Software view of the software component.
     *      @param [in] hardView   Software view of the hardware component.
     */
     void findHardwareBuildCommand(MakeFileData &makeData, QSharedPointer<SWView> softView, QSharedPointer<SWView> hardView);

     //! Collection of data sets, one for each make file.
     QList<MakeFileData> parsedData_;
     //! The fileSet for the main makefile and the launcher.
     QSharedPointer<FileSet> generalFileSet_;
     //! List of files that may be replaced
     QStringList replacedFiles_;
};

#endif // MakefileParser_H
