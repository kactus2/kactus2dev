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

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/modelparameter.h>
#include <IPXACTmodels/port.h>
#include <IPXACTmodels/view.h>

#include <Plugins/PluginSystem/ImportPlugin/Highlighter.h>
#include <Plugins/PluginSystem/ImportPlugin/ModelParameterVisualizer.h>

#include "VHDLPortParser.h"
#include "VHDLGenericParser.h"

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
    connect(portParser_, SIGNAL(add(QSharedPointer<Port>, QString const&)), 
        this, SLOT(onPortParsed(QSharedPointer<Port>, QString const&)), Qt::UniqueConnection);
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
QString const& VHDLimport::getName() const
{
    static QString name(tr("VHDL importer"));
    return name;
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::getVersion()
//-----------------------------------------------------------------------------
QString const& VHDLimport::getVersion() const
{
    static QString version("1.0");
    return version;
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::getDescription()
//-----------------------------------------------------------------------------
QString const& VHDLimport::getDescription() const
{
    static QString description(tr("Imports model parameters and ports from a VHDL file."));
    return description;
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::getVendor()
//-----------------------------------------------------------------------------
QString const& VHDLimport::getVendor() const
{
    static QString vendor("TUT");
    return vendor;
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::getLicence()
//-----------------------------------------------------------------------------
QString const& VHDLimport::getLicence() const
{
    static QString licence(tr("GPL2"));
    return licence;
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::getLicenceHolder()
//-----------------------------------------------------------------------------
QString const& VHDLimport::getLicenceHolder() const
{
    static QString licenceHolder(tr("Public"));
    return licenceHolder;
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
// Function: VHDLimport::parseFile()
//-----------------------------------------------------------------------------
void VHDLimport::import(QString const& input, QSharedPointer<Component> targetComponent)
{
    targetComponent_ = targetComponent;

    clear();

    grayOutFileContent(input);

    if (hasValidEntity(input))
    {        
        highlightEntity(input);

        parseModelName(input);
        setLanguageAndEnvironmentalIdentifiers();

        targetComponent->getModelParameters().clear();
        genericParser_->import(input, targetComponent);

        targetComponent->getPorts().clear();
        portParser_->import(input, targetComponent);
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
// Function: VHDLimport::onPortParsed()
//-----------------------------------------------------------------------------
void VHDLimport::onPortParsed(QSharedPointer<Port> parsedPort, QString const& declaration)
{
    parsedPortDeclarations_.insert(parsedPort, declaration);  

    foreach(QSharedPointer<ModelParameter> modelParameter, targetComponent_->getModelParameters())
    {
        if (declaration.contains(modelParameter->getName()))
        {
            addDependencyOfGenericToPort(modelParameter, parsedPort);
        }
    }
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
// Function: VHDLimport::editorChangedModelParameter()
//-----------------------------------------------------------------------------
void VHDLimport::onModelParameterChanged(QSharedPointer<ModelParameter> changedParameter) const
{
    foreach(QSharedPointer<Port> affectedPort, dependedGenerics_.value(changedParameter))
    {
        QString portDeclaration = parsedPortDeclarations_.value(affectedPort);

        affectedPort->setLeftBound(portParser_->parseLeftBound(portDeclaration, targetComponent_));
        affectedPort->setRightBound(portParser_->parseRightBound(portDeclaration, targetComponent_));
        affectedPort->setDefaultValue(portParser_->parseDefaultValue(portDeclaration));
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
// Function: VHDLimport::VHDLimport::grayOutFileContent()
//-----------------------------------------------------------------------------
void VHDLimport::grayOutFileContent(QString const& fileContent) const
{    
    if (highlighter_)
    {
        highlighter_->applyFontColor(fileContent, QColor("gray"));
    }    
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
void VHDLimport::parseModelName(QString const& input) const
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
    
    View* rtlView = findOrCreateFlatView();
    rtlView->setModelName(modelName);    
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
View* VHDLimport::findOrCreateFlatView() const
{
    QStringList flatViews = targetComponent_->getFlatViews();
    if (flatViews.isEmpty())
    {
        targetComponent_->createEmptyFlatView();
        flatViews = targetComponent_->getFlatViews();
    }

    return targetComponent_->findView(flatViews.first());
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::setLanguageAndEnvironmentalIdentifiers()
//-----------------------------------------------------------------------------
void VHDLimport::setLanguageAndEnvironmentalIdentifiers() const
{
    View* rtlView = findOrCreateFlatView();
    rtlView->setLanguage("vhdl");

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
}

//-----------------------------------------------------------------------------
// Function: VHDLimport::addDependencyOfGenericToPort()
//-----------------------------------------------------------------------------
void VHDLimport::addDependencyOfGenericToPort(QSharedPointer<ModelParameter> modelParameter, 
    QSharedPointer<Port> parsedPort)
{
    QList<QSharedPointer<Port> > portList = dependedGenerics_.value(modelParameter);

    if (!portList.contains(parsedPort))
    {
        portList.append(parsedPort);
    }

    dependedGenerics_.insert(modelParameter, portList);
}