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
	// Find the beginning and end of the module
    int moduleBegin = input.indexOf(VerilogSyntax::MODULE_BEGIN);
    int moduleEnd = VerilogSyntax::MODULE_END.match(input).capturedEnd();

	// Paint the text black.
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
	// Find the beginning of the module.
    int moduleBegin = input.indexOf(VerilogSyntax::MODULE_BEGIN);

	// Operate if it exists.
    if (moduleBegin != -1)
    {
		// Find the name of the module.
        QString moduleName = VerilogSyntax::MODULE_BEGIN.match(input).captured(1);
        targetComponentInstantiation->setModuleName(moduleName);

        if (highlighter_)
        {
			// Find the beginning and the end of the name.
            int moduleNameBegin = input.indexOf(moduleName, moduleBegin);
			int moduleNameEnd = moduleNameBegin + moduleName.length();
			// Highlight the name.
            highlighter_->applyHighlight(moduleNameBegin, moduleNameEnd, ImportColors::VIEWNAME);
        }
    }
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
		newView->setName(NameGenerationPolicy::verilogStructuralViewName(NameGenerationPolicy::flatViewName()));
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
    QString envIdentifierForImport = "verilog:Kactus2:";

    QStringList envIdentifiers = flatView->getEnvIdentifiers();

    if (!envIdentifiers.contains(envIdentifierForImport, Qt::CaseInsensitive))
    {
        envIdentifiers.append(envIdentifierForImport);
    }

	flatView->setEnvIdentifiers(envIdentifiers);

	// Must have a component instantiation for module parameters.
	targetComponentInstantiation = targetComponent->getModel()->
		findComponentInstantiation(flatView->getComponentInstantiationRef());

	if ( !targetComponentInstantiation )
	{
		// Create if does not exist.
		QString instaName = NameGenerationPolicy::verilogComponentInstantiationName( flatView->name() );
		targetComponentInstantiation = QSharedPointer<ComponentInstantiation>( new ComponentInstantiation );
		// Set appropriate name and language.
		targetComponentInstantiation->setName(instaName);
		targetComponentInstantiation->setLanguage("verilog");

		// Add to the component.
		targetComponent->getComponentInstantiations()->append(targetComponentInstantiation);
		// Set the view to refer to it.
		flatView->setComponentInstantiationRef( instaName );
	}
}
