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
        // The build command of the active software view of the software instance.
        QSharedPointer<SWBuildCommand> swBuildCmd;
        // The build command of the active software view of the hardware instance.
        QSharedPointer<SWBuildCommand> hwBuildCmd;
        // The name of the instance.
        QString name;
        // The list of all included directories.
        QStringList includeDirectories;
        // The list of parsed software instances. Tracked to avoid re-parsing a dependency.
        QStringList parsedInstances;
    };

    MakefileParser();

    ~MakefileParser();

    // Returns reference to all parsed MakeFileData.
    const QList<MakeFileData>& getParsedData();

    /*!
     *  Parses all software components from the design for the data needed in makefiles.
     *
     *      @param [in] library   The library containing all components in the design.
     *      @param [in] topComponent   The top component associated with the design.
     *      @param [in] desgConf   The design configuration object associated with the design.
     *      @param [in] design   The design object.
     */
     void parse( LibraryInterface* library, QSharedPointer<Component> topComponent,
        QSharedPointer<DesignConfiguration const> desgConf, QSharedPointer<const Design> design);

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
     *  Finds all files in the instance header fileset if none exist, and add their directories as includes.
     *
     *      @param [in] library   The library containing all components in the design.
     *      @param [in] topComponent   The top component associated with the design.
     *      @param [in] softInstance   The software instance which instance headers are to be found.
     *      @param [in] makeData   The make data associated with the makefile as whole.
     */
     void findInstanceHeaders(LibraryInterface* library, QSharedPointer<Component> topComponent,
         SWInstance &softInstance, MakeFileData &makeData);

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
     */
     void parseMakeObjects(LibraryInterface* library, QSharedPointer<SWView> view,
         QSharedPointer<Component> component, MakeFileData &makeData, QList<MakeObjectData>& objects);

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
     void findBuildCommands(MakeFileData &makeData, QSharedPointer<SWView> softView, QSharedPointer<SWView> hardView);

    //! Collection of data sets, one for each make file.
    QList<MakeFileData> parsedData_;
};

#endif // MakefileParser_H
