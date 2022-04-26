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
#include <common/NameGenerationPolicy.h>

#include <QString>

namespace
{
    const QRegularExpression MODULENAME_EXP("MODULE\\s+(\\w+)", QRegularExpression::CaseInsensitiveOption);
}


//-----------------------------------------------------------------------------
// Function: VerilogImporter::VerilogImporter()
//-----------------------------------------------------------------------------
VerilogImporter::VerilogImporter():
QObject(0),
portParser_(),
parameterParser_(),
instanceParser_(),
highlighter_(0)
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
    return "1.3";
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
    return tr("Tampere University (tuni.fi)");
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
QWidget* VerilogImporter::getSettingsWidget()
{
    return nullptr;
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
// Function: VerilogImporter::getFileComponents()
//-----------------------------------------------------------------------------
QStringList VerilogImporter::getFileComponents(QString const& input) const
{
    QStringList fileModules;

    QRegularExpression multilineComment(VerilogSyntax::MULTILINE_COMMENT);

    QString inspect = input;
    int moduleBegin = inspect.indexOf(VerilogSyntax::MODULE_BEGIN);
    int moduleEnd = VerilogSyntax::MODULE_END.match(inspect).capturedEnd();

    while (moduleBegin != -1)
    {
        QString newModule = inspect.mid(moduleBegin, moduleEnd - moduleBegin);
        fileModules.append(newModule);
        inspect.remove(newModule);

        moduleBegin = inspect.indexOf(VerilogSyntax::MODULE_BEGIN);
        moduleEnd = VerilogSyntax::MODULE_END.match(inspect).capturedEnd();
    }

    return fileModules;
}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::getComponentName()
//-----------------------------------------------------------------------------
QString VerilogImporter::getComponentName(QString const& componentDeclaration) const
{
    QRegularExpressionMatch match = MODULENAME_EXP.match(componentDeclaration);
    return match.captured(1);
}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::import()
//-----------------------------------------------------------------------------
void VerilogImporter::import(QString const& input, QString const& componentDeclaration,
    QSharedPointer<Component> targetComponent)
{   
    if (hasModuleDeclaration(componentDeclaration))
    {
        highlightModule(input, componentDeclaration);

		QSharedPointer<ComponentInstantiation> targetComponentInstantiation;
		setLanguageAndEnvironmentalIdentifiers(targetComponent, targetComponentInstantiation);
        importModelName(componentDeclaration, targetComponentInstantiation);

        parameterParser_.import(componentDeclaration, targetComponent, targetComponentInstantiation);
        portParser_.import(componentDeclaration, targetComponent, targetComponentInstantiation);
        instanceParser_.import(input, componentDeclaration, targetComponent);
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::setHighlighter()
//-----------------------------------------------------------------------------
void VerilogImporter::setHighlighter(Highlighter* highlighter)
{
    highlighter_ = highlighter;

    instanceParser_.setHighlighter(highlighter);
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
void VerilogImporter::highlightModule(QString const& input, QString const& moduleDeclaration)
{
    QRegularExpressionMatch match = MODULENAME_EXP.match(moduleDeclaration);
    QString moduleDeclarationLine = match.captured();

    int moduleBegin = input.indexOf(moduleDeclarationLine);
    int moduleEnd = VerilogSyntax::MODULE_END.match(input, moduleBegin).capturedEnd();

    QString highlightedModule = input.mid(moduleBegin, moduleEnd - moduleBegin);
    if (highlighter_)
    {
        highlighter_->applyFontColor(highlightedModule, KactusColors::REGULAR_TEXT);
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::importModelName()
//-----------------------------------------------------------------------------
void VerilogImporter::importModelName(QString const& moduleDeclaration,
    QSharedPointer<ComponentInstantiation> targetComponentInstantiation)
{
    QRegularExpressionMatch match = MODULENAME_EXP.match(moduleDeclaration);

    targetComponentInstantiation->setModuleName(match.captured(1));
}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::findOrCreateFlatView()
//-----------------------------------------------------------------------------
QSharedPointer<View> VerilogImporter::findOrCreateFlatView(QSharedPointer<Component> targetComponent) const
{
	// Find any flat views from the target component.
    QStringList flatViews = targetComponent->getFlatViews();

    if (flatViews.isEmpty())
    {
		// If there was none, create a new view.
		QSharedPointer<View> newView( new View() );
		// Rip its name from the generation policy.
		newView->setName(NameGenerationPolicy::flatViewName("verilog"));
		// Add to the component.
		targetComponent->getViews()->append(newView);

		// Return the result.
		return newView;
    }

	// Return the first one from the list.
    return targetComponent->getModel()->findView(flatViews.first());
}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::setLanguageAndEnvironmentalIdentifiers()
//-----------------------------------------------------------------------------
void VerilogImporter::setLanguageAndEnvironmentalIdentifiers(QSharedPointer<Component> targetComponent,
	QSharedPointer<ComponentInstantiation>& targetComponentInstantiation) const
{
	// Needs a view, no matter if it exist.
    QSharedPointer<View> flatView = findOrCreateFlatView(targetComponent);

	// Create environment identifiers for the view as needed.
    QSharedPointer<View::EnvironmentIdentifier> envIdentifierForImport(
        new View::EnvironmentIdentifier(QStringLiteral("Verilog")));
	envIdentifierForImport->tool = VerilogSyntax::TOOL_NAME;

    if (!flatView->hasEnvIdentifier(envIdentifierForImport))
    {
        flatView->getEnvIdentifiers()->append(envIdentifierForImport);
    }

	// Must have a component instantiation for module parameters.
	targetComponentInstantiation = targetComponent->getModel()->
		findComponentInstantiation(flatView->getComponentInstantiationRef());

	if ( !targetComponentInstantiation )
	{
		// Create if does not exist.
		QString instaName = NameGenerationPolicy::componentInstantiationName("verilog");
		targetComponentInstantiation = QSharedPointer<ComponentInstantiation>( new ComponentInstantiation );
		// Set appropriate name and language.
		targetComponentInstantiation->setName(instaName);
		targetComponentInstantiation->setLanguage("Verilog");

		// Add to the component.
		targetComponent->getComponentInstantiations()->append(targetComponentInstantiation);
		// Set the view to refer to it.
		flatView->setComponentInstantiationRef( instaName );
	}
}
