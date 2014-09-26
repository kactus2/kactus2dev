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