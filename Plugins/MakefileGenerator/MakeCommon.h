//-----------------------------------------------------------------------------
// File: MakeCommon.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 19.05.2016
//
// Description:
// Contains the data types and constants commonly used in the makefile generation.
//-----------------------------------------------------------------------------

#ifndef MAKECOMMON_H
#define MAKECOMMON_H

#include <QString>
#include <QSharedPointer>

#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BuildCommand.h>
#include <IPXACTmodels/Component/View.h>

namespace MakeCommon
{
    // The names of a generated makefiles.
    const QString MAKEFILE_MASTER_NAME = "Makefile";
    const QString MAKEFILE_FRAGMENT_SUFFIX = ".mak";

    // Flags appended by default to each object file build rule. Currently, the effects are:
    // 1. To make use of the automatic dependency analysis.
    // 2. To create an object file with specific name rather than executable with the default name.
    const QString DEFAULT_OBJECT_FLAGS = "-MMD -MP -c -o";

    // A struct containing parsed data of software instance in a stack.
    struct StackPart
    {
        // The name of the associated component instance.
        QString instanceName;
        // The instantiated component.
        QSharedPointer<Component> component;
        // The active view in the current design configuration.
        QSharedPointer<View> view;
        // The component instantiation associated with the view.
        QSharedPointer<ComponentInstantiation> instantiation;
        // The selected build command of the active software view.
        QSharedPointer<FileBuilder> buildCmd;
        // Header files associated with the component instance.
        QSharedPointer<FileSet> instanceHeaders;
    };

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
        QSharedPointer<FileBuilder> swBuildCmd;
        // The resolved compiler for the object file.
        QString compiler;
        // The resolved flags for the object file.
        QString flags;
        // The absolute path of the file.
        QString path;
        // The name of the file.
        QString fileName;
        // File set where the file is included.
        QSharedPointer<FileSet> fileSet;
        // The part of the stack where this is based to.
        QSharedPointer<StackPart> stackPart;
        // True, if this object will be generated to the makefile.
        bool isChosen = true;
    };

    // A struct containing needed data of a single makefile.
    struct MakeFileData
    {
        // The parsed software stack.
        QList<QSharedPointer<StackPart> > parts;

        // The hardware instance, where the top component of the stack is mapped to.
        QSharedPointer<StackPart> hardPart;

        // Parsed files found in software views of software components.
        QList<QSharedPointer<MakeObjectData> > swObjects;
        // Flags passed down from component instantiations.
        QStringList componentInstantiationFlags;
        // The name of the instance.
        QString name;
        // The list of all included directories.
        QStringList includeDirectories;
        // The list of parsed software instances. Tracked to avoid re-parsing a dependency.
        QList<QSharedPointer<ComponentInstance> > parsedInstances;
        // Header files associated with the topmost component instance.
        // This is to contain the makefile generated for the instance.
        QSharedPointer<FileSet> instanceFileSet;
        // The path where the makefile is generated to.
        QString makeName;
        // Sets of conflicting parts.
        QVector<QSet<QSharedPointer<MakeObjectData> > > conflicts;
    };

}
#endif // MAKECOMMON_H