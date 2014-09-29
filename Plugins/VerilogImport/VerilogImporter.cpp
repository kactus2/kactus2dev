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