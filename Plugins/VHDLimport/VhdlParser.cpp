//-----------------------------------------------------------------------------
// File: VhdlParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.06.2013
//
// Description:
// VhdlParser reads a given input, creates ports and generics accordingly
// and highlights the created ports and generics. 
//-----------------------------------------------------------------------------

#include "VhdlParser.h"

#include <QRegExp>
#include <QList>

#include <common/KactusColors.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/modelparameter.h>
#include <IPXACTmodels/port.h>

#include <Plugins/PluginSystem/Highlighter.h>
#include <Plugins/PluginSystem/PortVisualizer.h>
#include <Plugins/PluginSystem/ModelParameterVisualizer.h>

#include "VHDLPortParser.h"
#include "VHDLGenericParser.h"

namespace
{
    //!  Regual expression for VHDL entity.
    const QRegExp ENTITY_EXP = QRegExp("ENTITY\\s+(\\w+)\\s+IS\\s+.*END\\s+(?:ENTITY\\s+)?(\\1)?\\s*[;]", 
        Qt::CaseInsensitive);
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::VhdlParser()
//-----------------------------------------------------------------------------
VhdlParser::VhdlParser() : QObject(0),

    portParser_(new VHDLPortParser(this)),
    genericParser_(new VHDLGenericParser(this)),
    targetComponent_(0),        
    highlighter_(0),
    dependedGenerics_(),
    parsedPortDeclarations_()
{
    connect(portParser_, SIGNAL(add(QSharedPointer<Port>, QString const&)), 
        this, SLOT(onPortParsed(QSharedPointer<Port>, QString const&)), Qt::UniqueConnection);
    connect(portParser_, SIGNAL(highlight(QString const&, QColor const&)), 
        this, SLOT(highlight(QString const&, QColor const&)), Qt::UniqueConnection);

    connect(genericParser_, SIGNAL(add(QSharedPointer<ModelParameter>)), 
        this, SLOT(onGenericParsed(QSharedPointer<ModelParameter>)), Qt::UniqueConnection);
    connect(genericParser_, SIGNAL(highlight(QString const&, QColor const&)), 
        this, SLOT(highlight(QString const&, QColor const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::~VhdlParser()
//-----------------------------------------------------------------------------
VhdlParser::~VhdlParser()
{

}

//-----------------------------------------------------------------------------
// Function: VhdlParser::getName()
//-----------------------------------------------------------------------------
QString const& VhdlParser::getName() const
{
    static QString name(tr("VHDL importer"));
    return name;
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::getVersion()
//-----------------------------------------------------------------------------
QString const& VhdlParser::getVersion() const
{
    static QString version("1.0");
    return version;
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::getDescription()
//-----------------------------------------------------------------------------
QString const& VhdlParser::getDescription() const
{
    static QString description(tr("Imports model parameters and ports from a VHDL file."));
    return description;
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::getVendor()
//-----------------------------------------------------------------------------
QString const& VhdlParser::getVendor() const
{
    static QString vendor("TUT");
    return vendor;
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::getLicence()
//-----------------------------------------------------------------------------
QString const& VhdlParser::getLicence() const
{
    static QString licence(tr("GPL2"));
    return licence;
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::getLicenceHolder()
//-----------------------------------------------------------------------------
QString const& VhdlParser::getLicenceHolder() const
{
    static QString licenceHolder(tr("Public"));
    return licenceHolder;
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::getSettingsWidget()
//-----------------------------------------------------------------------------
PluginSettingsWidget* VhdlParser::getSettingsWidget()
{
    return new PluginSettingsWidget();
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::getProgramRequirements()
//-----------------------------------------------------------------------------
QList<IPlugin::ExternalProgramRequirement> VhdlParser::getProgramRequirements()
{
    return QList<IPlugin::ExternalProgramRequirement>();
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::acceptedFileTypes()
//-----------------------------------------------------------------------------
QStringList VhdlParser::acceptedFileTypes() const
{
    QStringList filetypes;
    filetypes << "vhdlSource";

    return filetypes;
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::parseFile()
//-----------------------------------------------------------------------------
void VhdlParser::runParser(QString const& input, QSharedPointer<Component> targetComponent)
{
    targetComponent_ = targetComponent;

    clear();

    grayOutFileContent(input);

    if (hasValidEntity(input))
    {        
        highlightEntity(input);

        genericParser_->runParser(input, targetComponent);
        portParser_->runParser(input, targetComponent);
    }
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::clear()
//-----------------------------------------------------------------------------
void VhdlParser::clear()
{
    removePreviousGenerics();
    removePreviousPorts();
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::setHighlighter()
//-----------------------------------------------------------------------------
void VhdlParser::setHighlighter(Highlighter* highlighter)
{
    highlighter_ = highlighter;

    portParser_->setHighlighter(highlighter);
    genericParser_->setHighlighter(highlighter);
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::setPortVisualizer()
//-----------------------------------------------------------------------------
void VhdlParser::setPortVisualizer(PortVisualizer* visualizer)
{
    portParser_->setPortVisualizer(visualizer);
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::setModelParameterVisualizer()
//-----------------------------------------------------------------------------
void VhdlParser::setModelParameterVisualizer(ModelParameterVisualizer* visualizer)
{
    genericParser_->setModelParameterVisualizer(visualizer);

    visualizer->registerChangeListener(this);
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::onPortParsed()
//-----------------------------------------------------------------------------
void VhdlParser::onPortParsed(QSharedPointer<Port> parsedPort, QString const& declaration)
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
// Function: VhdlParser::highlight()
//-----------------------------------------------------------------------------
void VhdlParser::highlight(QString const& text, QColor const& highlightColor) const
{
    if (highlighter_)
    {
        highlighter_->applyHighlight(text, highlightColor);
    }
}


//-----------------------------------------------------------------------------
// Function: VhdlParser::editorChangedModelParameter()
//-----------------------------------------------------------------------------
void VhdlParser::onModelParameterChanged(QSharedPointer<ModelParameter> changedParameter) const
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
// Function: VhdlParser::removePorts()
//-----------------------------------------------------------------------------
void VhdlParser::removePreviousPorts()
{
    portParser_->removePreviousPorts();

    parsedPortDeclarations_.clear();
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::removeGenerics()
//-----------------------------------------------------------------------------
void VhdlParser::removePreviousGenerics()
{
    genericParser_->removePreviousGenerics();

    dependedGenerics_.clear();
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::hasValidEntity()
//-----------------------------------------------------------------------------
bool VhdlParser::hasValidEntity(QString const& fileContent) const
{
    int entityStartIndex = ENTITY_EXP.indexIn(fileContent);
    bool hasEntity = entityStartIndex != -1;
    bool hasOnlyOneEntity = entityStartIndex == ENTITY_EXP.lastIndexIn(fileContent);

    return hasEntity && hasOnlyOneEntity;
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::VhdlParser::grayOutFileContent()
//-----------------------------------------------------------------------------
void VhdlParser::grayOutFileContent(QString const& fileContent) const
{    
    if (highlighter_)
    {
        highlighter_->applyFontColor(fileContent, QColor("gray"));
    }    
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::highlightEntity()
//-----------------------------------------------------------------------------
void VhdlParser::highlightEntity(QString const& fileContent) const
{
    if (highlighter_)
    {
        ENTITY_EXP.indexIn(fileContent);
        highlighter_->applyFontColor(ENTITY_EXP.cap(0), QColor("black"));
    }   
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::addDependencyOfGenericToPort()
//-----------------------------------------------------------------------------
void VhdlParser::addDependencyOfGenericToPort(QSharedPointer<ModelParameter> modelParameter, 
    QSharedPointer<Port> parsedPort)
{
    QList<QSharedPointer<Port> > portList = dependedGenerics_.value(modelParameter);

    if (!portList.contains(parsedPort))
    {
        portList.append(parsedPort);
    }

    dependedGenerics_.insert(modelParameter, portList);
}
