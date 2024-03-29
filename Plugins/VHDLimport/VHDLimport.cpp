//-----------------------------------------------------------------------------
// File: VHDLimport.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.06.2013
//
// Description:
// VHDLimport reads a given input, creates ports and generics accordingly
// and highlights the created ports and generics. 
//-----------------------------------------------------------------------------

#include "VHDLimport.h"

#include <QRegularExpression>
#include <QList>

#include <KactusAPI/include/ImportColors.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/common/ModuleParameter.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/View.h>

#include <KactusAPI/include/Highlighter.h>
#include <KactusAPI/include/ModelParameterVisualizer.h>

#include <common/NameGenerationPolicy.h>

#include "VHDLPortParser.h"
#include "VHDLGenericParser.h"

namespace
{
    //!  Regular expression for VHDL entity.
    const QRegularExpression ENTITY_EXP("ENTITY\\s+(\\w+)\\s+IS\\s+.*END(?:\\s+ENTITY)?(?:\\s+\\1)?\\s*[;]",
        QRegularExpression::CaseInsensitiveOption | QRegularExpression::DotMatchesEverythingOption);

    //! Regular expression for finding the name of a VHDL entity.
    const QRegularExpression ENTITYNAME_EXP("ENTITY\\s+(\\w+)\\s+IS", QRegularExpression::CaseInsensitiveOption);
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::VHDLimport()
//-----------------------------------------------------------------------------
VHDLimport::VHDLimport() : QObject(0),
    portParser_(new VHDLPortParser(this)),
    genericParser_(new VHDLGenericParser(this)),
    targetComponent_(0),        
    highlighter_(0)
{
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::~VHDLimport()
//-----------------------------------------------------------------------------
VHDLimport::~VHDLimport()
{

}

//-----------------------------------------------------------------------------
// Function: VHDLimport::getName()
//-----------------------------------------------------------------------------
QString VHDLimport::getName() const
{
    return tr("VHDL importer");
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::getVersion()
//-----------------------------------------------------------------------------
QString VHDLimport::getVersion() const
{
    return QStringLiteral("1.2");
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::getDescription()
//-----------------------------------------------------------------------------
QString VHDLimport::getDescription() const
{
    return tr("Imports model parameters and ports from a VHDL file.");
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::getVendor()
//-----------------------------------------------------------------------------
QString VHDLimport::getVendor() const
{
    return tr("TUT");
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::getLicense()
//-----------------------------------------------------------------------------
QString VHDLimport::getLicense() const
{
    return tr("GPL2");
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::getLicenseHolder()
//-----------------------------------------------------------------------------
QString VHDLimport::getLicenseHolder() const
{
    return tr("Public");
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::getSettingsWidget()
//-----------------------------------------------------------------------------
QWidget* VHDLimport::getSettingsWidget()
{
    return nullptr;
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::getProgramRequirements()
//-----------------------------------------------------------------------------
QList<IPlugin::ExternalProgramRequirement> VHDLimport::getProgramRequirements()
{
    return QList<IPlugin::ExternalProgramRequirement>();
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::acceptedFileTypes()
//-----------------------------------------------------------------------------
QStringList VHDLimport::getSupportedFileTypes() const
{
    QStringList filetypes;
    filetypes << "vhdlSource" << "vhdlSource-87" << "vhdlSource-93";

    return filetypes;
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::getCompatibilityWarnings()
//-----------------------------------------------------------------------------
QString VHDLimport::getCompatibilityWarnings() const
{
    return QString();
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::parseFile()
//-----------------------------------------------------------------------------
void VHDLimport::import(QString const& input, QString const& componentDeclaration,
    QSharedPointer<Component> targetComponent)
{
    targetComponent_ = targetComponent;
 
    if (hasValidEntity(input))
    {
        highlightEntity(componentDeclaration);

		QSharedPointer<ComponentInstantiation> targetComponentInstantiation = setupComponentInstantiation();
        parseModelNameAndArchitecture(input, componentDeclaration, targetComponentInstantiation);

        genericParser_->import(componentDeclaration, targetComponent, targetComponentInstantiation);
        portParser_->import(componentDeclaration, targetComponent, targetComponentInstantiation);
    }
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::getFileComponents()
//-----------------------------------------------------------------------------
QStringList VHDLimport::getFileComponents(QString const& input) const
{
    QStringList fileEntities;

    QRegularExpressionMatchIterator matchIterator = ENTITY_EXP.globalMatch(input);
    while (matchIterator.hasNext())
    {
        QRegularExpressionMatch match = matchIterator.next();
        if (match.hasMatch())
        {
            fileEntities.append(match.captured());
        }
    }

    return fileEntities;
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::getComponentName()
//-----------------------------------------------------------------------------
QString VHDLimport::getComponentName(QString const& componentDeclaration) const
{
    QRegularExpressionMatch match = ENTITYNAME_EXP.match(componentDeclaration);
    return match.captured(1);
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::setHighlighter()
//-----------------------------------------------------------------------------
void VHDLimport::setHighlighter(Highlighter* highlighter)
{
    highlighter_ = highlighter;

    portParser_->setHighlighter(highlighter);
    genericParser_->setHighlighter(highlighter);
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::setModelParameterVisualizer()
//-----------------------------------------------------------------------------
void VHDLimport::setModelParameterVisualizer(ModelParameterVisualizer* visualizer)
{
    visualizer->registerChangeListener(this);
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::hasValidEntity()
//-----------------------------------------------------------------------------
bool VHDLimport::hasValidEntity(QString const& fileContent) const
{
    return fileContent.count(ENTITY_EXP) > 0;
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::highlightEntity()
//-----------------------------------------------------------------------------
void VHDLimport::highlightEntity(QString const& fileContent) const
{
    if (highlighter_)
    {
        QRegularExpressionMatch match = ENTITY_EXP.match(fileContent);
        highlighter_->applyFontColor(match.captured(), Qt::black);
    }
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::parseModelNameAndArchitecture()
//-----------------------------------------------------------------------------
void VHDLimport::parseModelNameAndArchitecture(QString const& input, QString const& componentDeclaration,
    QSharedPointer<ComponentInstantiation> targetInstantiation) const
{
    QRegularExpressionMatch match = ENTITY_EXP.match(componentDeclaration);
    QString entityName = match.captured(1);

    QRegularExpression architectureExp("ARCHITECTURE\\s+((\\w+)\\s+OF\\s+(" + entityName + "))\\s+IS(?=\\s+)", 
        QRegularExpression::CaseInsensitiveOption);

    QRegularExpression configurationExp("CONFIGURATION\\s+((\\w+)\\s+OF\\s+" + entityName + ")\\s+IS(?=\\s+)", 
        QRegularExpression::CaseInsensitiveOption);

    QRegularExpressionMatch architectureMatch = architectureExp.match(input);
    QRegularExpressionMatch configurationMatch = configurationExp.match(input);

    QString modelName = entityName;
    QString architecture = QString();

    if (architectureMatch.hasMatch())
    {
        architecture = architectureMatch.captured(2);
        modelName = architectureMatch.captured(3);        
        highlightArchitecture(architectureMatch);
    }
    else if(configurationMatch.hasMatch())
    {
        modelName = configurationMatch.captured(2);
        highlightConfiguration(configurationMatch);
    }

    targetInstantiation->setModuleName(modelName); 
    targetInstantiation->setArchitectureName(architecture);
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::highlightArchitecture()
//-----------------------------------------------------------------------------
void VHDLimport::highlightArchitecture(QRegularExpressionMatch const& architectureExp) const
{
    if (highlighter_)
    {
        highlighter_->applyFontColor(architectureExp.captured(0), Qt::black);
        highlighter_->applyHighlight(architectureExp.captured(1), ImportColors::VIEWNAME);
    }
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::highlightConfiguration()
//-----------------------------------------------------------------------------
void VHDLimport::highlightConfiguration(QRegularExpressionMatch const& configurationExp) const
{
    if (highlighter_)
    {
        highlighter_->applyFontColor(configurationExp.captured(0), Qt::black);
        highlighter_->applyHighlight(configurationExp.captured(2), ImportColors::VIEWNAME);
    }
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::findOrCreateFlatView()
//-----------------------------------------------------------------------------
QSharedPointer<View> VHDLimport::findOrCreateFlatView() const
{
    QStringList flatViews = targetComponent_->getFlatViews();
    if (flatViews.isEmpty())
    {
		QSharedPointer<View> newView(new View());
		newView->setName(NameGenerationPolicy::flatViewName("vhdl"));
		targetComponent_->getViews()->append(newView);

		flatViews.append(newView->name());
    }

    return targetComponent_->getModel()->findView(flatViews.first());
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::setupComponentInstantiation()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentInstantiation> VHDLimport::setupComponentInstantiation() const
{
    QSharedPointer<View> rtlView = findOrCreateFlatView();

	// Create environment identifiers for the view as needed.
	QSharedPointer<View::EnvironmentIdentifier> importIdentifier(new View::EnvironmentIdentifier);
	importIdentifier->language = QStringLiteral("VHDL");
	importIdentifier->tool = QStringLiteral("Kactus2");

	if (!rtlView->hasEnvIdentifier(importIdentifier))
	{
		rtlView->getEnvIdentifiers()->append(importIdentifier);
	}

	// Must have a component instantiation for module parameters.
	QString instanceName = NameGenerationPolicy::componentInstantiationName("vhdl");
	QSharedPointer<ComponentInstantiation> targetComponentInstantiation =
		targetComponent_->getModel()->findComponentInstantiation(instanceName);

	// Create and add to the component if does not exist.
	if (!targetComponentInstantiation)
	{
		targetComponentInstantiation = QSharedPointer<ComponentInstantiation>(new ComponentInstantiation());
		targetComponentInstantiation->setName(instanceName);
		targetComponentInstantiation->setLanguage("VHDL");

		targetComponent_->getComponentInstantiations()->append(targetComponentInstantiation);
	}

	rtlView->setComponentInstantiationRef(instanceName);

	return targetComponentInstantiation;
}
