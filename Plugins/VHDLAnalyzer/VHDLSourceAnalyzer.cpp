//-----------------------------------------------------------------------------
// File: VHDLSourceAnalyzer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Honkonen, Joni-Matti M‰‰tt‰
// Date: 18.1.2013
//
// Description:
// C/C++ source analyzer plugin.
//-----------------------------------------------------------------------------

#include "VHDLSourceAnalyzer.h"
#include <models/fileset.h>
#include <models/file.h>
#include <models/generaldeclarations.h>

#include <QtPlugin>
#include <QMessageBox>
#include <QFileInfo>
#include <QSettings>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QTextStream>
#include <QFileInfo>
#include <QDir>

#include <PluginSystem/IPluginUtility.h>

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::VHDLSourceAnalyzer()
//-----------------------------------------------------------------------------
VHDLSourceAnalyzer::VHDLSourceAnalyzer()
    : fileTypes_()
{
    fileTypes_.append("vhdlSource");
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::~VHDLSourceAnalyzer()
//-----------------------------------------------------------------------------
VHDLSourceAnalyzer::~VHDLSourceAnalyzer()
{
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::getName()
//----------------------------------------------------------------------------
QString const& VHDLSourceAnalyzer::getName() const
{
    static QString name("VHDL Source Analyzer");
    return name;
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::getVersion()
//-----------------------------------------------------------------------------
QString const& VHDLSourceAnalyzer::getVersion() const
{
    static QString version("1.0");
    return version;
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::getDescription()
//-----------------------------------------------------------------------------
QString const& VHDLSourceAnalyzer::getDescription() const
{
    static QString desc("Analyzes file dependencies from VHDL files.");
    return desc;
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::getSettingsWidget()
//-----------------------------------------------------------------------------
PluginSettingsWidget* VHDLSourceAnalyzer::getSettingsWidget()
{
    return new PluginSettingsWidget();
}


//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::getSupportedFileTypes()
//-----------------------------------------------------------------------------
QStringList const& VHDLSourceAnalyzer::getSupportedFileTypes() const
{
    return fileTypes_;
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::calculateHash()
//-----------------------------------------------------------------------------
QString VHDLSourceAnalyzer::calculateHash(QString const& filename)
{
    // Try to open the file
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text) )
    {
        // File could not be opened, show error
        return 0;
    }

    QString source = getSourceData(file);

    // Calculate the hash
    QCryptographicHash hash(QCryptographicHash::Sha1);
    hash.addData(source.toLatin1());

    QString result = hash.result().toHex();
    return result;
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::getFileDependencies()
//-----------------------------------------------------------------------------
void VHDLSourceAnalyzer::getFileDependencies(Component const* component,
                                             QString const& componentPath,
                                             QString const& filename,
                                             QList<FileDependencyDesc>& dependencies)
{
    // Try to open the file.
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text) )
    {
        return;
    }

    // Read file contents into a buffer.
    QString source = getSourceData(file);

    scanEntityReferences(source, filename, dependencies);
    scanPackageReferences(source, filename, dependencies);

}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::getSourceData()
//-----------------------------------------------------------------------------
QString VHDLSourceAnalyzer::getSourceData(QFile& file)
{
    // Read the file data
    QString contents = file.readAll();
    contents.replace(QRegExp("--[^\\n]*"), "");

    return contents.simplified();
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::beginAnalysis()
//-----------------------------------------------------------------------------
void VHDLSourceAnalyzer::beginAnalysis(Component const* component, QString const& componentPath)
{
    scanDefinitions(component, componentPath);
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::endAnalysis()
//-----------------------------------------------------------------------------
void VHDLSourceAnalyzer::endAnalysis(Component const* component, QString const& componentPath)
{
    cachedEntities_.clear();
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::scanEntities()
//-----------------------------------------------------------------------------
void VHDLSourceAnalyzer::scanDefinitions(Component const* component, QString const& componentPath)
{
    // Get all the file sets from the component.
    QList< QSharedPointer<FileSet> > fileSets = component->getFileSets();
    
    // Scan all the file sets.
    for (int j = 0; j < fileSets.size(); ++j)
    {
        QList< QSharedPointer<File> > files = fileSets.at(j)->getFiles();

        // Go through all the files in the file set.
        for (int i = 0; i < files.size(); ++i)
        {
            // Skip the file if it's not a VHDL source file.
            if (!files.at(i)->getAllFileTypes().contains("vhdlSource"))
            {
                continue;
            }

            // Try to open the file.
            QString filename = General::getAbsolutePath(componentPath, files.at(i)->getName());
            QFile file(filename);

            if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                // File could not be opened, skip
                continue;
            }

            QString source = getSourceData(file);

            scanEntities(source, filename);
            scanPackages(source, filename);
        }
    }
}

QList<IPlugin::ExternalProgramRequirement> VHDLSourceAnalyzer::getProgramRequirements() {
	return QList<IPlugin::ExternalProgramRequirement>();
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::scanEntities()
//-----------------------------------------------------------------------------
void VHDLSourceAnalyzer::scanEntities(QString const& source, QString const& filename)
{
    // Look for entities.
    static QRegExp entityBeginExp("\\bENTITY\\s+(\\w+)\\s+(?:IS)\\s*", Qt::CaseInsensitive);
    int startIndex = entityBeginExp.indexIn(source);
    int endIndex = 0;

    while (startIndex != -1)
    {
        // Register the entity name.
        QString entityName = entityBeginExp.cap(1).toLower();
        cachedEntities_[entityName].append(filename);

        // Skip down to the entity end and look for the next entity declaration.
        QRegExp entityEndExp("\\bEND\\w+(?:ENTITY\\w+)" + entityName + "\\b", Qt::CaseInsensitive);
        endIndex = entityEndExp.indexIn(source, startIndex + entityBeginExp.matchedLength());

        if (endIndex == -1)
        {
            break;
        }

        endIndex += entityEndExp.matchedLength();
        startIndex = entityBeginExp.indexIn(source, endIndex);
    }
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::scanPackages()
//-----------------------------------------------------------------------------
void VHDLSourceAnalyzer::scanPackages(QString const& source, QString const& filename)
{
    // Look for packages.
    static QRegExp packageBeginExp("\\bPACKAGE\\s+(\\w+)\\s+IS\\s*", Qt::CaseInsensitive);
    int startIndex = packageBeginExp.indexIn(source);
    int endIndex = 0;

    while (startIndex != -1)
    {
        // Register the package name.
        QString packageName = packageBeginExp.cap(1).toLower();
        cachedPackages_[packageName].append(filename);

        // Skip down to the package end and look for the next package declaration.
        QRegExp packageEndExp("\\bEND\\w+(?:PACKAGE\\w+)" + packageName + "\\b", Qt::CaseInsensitive);
        endIndex = packageEndExp.indexIn(source, startIndex + packageBeginExp.matchedLength());

        if (endIndex == -1)
        {
            break;
        }

        endIndex += packageEndExp.matchedLength();
        startIndex = packageBeginExp.indexIn(source, endIndex);
    }
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::scanEntityReferences()
//-----------------------------------------------------------------------------
void VHDLSourceAnalyzer::scanEntityReferences(QString const& source, QString const& filename,
                                              QList<FileDependencyDesc>& dependencies)
{
    // Look for architecture declarations.
    static QRegExp archBeginExp("\\bARCHITECTURE\\s+(\\w+)\\s+OF\\s+(\\w+)\\s+IS\\b", Qt::CaseInsensitive);
    static QRegExp beginExp("\\bBEGIN\\b", Qt::CaseInsensitive);
    static QRegExp entityExp("\\b(\\w+)\\s+\\:\\s+(ENTITY\\s+)?(\\w+\\.)?(\\w+)\\b", Qt::CaseInsensitive);
    static QRegExp funcStartExp("\\bFUNCTION\\s+(\\w+)\\b", Qt::CaseInsensitive);

    int archStart = 0;
    int archEnd = 0;
    int index = 0;

    while (index >= 0 && index < source.length())
    {
        archStart = archBeginExp.indexIn(source, index);

        if (archStart == -1)
        {
            break;
        }

        QRegExp archEndExp("\\bEND\\s+(ARCHITECTURE\\s+)?" + archBeginExp.cap(1) + "\\b", Qt::CaseInsensitive);
        archEnd = archEndExp.indexIn(source, archStart + archBeginExp.matchedLength());

        // Skip functions that are inside this architecture declaration.
        int endFuncIndex = archStart + archBeginExp.matchedLength();
        int startFuncIndex = funcStartExp.indexIn(source, endFuncIndex);

        while (startFuncIndex != -1 && startFuncIndex < archEnd)
        {
            // Skip the whole function and look for a next one.
            QRegExp funcEndExp("\\bEND\\s+(FUNCTION\\s+)?" + funcStartExp.cap(1) + "\\b", Qt::CaseInsensitive);
            endFuncIndex = funcEndExp.indexIn(source, startFuncIndex + funcStartExp.matchedLength());

            if (endFuncIndex == -1)
            {
                endFuncIndex = archEnd;
                break;
            }

            endFuncIndex += funcEndExp.matchedLength();
            startFuncIndex = funcStartExp.indexIn(source, endFuncIndex);
        }

        // Look for the begin word. It must be before the architecture end.
        int beginIndex = beginExp.indexIn(source, endFuncIndex);

        if (beginIndex >= 0 && (archEnd == -1 || beginIndex < archEnd))
        {
            // Look for component instantiations.
            int entityIndex = beginIndex + beginExp.matchedLength();

            while (archEnd == -1 || entityIndex <= archEnd)
            {
                entityIndex = entityExp.indexIn(source, entityIndex);

                if (entityIndex == -1)
                {
                    break;
                }

                entityIndex += entityExp.matchedLength();

                QString tokenName = entityExp.cap(4).toLower();

                if (tokenName == "process")
                {
                    QRegExp processEndExp("\\bEND\\s+PROCESS\\s+" + entityExp.cap(1) + "\\b", Qt::CaseInsensitive);
                    entityIndex = processEndExp.indexIn(source, entityIndex);

                    if (entityIndex == -1)
                    {
                        break;
                    }

                    entityIndex += processEndExp.matchedLength();
                }
                else if (tokenName != "if" && tokenName != "for")
                {
                    addEntityDependency(entityExp.cap(4), filename, dependencies);
                }
            }
        }

        if (archEnd == -1)
        {
            break;
        }

        index = archEnd + archEndExp.matchedLength();
    }
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::scanPackageReferences()
//-----------------------------------------------------------------------------
void VHDLSourceAnalyzer::scanPackageReferences(QString const& source, QString const& filename,
                                               QList<FileDependencyDesc>& dependencies)
{
    static QRegExp packageRefExp("\\bUSE\\s+(\\w+)\\.(\\w+)\\.(\\w+)\\b", Qt::CaseInsensitive);
    int index = packageRefExp.indexIn(source);
    
    while (index != -1)
    {
        // Discard anything in the IEEE library.
        QString libraryName = packageRefExp.cap(1).toLower();

        if (libraryName != "ieee" && libraryName != "std")
        {
            QString packageName = packageRefExp.cap(2);
            addPackageDependency(packageName, filename, dependencies);
        }

        index = packageRefExp.indexIn(source, index + packageRefExp.matchedLength());
    }
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::addDependency()
//-----------------------------------------------------------------------------
void VHDLSourceAnalyzer::addEntityDependency(QString const& componentName, QString const& filename,
                                             QList<FileDependencyDesc>& dependencies)
{
    FileDependencyDesc dependency;
    dependency.description = "Component instantiation for entity " + componentName;

    QString lowCase = componentName.toLower();

    if (cachedEntities_.contains(lowCase))
    {
        // Add all existing entities to the return value list.
        for (int j = 0; j < cachedEntities_[lowCase].count(); ++j)
        {
            dependency.filename = General::getRelativePath(filename, cachedEntities_[lowCase].at(j));
        }
    }
    else
    {
        dependency.filename = componentName + ".vhd";
    }

    // Discard if this is a duplicate.
    for (int i = 0; i < dependencies.size(); ++i)
    {
        if (dependencies[i].filename == dependency.filename)
        {
            return;
        }
    }

    dependencies.append(dependency);
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::addPackageDependency()
//-----------------------------------------------------------------------------
void VHDLSourceAnalyzer::addPackageDependency(QString const& packageName, QString const& filename,
                                              QList<FileDependencyDesc> &dependencies)
{
    FileDependencyDesc dependency;
    dependency.description = "Reference to a package " + packageName;

    QString lowCase = packageName.toLower();

    if (cachedPackages_.contains(lowCase))
    {
        // Add all existing entities to the return value list.
        for (int j = 0; j < cachedPackages_[lowCase].count(); ++j)
        {
            dependency.filename = General::getRelativePath(filename, cachedPackages_[lowCase].at(j));
        }
    }
    else
    {
        dependency.filename = packageName + ".vhd";
    }

    // Discard if this is a duplicate.
    for (int i = 0; i < dependencies.size(); ++i)
    {
        if (dependencies[i].filename == dependency.filename)
        {
            return;
        }
    }

    dependencies.append(dependency);
}
