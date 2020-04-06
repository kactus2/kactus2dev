//-----------------------------------------------------------------------------
// File: VHDLSourceAnalyzer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Honkonen, Joni-Matti M‰‰tt‰
// Date: 18.1.2013
//
// Description:
// VHDL source analyzer plugin.
//-----------------------------------------------------------------------------

#include "VHDLSourceAnalyzer.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <QCryptographicHash>
#include <QRegularExpression>
//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::VHDLSourceAnalyzer()
//-----------------------------------------------------------------------------
VHDLSourceAnalyzer::VHDLSourceAnalyzer(): QObject(nullptr)
{
    
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::getName()
//----------------------------------------------------------------------------
QString VHDLSourceAnalyzer::getName() const
{
    return "VHDL Source Analyzer";
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::getVersion()
//-----------------------------------------------------------------------------
QString VHDLSourceAnalyzer::getVersion() const
{
    return "1.2";
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::getVendor()
//-----------------------------------------------------------------------------
QString VHDLSourceAnalyzer::getVendor() const
{
    return tr("tuni.fi");
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::getLicence()
//-----------------------------------------------------------------------------
QString VHDLSourceAnalyzer::getLicence() const
{
    return tr("GPL2");
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::getLicenceHolder()
//-----------------------------------------------------------------------------
QString VHDLSourceAnalyzer::getLicenceHolder() const
{
    return tr("Public");
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::getDescription()
//-----------------------------------------------------------------------------
QString VHDLSourceAnalyzer::getDescription() const
{
    return "Analyzes file dependencies from VHDL files.";
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::getSettingsWidget()
//-----------------------------------------------------------------------------
QWidget* VHDLSourceAnalyzer::getSettingsWidget()
{
    return new QWidget();
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::getSupportedFileTypes()
//-----------------------------------------------------------------------------
QStringList VHDLSourceAnalyzer::getSupportedFileTypes() const
{
    return fileTypes_;
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::calculateHash()
//-----------------------------------------------------------------------------
QString VHDLSourceAnalyzer::calculateHash(QString const& filename)
{
    QString source = getSourceData(filename);

    // Calculate the hash
    QCryptographicHash hash(QCryptographicHash::Sha1);
    hash.addData(source.toLatin1());

    return hash.result().toHex();
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::getFileDependencies()
//-----------------------------------------------------------------------------
QList<FileDependencyDesc> VHDLSourceAnalyzer::getFileDependencies(
	Component const* /*component*/, QString const& /*componentPath*/, QString const& filename)
{
	QList<FileDependencyDesc> dependencies;

	// Read file contents into a buffer.
	QString source = getSourceData(filename);

	scanEntityReferences(source, filename, dependencies);
	scanPackageReferences(source, filename, dependencies);

	return dependencies;
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::getSourceData()
//-----------------------------------------------------------------------------
QString VHDLSourceAnalyzer::getSourceData(QString const& filename)
{
	// Try to open the file
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return QString();
	}

    // Read the file data
    QString contents = file.readAll();
    contents.remove(QRegularExpression("--[^\\n]*"));

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
void VHDLSourceAnalyzer::endAnalysis(Component const* /*component*/, QString const& /*componentPath*/)
{
    cachedEntities_.clear();
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::scanEntities()
//-----------------------------------------------------------------------------
void VHDLSourceAnalyzer::scanDefinitions(Component const* component, QString const& componentPath)
{
    // Scan all the file sets.
    for (auto const& fileset : *component->getFileSets())
    {
        for (auto const& file : *fileset->getFiles())
        {
            if (file->getFileTypes()->contains("vhdlSource"))
            {
				QString filename = General::getAbsolutePath(componentPath, file->name());
				QString source = getSourceData(filename);

				scanEntities(source, filename);
				scanPackages(source, filename);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::getProgramRequirements()
//-----------------------------------------------------------------------------
QList<IPlugin::ExternalProgramRequirement> VHDLSourceAnalyzer::getProgramRequirements()
{
	return QList<IPlugin::ExternalProgramRequirement>();
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::scanEntities()
//-----------------------------------------------------------------------------
void VHDLSourceAnalyzer::scanEntities(QString const& source, QString const& filename)
{
    // Look for entities.
    static QRegularExpression entityBeginExp("\\bENTITY\\s+(\\w+)\\s+(?:IS)\\s*",
		QRegularExpression::CaseInsensitiveOption);

	auto matches = entityBeginExp.globalMatch(source);
    while (matches.hasNext())
    {
		auto match = matches.next();
        // Register the entity name.
        QString entityName = match.captured(1).toLower();
        cachedEntities_[entityName].append(filename);
    }
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::scanPackages()
//-----------------------------------------------------------------------------
void VHDLSourceAnalyzer::scanPackages(QString const& source, QString const& filename)
{
    // Look for packages.
    static QRegularExpression packageBeginExp("\\bPACKAGE\\s+(\\w+)\\s+IS\\s*",
		QRegularExpression::CaseInsensitiveOption);
	
	auto matches = packageBeginExp.globalMatch(source);

    while (matches.hasNext())
    {
		auto match = matches.next();
        // Register the package name.
        QString packageName = match.captured(1).toLower();
        cachedPackages_[packageName].append(filename);
    }
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::scanEntityReferences()
//-----------------------------------------------------------------------------
void VHDLSourceAnalyzer::scanEntityReferences(QString const& source, QString const& filename,
	 QList<FileDependencyDesc>& dependencies)
{
	static QRegularExpression entityExp(
		"\\b(\\w+)\\s+\\:\\s+(ENTITY\\s+)?(\\w+\\.)?(\\w+)\\s+(GENERIC|PORT)\\s+MAP\\b",
		QRegularExpression::CaseInsensitiveOption);

	auto entityMatches = entityExp.globalMatch(source);
	while (entityMatches.hasNext())
	{
		auto entityMatch = entityMatches.next();

		QString entityName = entityMatch.captured(4);
		addEntityDependency(entityName, filename, dependencies);
	}
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::scanPackageReferences()
//-----------------------------------------------------------------------------
void VHDLSourceAnalyzer::scanPackageReferences(QString const& source, QString const& filename,
                                               QList<FileDependencyDesc>& dependencies)
{
    static QRegularExpression packageRefExp("\\bUSE\\s+(\\w+)\\.(\\w+)\\.(\\w+)\\b",
		QRegularExpression::CaseInsensitiveOption);
    
	auto matches = packageRefExp.globalMatch(source);
    while (matches.hasNext())
    {
		auto match = matches.next();

        // Discard anything in the IEEE library.
        QString libraryName = match.captured(1).toLower();
        if (libraryName != QLatin1String("ieee") && libraryName != QLatin1String("std"))
        {
            QString packageName = match.captured(2);
            addPackageDependency(packageName, filename, dependencies);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::addDependency()
//-----------------------------------------------------------------------------
void VHDLSourceAnalyzer::addEntityDependency(QString const& componentName, QString const& filename,
                                             QList<FileDependencyDesc>& dependencies)
{
	const QString description("Component instantiation for entity %1");
  
    QString lowCase = componentName.toLower();

    if (cachedEntities_.contains(lowCase))
    {
        // Add all existing entities to the return value list.
        for (auto const& cachedFile : cachedEntities_[lowCase])
        {
			FileDependencyDesc dependency;
			dependency.description = description.arg(componentName);
            dependency.filename = General::getRelativePath(filename, cachedFile);

			addUniqueDependency(dependency, dependencies);
        }
    }
    else
    {
		FileDependencyDesc dependency;
		dependency.description = description.arg(componentName);
        dependency.filename = componentName + ".vhd";

		addUniqueDependency(dependency, dependencies);
    }
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::addPackageDependency()
//-----------------------------------------------------------------------------
void VHDLSourceAnalyzer::addPackageDependency(QString const& packageName, QString const& filename,
                                              QList<FileDependencyDesc> &dependencies)
{
	const QString description = "Reference to package %1";
  
    QString lowCase = packageName.toLower();
    if (cachedPackages_.contains(lowCase))
    {
        // Add all existing entities to the return value list.
        for (auto const& cachedFile : cachedPackages_[lowCase])
        {
			FileDependencyDesc dependency;
			dependency.description = description.arg(packageName);
            dependency.filename = General::getRelativePath(filename, cachedFile);

			addUniqueDependency(dependency, dependencies);
        }
    }
    else
    {
		FileDependencyDesc dependency;
		dependency.description = description.arg(packageName);
        dependency.filename = packageName + ".vhd";

		addUniqueDependency(dependency, dependencies);
    }
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::addUniqueDependency()
//-----------------------------------------------------------------------------
void VHDLSourceAnalyzer::addUniqueDependency(FileDependencyDesc const& dependency,
	QList<FileDependencyDesc>& dependencies)
{
	// Discard if this is a duplicate.
	if (dependencies.contains(dependency))
	{
		return;
	}

	dependencies.append(dependency);
}
