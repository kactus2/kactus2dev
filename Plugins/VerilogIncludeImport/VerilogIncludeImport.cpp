//-----------------------------------------------------------------------------
// File: VerilogIncludeImport.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.03.2015
//
// Description:
// Import plugin for Verilog definitions.
//-----------------------------------------------------------------------------

#include "VerilogIncludeImport.h"

#include <Plugins/VerilogImport/VerilogSyntax.h>
#include <Plugins/PluginSystem/ImportPlugin/ImportColors.h>

#include <IPXACTmodels/parameter.h>
#include <IPXACTmodels/component.h>

#include <QRegularExpression>

namespace
{
    QRegularExpression const DEFINE("`define (\\w+) (((\\\\\\\n)|(.*?))*)\\n");
}

//-----------------------------------------------------------------------------
// Function: VerilogIncludeImport::VerilogIncludeImport()
//-----------------------------------------------------------------------------
VerilogIncludeImport::VerilogIncludeImport() : QObject(0), highlighter_(0)
{

}

//-----------------------------------------------------------------------------
// Function: VerilogIncludeImport::~VerilogIncludeImport()
//-----------------------------------------------------------------------------
VerilogIncludeImport::~VerilogIncludeImport()
{

}

//-----------------------------------------------------------------------------
// Function: VerilogIncludeImport::getName()
//-----------------------------------------------------------------------------
QString const& VerilogIncludeImport::getName() const
{
    static QString name("Verilog include import");
    return name;
}

//-----------------------------------------------------------------------------
// Function: VerilogIncludeImport::getVersion()
//-----------------------------------------------------------------------------
QString const& VerilogIncludeImport::getVersion() const
{
    static QString version("1.0");
    return version;
}

//-----------------------------------------------------------------------------
// Function: VerilogIncludeImport::getDescription()
//-----------------------------------------------------------------------------
QString const& VerilogIncludeImport::getDescription() const
{
    static QString description("Import defines from a verilog include file.");
    return description;
}

//-----------------------------------------------------------------------------
// Function: VerilogIncludeImport::getVendor()
//-----------------------------------------------------------------------------
QString const& VerilogIncludeImport::getVendor() const
{
    static QString vendor("TUT");
    return vendor;
}

//-----------------------------------------------------------------------------
// Function: VerilogIncludeImport::getLicence()
//-----------------------------------------------------------------------------
QString const& VerilogIncludeImport::getLicence() const
{
    static QString licence("GPL2");
    return licence;
}

//-----------------------------------------------------------------------------
// Function: VerilogIncludeImport::getLicenceHolder()
//-----------------------------------------------------------------------------
QString const& VerilogIncludeImport::getLicenceHolder() const
{
    static QString licenceHolder(tr("Public"));
    return licenceHolder;
}

//-----------------------------------------------------------------------------
// Function: VerilogIncludeImport::getSettingsWidget()
//-----------------------------------------------------------------------------
PluginSettingsWidget* VerilogIncludeImport::getSettingsWidget()
{
    return new PluginSettingsWidget();
}

//-----------------------------------------------------------------------------
// Function: VerilogIncludeImport::getProgramRequirements()
//-----------------------------------------------------------------------------
QList<IPlugin::ExternalProgramRequirement> VerilogIncludeImport::getProgramRequirements()
{
    return QList<IPlugin::ExternalProgramRequirement>();
}

//-----------------------------------------------------------------------------
// Function: VerilogIncludeImport::getSupportedFileTypes()
//-----------------------------------------------------------------------------
QStringList VerilogIncludeImport::getSupportedFileTypes() const
{
    QStringList supportedTypes;
    supportedTypes << "verilogSource" << "verilogSource-95" << "verilogSource-2001";
    return supportedTypes;
}

//-----------------------------------------------------------------------------
// Function: VerilogIncludeImport::getCompatibilityWarnings()
//-----------------------------------------------------------------------------
QString VerilogIncludeImport::getCompatibilityWarnings() const
{
    return QString();
}

//-----------------------------------------------------------------------------
// Function: VerilogIncludeImport::import()
//-----------------------------------------------------------------------------
void VerilogIncludeImport::import(QString const& input, QSharedPointer<Component> targetComponent)
{
    QString nonCommentedInput = input;
    nonCommentedInput.remove(VerilogSyntax::MULTILINE_COMMENT);
    nonCommentedInput.remove(VerilogSyntax::COMMENTLINE);

    int position = 0;
    QRegularExpressionMatch match = DEFINE.match(nonCommentedInput, position);

    while (match.hasMatch())
    {
        QString definition = match.captured();

        highlightDefinition(definition);
        createParameterFromDefinition(definition, targetComponent);

        position = match.capturedStart() + match.capturedLength();
        match = DEFINE.match(nonCommentedInput, position);
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogIncludeImport::setHighlighter()
//-----------------------------------------------------------------------------
void VerilogIncludeImport::setHighlighter(Highlighter* highlighter)
{
    highlighter_ = highlighter;
}

//-----------------------------------------------------------------------------
// Function: VerilogIncludeImport::highlightDefinition()
//-----------------------------------------------------------------------------
void VerilogIncludeImport::highlightDefinition(QString const& definition)
{
    if (highlighter_)
    {     
        highlighter_->applyFontColor(definition, Qt::black);
        highlighter_->applyHighlight(definition, ImportColors::PARAMETER);
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogIncludeImport::createParameterFromDefinition()
//-----------------------------------------------------------------------------
void VerilogIncludeImport::createParameterFromDefinition(QString const& definition, 
    QSharedPointer<Component> targetComponent)
{
    QRegularExpressionMatch match = DEFINE.match(definition);

    QString name = match.captured(1);
    QString value = match.captured(2).remove(QRegularExpression("\\\\\\\n"));

    QSharedPointer<Parameter> parameter = findParameterByName(targetComponent, name);
    if (parameter.isNull())
    {
        parameter = QSharedPointer<Parameter>(new Parameter());                    
        parameter->setName(name);
        targetComponent->getParameters()->append(parameter);
    }

    parameter->setValue(value);
}

//-----------------------------------------------------------------------------
// Function: VerilogIncludeImport::findParameterByName()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> VerilogIncludeImport::findParameterByName(QSharedPointer<Component> targetComponent, 
    QString const& name) const
{
    QSharedPointer<Parameter> parameter;
    foreach (QSharedPointer<Parameter> existingParameter, *targetComponent->getParameters())
    {
        if (existingParameter->getName() == name)
        {
            return existingParameter;            
        }
    }	

    return parameter;
}
