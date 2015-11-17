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

#include <IPXACTmodels/Component/Component.h>
#include <Plugins/PluginSystem/ImportPlugin/ImportColors.h>

#include <QString>
#include "../common/NameGenerationPolicy.h"

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
QString VerilogImporter::getName() const
{
    return "Verilog importer";
}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::getVersion()
//-----------------------------------------------------------------------------
QString VerilogImporter::getVersion() const
{
    return "1.2";
}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::getDescription()
//-----------------------------------------------------------------------------
QString VerilogImporter::getDescription() const
{
    return "Imports model parameters and ports from a verilog file.";
}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::getVendor()
//-----------------------------------------------------------------------------
QString VerilogImporter::getVendor() const
{
    return tr("TUT");
}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::getLicence()
//-----------------------------------------------------------------------------
QString VerilogImporter::getLicence() const
{
    return tr("GPL2");
}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::getLicenceHolder()
//-----------------------------------------------------------------------------
QString VerilogImporter::getLicenceHolder() const
{
    return tr("Public");
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
    supportedTypes << "verilogSource" << "verilogSource-95" << "verilogSource-2001" << "systemVerilogSource";

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

		QSharedPointer<ComponentInstantiation> targetComponentInstantiation;
		setLanguageAndEnvironmentalIdentifiers(targetComponent, targetComponentInstantiation);
		importModelName(input, targetComponent, targetComponentInstantiation);

        parameterParser_.import(input, targetComponent, targetComponentInstantiation);

        portParser_.import(input, targetComponent, targetComponentInstantiation);
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
	QRegularExpression multilineComment(VerilogSyntax::MULTILINE_COMMENT);

	QString inspect = input;
	inspect = inspect.remove(VerilogSyntax::COMMENTLINE).remove(multilineComment);

    int moduleBegin = inspect.indexOf(VerilogSyntax::MODULE_BEGIN);
    int moduleEnd = inspect.indexOf(VerilogSyntax::MODULE_END, moduleBegin);

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
void VerilogImporter::importModelName(QString const& input, QSharedPointer<Component> targetComponent,
	QSharedPointer<ComponentInstantiation> targetComponentInstantiation)
{
    int moduleBegin = input.indexOf(VerilogSyntax::MODULE_BEGIN);
    if (moduleBegin != -1)
    {
        QString modelName = VerilogSyntax::MODULE_BEGIN.match(input).captured(1);

        targetComponentInstantiation->setModuleName(modelName);

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
QSharedPointer<View> VerilogImporter::findOrCreateFlatView(QSharedPointer<Component> targetComponent) const
{
    QStringList flatViews = targetComponent->getFlatViews();
    if (flatViews.isEmpty())
    {
		// create new view
		QSharedPointer<View> newView( new View() );
		newView->setName("flat");
		newView->addEnvIdentifier(QString("::"));
		targetComponent->getViews()->append(newView);

        flatViews = targetComponent->getFlatViews();
    }

    return targetComponent->getModel()->findView(flatViews.first());
}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::setLanguageAndEnvironmentalIdentifiers()
//-----------------------------------------------------------------------------
void VerilogImporter::setLanguageAndEnvironmentalIdentifiers(QSharedPointer<Component> targetComponent,
	QSharedPointer<ComponentInstantiation>& targetComponentInstantiation) const
{
    QSharedPointer<View> flatView = findOrCreateFlatView(targetComponent);

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

	// Must have a component instantiation for module parameters.
	QString instaName = NameGenerationPolicy::verilogComponentInstantiationName( flatView->name() );
	targetComponentInstantiation = targetComponent->getModel()->findComponentInstantiation(instaName);

	// Create and add to the component if does not exist.
	if ( !targetComponentInstantiation )
	{
		targetComponentInstantiation = QSharedPointer<ComponentInstantiation>( new ComponentInstantiation );
		targetComponentInstantiation->setName(instaName);
		targetComponentInstantiation->setLanguage("verilog");
		targetComponent->getComponentInstantiations()->append(targetComponentInstantiation);
	}

	flatView->setComponentInstantiationRef( instaName );
}
