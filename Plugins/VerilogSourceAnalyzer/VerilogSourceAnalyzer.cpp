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

#include <IPXACTmodels/file.h>
#include <IPXACTmodels/fileset.h>

#include <QCryptographicHash>
#include <QDir>

//-----------------------------------------------------------------------------
// Function: VerilogSourceAnalyzer::VerilogSourceAnalyzer()
//-----------------------------------------------------------------------------
VerilogSourceAnalyzer::VerilogSourceAnalyzer() : QObject(0)
{

}

//-----------------------------------------------------------------------------
// Function: VerilogSourceAnalyzer::~VerilogSourceAnalyzer()
//-----------------------------------------------------------------------------
VerilogSourceAnalyzer::~VerilogSourceAnalyzer()
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
    return "0.1";
}

//-----------------------------------------------------------------------------
// Function: VerilogSourceAnalyzer::getDescription()
//-----------------------------------------------------------------------------
QString VerilogSourceAnalyzer::getDescription() const
{
    return "Analyzes file dependencies from Verilog files.";
}

//-----------------------------------------------------------------------------
// Function: VerilogSourceAnalyzer::getVendor()
//-----------------------------------------------------------------------------
QString VerilogSourceAnalyzer::getVendor() const
{
    return tr("TUT");
}

//-----------------------------------------------------------------------------
// Function: VerilogSourceAnalyzer::getLicence()
//-----------------------------------------------------------------------------
QString VerilogSourceAnalyzer::getLicence() const
{
    return tr("GPL2");
}

//-----------------------------------------------------------------------------
// Function: VerilogSourceAnalyzer::getLicenceHolder()
//-----------------------------------------------------------------------------
QString VerilogSourceAnalyzer::getLicenceHolder() const
{
    return tr("Public");
}

//-----------------------------------------------------------------------------
// Function: VerilogSourceAnalyzer::getSettingsWidget()
//-----------------------------------------------------------------------------
PluginSettingsWidget* VerilogSourceAnalyzer::getSettingsWidget()
{
    return new PluginSettingsWidget();
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
    QStringList fileTypes;
    fileTypes << "verilogSource" << "verilogSource-95" << "verilogSource-2001";
    return fileTypes;
}

//-----------------------------------------------------------------------------
// Function: VerilogSourceAnalyzer::calculateHash()
//-----------------------------------------------------------------------------
QString VerilogSourceAnalyzer::calculateHash(QString const& filename)
{
    QString content = readFileContentAndRemoveComments(filename);

    if (content.isEmpty())
    {
        return content;
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
    
}

//-----------------------------------------------------------------------------
// Function: VerilogSourceAnalyzer::endAnalysis()
//-----------------------------------------------------------------------------
void VerilogSourceAnalyzer::endAnalysis(Component const* /*component*/, QString const& /*componentPath*/)
{
    
}

//-----------------------------------------------------------------------------
// Function: VerilogSourceAnalyzer::getFileDependencies()
//-----------------------------------------------------------------------------
QList<FileDependencyDesc> VerilogSourceAnalyzer::getFileDependencies(Component const* component, 
    QString const& componentPath, QString const& filename)
{
    QList<FileDependencyDesc> dependencies;

    QString fileAbsolutePath = findAbsolutePathFor(filename, componentPath);
    QString fileContent = readFileContentAndRemoveComments(fileAbsolutePath);

    dependencies.append(getIncludeDependencies(fileContent, fileAbsolutePath, componentPath, 
        component->getFileSets()));

    return dependencies;
}

//-----------------------------------------------------------------------------
// Function: VerilogSourceAnalyzer::readFileContentAndRemoveComments()
//-----------------------------------------------------------------------------
QString VerilogSourceAnalyzer::readFileContentAndRemoveComments(QString const& filePath) const
{
    QFile targetFile(filePath);

    if (!targetFile.open(QIODevice::ReadOnly))
    {
        return QString();
    }

    QString fileContent = targetFile.readAll();
    targetFile.close();

    fileContent.remove(VerilogSyntax::MULTILINE_COMMENT);
    fileContent.remove(VerilogSyntax::COMMENTLINE);

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
// Function: VerilogSourceAnalyzer::getIncludeDependencies()
//-----------------------------------------------------------------------------
QList<FileDependencyDesc> VerilogSourceAnalyzer::getIncludeDependencies(QString const& fileContent, 
    QString const& fileAbsolutePath, QString const& xmlPath, QList<QSharedPointer<FileSet> > const& fileSets) const
{
    QList<FileDependencyDesc> includes;

    QDir absoluteFileDir = QFileInfo(fileAbsolutePath).absoluteDir();

    QRegularExpression includePattern("`include [\"<](.*?)[\">]");
    int position = 0;

    QRegularExpressionMatch match = includePattern.match(fileContent, position);
    while (match.hasMatch())
    {        
        QString targetFileName = match.captured(1);

        QString targetAbsolutePath = findAbsolutePathInFileSets(targetFileName, xmlPath, fileSets);
        QString targetRelativePath = absoluteFileDir.relativeFilePath(targetAbsolutePath);

        FileDependencyDesc includeDependency;
        includeDependency.filename = targetRelativePath;
        includeDependency.description = tr("Include reference to %1").arg(targetFileName);
        includes.append(includeDependency);

        position = match.capturedEnd();
        match = includePattern.match(fileContent, position);
    }

    return includes;
}

//-----------------------------------------------------------------------------
// Function: VerilogSourceAnalyzer::findAbsolutePathInFileSets()
//-----------------------------------------------------------------------------
QString VerilogSourceAnalyzer::findAbsolutePathInFileSets(QString fileName,
    QString const& xmlPath, QList<QSharedPointer<FileSet> > const& fileSets) const
{
    QRegularExpression namePattern("(?:^|[/\\\\])" + fileName + "$");
    foreach (QSharedPointer<FileSet> fileSet, fileSets)
    {
        foreach (QSharedPointer<File> file, fileSet->getFiles())
        {
            if (namePattern.match(file->getName()).hasMatch())
            {
                return QFileInfo(xmlPath + "/" + file->getName()).absoluteFilePath();
            }
        } 
    }	

    return fileName;
}