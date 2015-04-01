//-----------------------------------------------------------------------------
// File: VerilogImporter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 23.09.2014
//
// Description:
// Import plugin for verilog source files.
//-----------------------------------------------------------------------------

#include "VerilogImporter.h"

#include "VerilogSyntax.h"

#include <IPXACTmodels/component.h>
#include <Plugins/PluginSystem/ImportPlugin/ImportColors.h>

#include <QString>

//-----------------------------------------------------------------------------
// Function: VerilogImporter::VerilogImporter()
//-----------------------------------------------------------------------------
VerilogImporter::VerilogImporter(): QObject(0), portParser_(), parameterParser_(), highlighter_(0)
{

}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::~VerilogImporter()
//-----------------------------------------------------------------------------
VerilogImporter::~VerilogImporter()
{

}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::getName()
//-----------------------------------------------------------------------------
QString const& VerilogImporter::getName() const
{
    static QString name("Verilog importer");
    return name;
}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::getVersion()
//-----------------------------------------------------------------------------
QString const& VerilogImporter::getVersion() const
{
    static QString version("1.1");
    return version;
}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::getDescription()
//-----------------------------------------------------------------------------
QString const& VerilogImporter::getDescription() const
{
    static QString description("Imports model parameters and ports from a verilog file.");
    return description;
}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::getVendor()
//-----------------------------------------------------------------------------
QString const& VerilogImporter::getVendor() const
{
    static QString vendor("TUT");
    return vendor;
}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::getLicence()
//-----------------------------------------------------------------------------
QString const& VerilogImporter::getLicence() const
{
    static QString licence("GPL2");
    return licence;
}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::getLicenceHolder()
//-----------------------------------------------------------------------------
QString const& VerilogImporter::getLicenceHolder() const
{
    static QString licenceHolder(tr("Public"));
    return licenceHolder;
}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::getSettingsWidget()
//-----------------------------------------------------------------------------
PluginSettingsWidget* VerilogImporter::getSettingsWidget()
{
    return new PluginSettingsWidget();
}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::getProgramRequirements()
//-----------------------------------------------------------------------------
QList<IPlugin::ExternalProgramRequirement> VerilogImporter::getProgramRequirements()
{
    return QList<IPlugin::ExternalProgramRequirement>();
}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::acceptedFileTypes()
//-----------------------------------------------------------------------------
QStringList VerilogImporter::getSupportedFileTypes() const
{
    QStringList supportedTypes;
    supportedTypes << "verilogSource" << "verilogSource-95" << "verilogSource-2001";

    return supportedTypes;
}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::getCompatibilityWarnings()
//-----------------------------------------------------------------------------
QString VerilogImporter::getCompatibilityWarnings() const
{
    return QString();
}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::import()
//-----------------------------------------------------------------------------
void VerilogImporter::import(QString const& input, QSharedPointer<Component> targetComponent)
{   
    if (hasModuleDeclaration(input))
    {
        highlightModule(input);

        importModelName(input, targetComponent);
        setLanguageAndEnvironmentalIdentifiers(targetComponent);

        parameterParser_.import(input, targetComponent);

        portParser_.import(input, targetComponent);
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::setHighlighter()
//-----------------------------------------------------------------------------
void VerilogImporter::setHighlighter(Highlighter* highlighter)
{
    highlighter_ = highlighter;

    parameterParser_.setHighlighter(highlighter);
    portParser_.setHighlighter(highlighter);
}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::setExpressionParser()
//-----------------------------------------------------------------------------
void VerilogImporter::setExpressionParser(QSharedPointer<ExpressionParser> parser)
{
    portParser_.setExpressionParser(parser);
}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::hasModuleDeclaration()
//-----------------------------------------------------------------------------
bool VerilogImporter::hasModuleDeclaration(QString const& input)
{
    int moduleBegin = input.indexOf(VerilogSyntax::MODULE_BEGIN);
    int moduleEnd = input.indexOf(VerilogSyntax::MODULE_END, moduleBegin);

    return moduleBegin != -1 && moduleEnd != -1;
}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::highlightModule()
//-----------------------------------------------------------------------------
void VerilogImporter::highlightModule(QString const& input)
{
    int moduleBegin = input.indexOf(VerilogSyntax::MODULE_BEGIN);
    int moduleEnd = VerilogSyntax::MODULE_END.match(input).capturedEnd();

    if (highlighter_)
    {        
        highlighter_->applyFontColor(input.mid(moduleBegin, moduleEnd  - moduleBegin), QColor("black"));
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::importModelName()
//-----------------------------------------------------------------------------
void VerilogImporter::importModelName(QString const& input, QSharedPointer<Component> targetComponent)
{
    int moduleBegin = input.indexOf(VerilogSyntax::MODULE_BEGIN);
    if (moduleBegin != -1)
    {
        QString modelName = VerilogSyntax::MODULE_BEGIN.match(input).captured(1);

        View* flatView = findOrCreateFlatView(targetComponent);
        flatView->setModelName(modelName);

        if (highlighter_)
        {
            int modelNameBegin = input.indexOf(modelName, moduleBegin);
            int modelNameEnd = modelNameBegin + modelName.length();
            highlighter_->applyHighlight(modelNameBegin, modelNameEnd, ImportColors::VIEWNAME);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::findOrCreateFlatView()
//-----------------------------------------------------------------------------
View* VerilogImporter::findOrCreateFlatView(QSharedPointer<Component> targetComponent) const
{
    QStringList flatViews = targetComponent->getFlatViews();
    if (flatViews.isEmpty())
    {
        targetComponent->createEmptyFlatView();
        flatViews = targetComponent->getFlatViews();
    }

    return targetComponent->findView(flatViews.first());
}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::setLanguageAndEnvironmentalIdentifiers()
//-----------------------------------------------------------------------------
void VerilogImporter::setLanguageAndEnvironmentalIdentifiers(QSharedPointer<Component> targetComponent) const
{
    View* flatView = findOrCreateFlatView(targetComponent);
    flatView->setLanguage("verilog");

    QString envIdentifierForImport = "verilog:Kactus2:";

    QStringList envIdentifiers = flatView->getEnvIdentifiers();

    if (envIdentifiers.contains("::"))
    {
        envIdentifiers.replace(envIdentifiers.indexOf("::"), envIdentifierForImport);
    }
    else if (!envIdentifiers.contains(envIdentifierForImport, Qt::CaseInsensitive))
    {
        envIdentifiers.append(envIdentifierForImport);
    }

    flatView->setEnvIdentifiers(envIdentifiers);
}
