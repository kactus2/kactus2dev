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

#include <QRegExp>
#include <QList>

#include <Plugins/PluginSystem/ImportPlugin/ImportColors.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/common/ModuleParameter.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/View.h>

#include <Plugins/PluginSystem/ImportPlugin/Highlighter.h>
#include <Plugins/PluginSystem/ImportPlugin/ModelParameterVisualizer.h>

#include "VHDLPortParser.h"
#include "VHDLGenericParser.h"
#include "../common/NameGenerationPolicy.h"

namespace
{
    //!  Regual expression for VHDL entity.
    const QRegExp ENTITY_EXP = QRegExp("ENTITY\\s+(\\w+)\\s+IS\\s+.*END\\s+(?:ENTITY\\s+)?(\\1)?\\s*[;]", 
        Qt::CaseInsensitive);
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::VHDLimport()
//-----------------------------------------------------------------------------
VHDLimport::VHDLimport() : QObject(0),

    portParser_(new VHDLPortParser(this)),
    genericParser_(new VHDLGenericParser(this)),
    targetComponent_(0),        
    highlighter_(0),
    dependedGenerics_(),
    parsedPortDeclarations_()
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
    return "1.1";
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
// Function: VHDLimport::getLicence()
//-----------------------------------------------------------------------------
QString VHDLimport::getLicence() const
{
    return tr("GPL2");
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::getLicenceHolder()
//-----------------------------------------------------------------------------
QString VHDLimport::getLicenceHolder() const
{
    return tr("Public");
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::getSettingsWidget()
//-----------------------------------------------------------------------------
PluginSettingsWidget* VHDLimport::getSettingsWidget()
{
    return new PluginSettingsWidget();
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
    return tr("Warning: %1 version 1.1 and earlier will not preserve references "
        "to model parameters.").arg(getName());
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::parseFile()
//-----------------------------------------------------------------------------
void VHDLimport::import(QString const& input, QSharedPointer<Component> targetComponent)
{
    targetComponent_ = targetComponent;

    clear();

    if (hasValidEntity(input))
    {        
        highlightEntity(input);

		QSharedPointer<ComponentInstantiation> targetComponentInstantiation =
		setLanguageAndEnvironmentalIdentifiers();
        parseModelName(input, targetComponentInstantiation);

        genericParser_->import(input, targetComponent, targetComponentInstantiation);

        portParser_->import(input, targetComponent, targetComponentInstantiation);
    }
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::clear()
//-----------------------------------------------------------------------------
void VHDLimport::clear()
{
    removePreviouslyDependentGenerics();
    removePreviousPortDeclarations();    
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
// Function: VHDLimport::highlight()
//-----------------------------------------------------------------------------
void VHDLimport::highlight(QString const& text, QColor const& highlightColor) const
{
    if (highlighter_)
    {
        highlighter_->applyHighlight(text, highlightColor);
    }
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::removePreviousPortDeclarations()
//-----------------------------------------------------------------------------
void VHDLimport::removePreviousPortDeclarations()
{
    parsedPortDeclarations_.clear();
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::removePreviouslyDependentGenerics()
//-----------------------------------------------------------------------------
void VHDLimport::removePreviouslyDependentGenerics()
{
    dependedGenerics_.clear();
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::hasValidEntity()
//-----------------------------------------------------------------------------
bool VHDLimport::hasValidEntity(QString const& fileContent) const
{
    int entityStartIndex = ENTITY_EXP.indexIn(fileContent);
    bool hasEntity = entityStartIndex != -1;
    bool hasOnlyOneEntity = entityStartIndex == ENTITY_EXP.lastIndexIn(fileContent);

    return hasEntity && hasOnlyOneEntity;
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::highlightEntity()
//-----------------------------------------------------------------------------
void VHDLimport::highlightEntity(QString const& fileContent) const
{
    if (highlighter_)
    {
        ENTITY_EXP.indexIn(fileContent);
        highlighter_->applyFontColor(ENTITY_EXP.cap(0), QColor("black"));
    }   
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::parseModelName()
//-----------------------------------------------------------------------------
void VHDLimport::parseModelName(QString const& input, QSharedPointer<ComponentInstantiation> targetComponentInstantiation) const
{
    ENTITY_EXP.indexIn(input);
    QString entityName = ENTITY_EXP.cap(1);

    QRegExp architectureExp("ARCHITECTURE\\s+((\\w+)\\s+OF\\s+(" + entityName + "))\\s+IS(?=\\s+)", 
        Qt::CaseInsensitive);

    QRegExp configurationExp("CONFIGURATION\\s+((\\w+)\\s+OF\\s+" + entityName + ")\\s+IS(?=\\s+)", 
        Qt::CaseInsensitive);

   QString modelName = "";

   if (hasArchitecture(architectureExp, input))
    {
        modelName = createModelNameFromArchitecture(architectureExp);
        highlightArchitecture(architectureExp);
    }
   else if(hasConfiguration(configurationExp, input))
    {
        modelName = createModelNameFromConfiguration(configurationExp);
        highlightConfiguration(configurationExp);
    }

	targetComponentInstantiation->setModuleName(modelName); 
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::hasArchitecture()
//-----------------------------------------------------------------------------
bool VHDLimport::hasArchitecture(QRegExp const& architectureExp, QString const& input) const
{
    return architectureExp.indexIn(input) != -1;
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::createModelNameForArchitecture()
//-----------------------------------------------------------------------------
QString VHDLimport::createModelNameFromArchitecture(QRegExp const& architectureExp) const
{
    QString entityName = architectureExp.cap(3);
    QString architectureName = architectureExp.cap(2);

    return entityName + "(" + architectureName + ")";	
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::highlightArchitecture()
//-----------------------------------------------------------------------------
void VHDLimport::highlightArchitecture(QRegExp const& architectureExp) const
{
    if (highlighter_)
    {
        highlighter_->applyFontColor(architectureExp.cap(0), QColor("black"));
        highlighter_->applyHighlight(architectureExp.cap(1), ImportColors::VIEWNAME);
    }
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::hasConfiguration()
//-----------------------------------------------------------------------------
bool VHDLimport::hasConfiguration(QRegExp const& configurationExp, QString const& input) const
{
    return configurationExp.indexIn(input) != -1;
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::createModelNameFromConfiguration()
//-----------------------------------------------------------------------------
QString VHDLimport::createModelNameFromConfiguration(QRegExp const& configurationExp) const
{
    QString configurationName = configurationExp.cap(2);
    return configurationName;
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::highlightConfiguration()
//-----------------------------------------------------------------------------
void VHDLimport::highlightConfiguration(QRegExp const& configurationExp) const
{
    if (highlighter_)
    {
        highlighter_->applyFontColor(configurationExp.cap(0), QColor("black"));
        highlighter_->applyHighlight(configurationExp.cap(2), ImportColors::VIEWNAME);
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
		// create new view
		QSharedPointer<View> newView( new View() );
		newView->setName("flat");
		newView->addEnvIdentifier(QString("::"));
		targetComponent_->getViews()->append(newView);

		flatViews = targetComponent_->getFlatViews();
    }

    return targetComponent_->getModel()->findView(flatViews.first());
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::setLanguageAndEnvironmentalIdentifiers()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentInstantiation> VHDLimport::setLanguageAndEnvironmentalIdentifiers() const
{
    QSharedPointer<View> rtlView = findOrCreateFlatView();

    QString createdEnvIdentifier = "VHDL:Kactus2:";

    QStringList envIdentifiers = rtlView->getEnvIdentifiers();

    if (envIdentifiers.contains("::"))
    {
        envIdentifiers.replace(envIdentifiers.indexOf("::"), createdEnvIdentifier);
    }
    else if (!envIdentifiers.contains(createdEnvIdentifier, Qt::CaseInsensitive))
    {
        envIdentifiers.append(createdEnvIdentifier);
    }

	rtlView->setEnvIdentifiers(envIdentifiers);    

	// Must have a component instantiation for module parameters.
	QString instaName = NameGenerationPolicy::vhdlComponentInstantiationName( rtlView->name() );
	QSharedPointer<ComponentInstantiation> targetComponentInstantiation =
		targetComponent_->getModel()->findComponentInstantiation(instaName);

	// Create and add to the component if does not exist.
	if ( !targetComponentInstantiation )
	{
		targetComponentInstantiation = QSharedPointer<ComponentInstantiation>( new ComponentInstantiation );
		targetComponentInstantiation->setName(instaName);
		targetComponentInstantiation->setLanguage("vhdl");
		targetComponent_->getComponentInstantiations()->append(targetComponentInstantiation);
	}

	rtlView->setComponentInstantiationRef( instaName );

	return targetComponentInstantiation;
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::addDependencyOfGenericToPort()
//-----------------------------------------------------------------------------
void VHDLimport::addDependencyOfGenericToPort(QSharedPointer<ModuleParameter> modelParameter, 
    QSharedPointer<Port> parsedPort)
{
    QList<QSharedPointer<Port> > portList = dependedGenerics_.value(modelParameter);

    if (!portList.contains(parsedPort))
    {
        portList.append(parsedPort);
    }

    dependedGenerics_.insert(modelParameter, portList);
}