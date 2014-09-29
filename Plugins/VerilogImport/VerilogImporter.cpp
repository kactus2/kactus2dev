//-----------------------------------------------------------------------------
// File: VerilogImporter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 23.09.2014
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#include "VerilogImporter.h"

#include "VerilogSyntax.h"

#include <IPXACTmodels/component.h>
#include <Plugins/PluginSystem/ImportPlugin/ImportColors.h>

#include <QString>

//-----------------------------------------------------------------------------
// Function: VerilogImporter::VerilogImporter()
//-----------------------------------------------------------------------------
VerilogImporter::VerilogImporter(): portParser_(), highlighter_(0)
{

}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::~VerilogImporter()
//-----------------------------------------------------------------------------
VerilogImporter::~VerilogImporter()
{

}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::runParser()
//-----------------------------------------------------------------------------
void VerilogImporter::runParser(QString const& input, QSharedPointer<Component> targetComponent)
{   
    highlightModule(input);
    portParser_.runParser(input, targetComponent);
    importModelName(input, targetComponent);
    setLanguageAndEnvironmentalIdentifiers(targetComponent);
    
}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::setHighlighter()
//-----------------------------------------------------------------------------
void VerilogImporter::setHighlighter(Highlighter* highlighter)
{
    highlighter_ = highlighter;

    portParser_.setHighlighter(highlighter);
}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::highlightModule()
//-----------------------------------------------------------------------------
void VerilogImporter::highlightModule(QString const& input)
{
    int moduleBegin = VerilogSyntax::MODULE_BEGIN.indexIn(input);
    int moduleEnd = VerilogSyntax::MODULE_END.indexIn(input) + VerilogSyntax::MODULE_END.matchedLength();

    if (highlighter_)
    {
        highlighter_->applyFontColor(input, QColor("gray"));
        highlighter_->applyFontColor(input.mid(moduleBegin, moduleEnd  - moduleBegin), QColor("black"));
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::importModelName()
//-----------------------------------------------------------------------------
void VerilogImporter::importModelName(QString const& input, QSharedPointer<Component> targetComponent)
{
    int moduleBegin = VerilogSyntax::MODULE_BEGIN.indexIn(input);
    if (moduleBegin != -1)
    {
        QString modelName = VerilogSyntax::MODULE_BEGIN.cap(1);

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

    QString createdEnvIdentifier = "verilog:Kactus2:";

    QStringList envIdentifiers = flatView->getEnvIdentifiers();

    if (envIdentifiers.isEmpty())
    {
        envIdentifiers.append(createdEnvIdentifier);
    }
    else if (envIdentifiers.first() == "::")
    {
        envIdentifiers.first() = createdEnvIdentifier;
    }
    else if (!envIdentifiers.contains(createdEnvIdentifier, Qt::CaseInsensitive))
    {
        envIdentifiers.append(createdEnvIdentifier);
    }     

    flatView->setEnvIdentifiers(envIdentifiers);
}
