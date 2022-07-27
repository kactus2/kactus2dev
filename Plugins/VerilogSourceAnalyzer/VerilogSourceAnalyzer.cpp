//-----------------------------------------------------------------------------
// File: VerilogSourceAnalyzer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 07.04.2015
//
// Description:
// Verilog source file analyzer for finding file dependencies.
//-----------------------------------------------------------------------------

#include "VerilogSourceAnalyzer.h"

#include <Plugins/VerilogImport/VerilogSyntax.h>

#include <QCryptographicHash>
#include <QDir>
#include <QDebug>
//-----------------------------------------------------------------------------
// Function: VerilogSourceAnalyzer::VerilogSourceAnalyzer()
//-----------------------------------------------------------------------------
VerilogSourceAnalyzer::VerilogSourceAnalyzer() : QObject(nullptr)
{

}

//-----------------------------------------------------------------------------
// Function: VerilogSourceAnalyzer::getName()
//-----------------------------------------------------------------------------
QString VerilogSourceAnalyzer::getName() const
{
    return "Verilog Source Analyzer";
}

//-----------------------------------------------------------------------------
// Function: VerilogSourceAnalyzer::getVersion()
//-----------------------------------------------------------------------------
QString VerilogSourceAnalyzer::getVersion() const
{
    return "0.10";
}

//-----------------------------------------------------------------------------
// Function: VerilogSourceAnalyzer::getDescription()
//-----------------------------------------------------------------------------
QString VerilogSourceAnalyzer::getDescription() const
{
    return "Analyzes file dependencies from (System)Verilog files.";
}

//-----------------------------------------------------------------------------
// Function: VerilogSourceAnalyzer::getVendor()
//-----------------------------------------------------------------------------
QString VerilogSourceAnalyzer::getVendor() const
{
    return tr("tuni.fi");
}

//-----------------------------------------------------------------------------
// Function: VerilogSourceAnalyzer::getLicense()
//-----------------------------------------------------------------------------
QString VerilogSourceAnalyzer::getLicense() const
{
    return tr("GPL2");
}

//-----------------------------------------------------------------------------
// Function: VerilogSourceAnalyzer::getLicenseHolder()
//-----------------------------------------------------------------------------
QString VerilogSourceAnalyzer::getLicenseHolder() const
{
    return tr("Public");
}

//-----------------------------------------------------------------------------
// Function: VerilogSourceAnalyzer::getSettingsWidget()
//-----------------------------------------------------------------------------
QWidget* VerilogSourceAnalyzer::getSettingsWidget()
{
    return nullptr;
}

//-----------------------------------------------------------------------------
// Function: VerilogSourceAnalyzer::getProgramRequirements()
//-----------------------------------------------------------------------------
QList<IPlugin::ExternalProgramRequirement> VerilogSourceAnalyzer::getProgramRequirements()
{
    return QList<IPlugin::ExternalProgramRequirement>();
}

//-----------------------------------------------------------------------------
// Function: VerilogSourceAnalyzer::getSupportedFileTypes()
//-----------------------------------------------------------------------------
QStringList VerilogSourceAnalyzer::getSupportedFileTypes() const
{
    return QStringList({"verilogSource", "verilogSource-95", "verilogSource-2001", "systemVerilogSource"});
}

//-----------------------------------------------------------------------------
// Function: VerilogSourceAnalyzer::calculateHash()
//-----------------------------------------------------------------------------
QString VerilogSourceAnalyzer::calculateHash(QString const& filename)
{
    QString content = readFileContentAndRemoveComments(filename);
    if (content.isEmpty())
    {
        return QString();
    }

    QCryptographicHash hashFunction(QCryptographicHash::Sha1);
    hashFunction.addData(content.toLatin1());

    return hashFunction.result().toHex();
}

//-----------------------------------------------------------------------------
// Function: VerilogSourceAnalyzer::beginAnalysis()
//-----------------------------------------------------------------------------
void VerilogSourceAnalyzer::beginAnalysis(Component const* /*component*/, QString const& /*componentPath*/)
{
    // Nothing to do.
}

//-----------------------------------------------------------------------------
// Function: VerilogSourceAnalyzer::endAnalysis()
//-----------------------------------------------------------------------------
void VerilogSourceAnalyzer::endAnalysis(Component const* /*component*/, QString const& /*componentPath*/)
{
	// Nothing to do.
}

//-----------------------------------------------------------------------------
// Function: VerilogSourceAnalyzer::getFileDependencies()
//-----------------------------------------------------------------------------
QList<FileDependencyDesc> VerilogSourceAnalyzer::getFileDependencies(Component const* component, 
    QString const& componentPath, QString const& filename)
{
	QMap<QString, QString> itemsInFilesets = findItemsInFilesets(component, componentPath);
	
	QString sourceAbsolutePath = findAbsolutePathFor(filename, componentPath);
	QFileInfo sourceFileInfo(sourceAbsolutePath);

	QString fileContent = readFileContentAndRemoveComments(sourceAbsolutePath);

	QList<FileDependencyDesc> dependencies;
	dependencies.append(findIncludeDependencies(fileContent, sourceFileInfo, itemsInFilesets));
	dependencies.append(findInstantiationDependencies(fileContent, sourceFileInfo, itemsInFilesets));

    return dependencies;
}

//-----------------------------------------------------------------------------
// Function: VerilogSourceAnalyzer::readFileContentAndRemoveComments()
//-----------------------------------------------------------------------------
QString VerilogSourceAnalyzer::readFileContentAndRemoveComments(QString const& filePath) const
{
    QFile targetFile(filePath);

    if (!targetFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return QString();
    }

    QString fileContent = targetFile.readAll();
    targetFile.close();

    fileContent.remove(VerilogSyntax::MULTILINE_COMMENT);
    fileContent.remove(QRegularExpression(VerilogSyntax::COMMENT));

    return fileContent.simplified();
}

//-----------------------------------------------------------------------------
// Function: VerilogSourceAnalyzer::findAbsolutePathFor()
//-----------------------------------------------------------------------------
QString VerilogSourceAnalyzer::findAbsolutePathFor(QString const& filename, QString const& componentPath) const
{
	QString path = filename;
	if (!QFileInfo(path).isAbsolute())
	{
		path = componentPath + "/" + filename;
	}

	return path;
}

//-----------------------------------------------------------------------------
// Function: VerilogSourceAnalyzer::findItemsInFilesets()
//-----------------------------------------------------------------------------
QMap<QString, QString> VerilogSourceAnalyzer::findItemsInFilesets(Component const* component,
	QString const& componentPath)
{
	QMap<QString, QString> itemsInFilesets;
	for (auto const& fileSet : *component->getFileSets())
	{
		for (auto const& file : *fileSet->getFiles())
		{
			if (isOfSupportedFileType(file))
			{
				QString path = findAbsolutePathFor(file->name(), componentPath);
				QString content = readFileContentAndRemoveComments(file->name());

				itemsInFilesets.insert(findItemsInFileContent(content, path));

			}
		}
	}

	return itemsInFilesets;
}

//-----------------------------------------------------------------------------
// Function: VerilogSourceAnalyzer::findItemsInFileContent()
//-----------------------------------------------------------------------------
QMap<QString, QString> VerilogSourceAnalyzer::findItemsInFileContent(QString const& content,
	QString const& filePath)
{
	QMap<QString, QString> itemsInFile;

	auto modulesInFile = VerilogSyntax::MODULE_KEY_WORD.globalMatch(content);
	bool isHeader = modulesInFile.hasNext() == false;
	while (modulesInFile.hasNext())
	{
		auto match = modulesInFile.next();
		QString moduleName = match.captured(1);
		itemsInFile.insert(moduleName, filePath);
	}

	if (isHeader)
	{
		itemsInFile.insert(QFileInfo(filePath).fileName(), filePath);
	}

	return itemsInFile;
}

//-----------------------------------------------------------------------------
// Function: VerilogSourceAnalyzer::isOfSupportedFileType()
//-----------------------------------------------------------------------------
bool VerilogSourceAnalyzer::isOfSupportedFileType(QSharedPointer<File> file)
{
	for (auto const& filetype : *file->getFileTypes())
	{
		if (getSupportedFileTypes().contains(filetype))
		{
			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// Function: VerilogSourceAnalyzer::findIncludeDependencies()
//-----------------------------------------------------------------------------
QList<FileDependencyDesc> VerilogSourceAnalyzer::findIncludeDependencies(QString const& fileContent,
	QFileInfo const& sourceFileInfo, QMap<QString, QString> const& itemsInFilesets) const
{
	QList<FileDependencyDesc> dependencies;

	const QDir sourceAbsoluteDir = sourceFileInfo.absoluteDir();

	QStringList includeFiles = findDependencies(fileContent, QRegularExpression("`include [\"<](.*?)[\">]"));
	for (auto const& includeName : includeFiles)
	{
		QString targetAbsolutePath = itemsInFilesets.value(includeName, includeName);
		QString targetRelativePath = sourceAbsoluteDir.relativeFilePath(targetAbsolutePath);

		FileDependencyDesc dependency;
		dependency.description = tr("Include reference to %1").arg(includeName); 
		dependency.filename =  targetRelativePath;
		dependencies.append(dependency);
	}

	return dependencies;
}

//-----------------------------------------------------------------------------
// Function: VerilogSourceAnalyzer::findDependencies()
//-----------------------------------------------------------------------------
QStringList VerilogSourceAnalyzer::findDependencies(QString const& fileContent,
	QRegularExpression const& matchPattern) const
{
	QStringList dependentFiles;
	auto matches = matchPattern.globalMatch(fileContent);
	while (matches.hasNext())
	{
		auto match = matches.next();
		QString dependencyName = match.captured(1);

		if (VerilogSyntax::KEYWORDS.contains(dependencyName) == false && 
			dependentFiles.contains(dependencyName) == false)
		{
			dependentFiles.append(dependencyName);
		}
	}

	return dependentFiles;
}

//-----------------------------------------------------------------------------
// Function: VerilogSourceAnalyzer::findInstantiationDependencies()
//-----------------------------------------------------------------------------
QList<FileDependencyDesc>  VerilogSourceAnalyzer::findInstantiationDependencies(QString const& fileContent, 
	QFileInfo const& sourceFileInfo, QMap<QString, QString> const& itemsInFilesets) const
{
	QStringList instanceFiles = findDependencies(fileContent, 
		QRegularExpression(";\\s+([a-zA-Z_][\\w$]*)(\\s+#[(].*[)])?\\s+([a-zA-Z_][\\w$]*)",
		QRegularExpression::DotMatchesEverythingOption | QRegularExpression::InvertedGreedinessOption));

	const QString sourceFileSuffix = "." + sourceFileInfo.suffix();
	const QDir sourceAbsoluteDir = sourceFileInfo.absoluteDir();

	QList<FileDependencyDesc> dependencies;
	for (auto const& moduleName : instanceFiles)
	{
		QString targetAbsolutePath = itemsInFilesets.value(moduleName, moduleName + sourceFileSuffix);
		QString targetRelativePath = sourceAbsoluteDir.relativeFilePath(targetAbsolutePath);

		FileDependencyDesc dependency;
		dependency.description = tr("Submodule instantiation of module %1").arg(moduleName); 
		dependency.filename = targetRelativePath;		
		dependencies.append(dependency);
	}

	return dependencies;
}
