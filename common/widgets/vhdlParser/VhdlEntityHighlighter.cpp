//-----------------------------------------------------------------------------
// File: VhdlEntityHighlighter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 13.06.2013
//
// Description:
// Highlights the entity section in an vhdl text.
//-----------------------------------------------------------------------------
#include "VhdlEntityHighlighter.h"

#include <QFile>
#include <QFont>
#include <QTextCursor>
#include <QTextEdit>
#include <QMessageBox>
#include <QTextStream>
#include <QScrollBar>
#include <QRegExp>
#include <QDebug>

#include <models/modelparameter.h>
#include <models/port.h>
#include <common/KactusColors.h>
#include <common/widgets/vhdlParser/VhdlSyntax.h>

//-----------------------------------------------------------------------------
// Function: VhdlEntityHighlighter()
//-----------------------------------------------------------------------------
VhdlEntityHighlighter::VhdlEntityHighlighter(QTextDocument* document) : 
QSyntaxHighlighter( document ), insideEntity_(), outsideEntity_(),
selectedPort_(), selectedGeneric_(), notSelected_()
{
    insideEntity_.setForeground(QColor("black"));

    outsideEntity_.setForeground(QColor("gray"));

    selectedPort_.setForeground(QColor("black"));
    selectedPort_.setBackground(QBrush(KactusColors::SW_COMPONENT));

    selectedGeneric_.setForeground(QColor("black"));
    selectedGeneric_.setBackground(QBrush(KactusColors::HW_BUS_COMPONENT));

    notSelected_.setForeground(QColor("black"));
    notSelected_.setBackground(QBrush(KactusColors::MISSING_COMPONENT));

}

//-----------------------------------------------------------------------------
// Function: ~VhdlEntityHighlighter()
//-----------------------------------------------------------------------------
VhdlEntityHighlighter::~VhdlEntityHighlighter()
{
}

//-----------------------------------------------------------------------------
// Function: highlightBlock()
//-----------------------------------------------------------------------------
void VhdlEntityHighlighter::highlightBlock(const QString &text)
{
    int startIndex = 0;
    int endIndex = text.length();
 
   switch ( currentBlockState() ){

    case  BlockStyle::INSIDE_ENTITY:
        {
            setFormat(startIndex, endIndex, insideEntity_);
            break;
        }
    case  BlockStyle::PORT_SELECTED:
        {
            startIndex = VhdlSyntax::PORT_EXP.indexIn(text);
            endIndex = startIndex + VhdlSyntax::PORT_EXP.matchedLength();
            setFormat(startIndex, endIndex, selectedPort_);
            break;
        }
    case  BlockStyle::PORT_NOT_SELECTED:
        {
            startIndex = VhdlSyntax::PORT_EXP.indexIn(text);
            endIndex = startIndex + VhdlSyntax::PORT_EXP.matchedLength();
            setFormat(startIndex, endIndex, notSelected_);
            break;
        }
    case  BlockStyle::GENERIC_SELECTED:
        {
            startIndex = VhdlSyntax::GENERIC_EXP.indexIn(text);
            endIndex = startIndex + VhdlSyntax::GENERIC_EXP.matchedLength();
            setFormat(startIndex, endIndex, selectedGeneric_);
            break;
        }
    case   BlockStyle::GENERIC_NOT_SELECTED:
        {
            startIndex = VhdlSyntax::GENERIC_EXP.indexIn(text);
            endIndex = startIndex + VhdlSyntax::GENERIC_EXP.matchedLength();
            setFormat(startIndex, endIndex, notSelected_);
            break;
        }
    case  BlockStyle::OUTSIDE_ENTITY :
    default:
        {
            setFormat(startIndex, endIndex, outsideEntity_);
            break;
        }

    }
  
}