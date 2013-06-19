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
    selectedGeneric_.setBackground(QBrush(KactusColors::MISSING_COMPONENT));

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

    case  INSIDE_ENTITY:
        {
            setFormat(startIndex, endIndex, insideEntity_);
            break;
        }
    case PORT_SELECTED:
        {
            setFormat(startIndex, endIndex, insideEntity_);
            break;
        }
    case PORT_NOT_SELECTED:
        {
            setFormat(startIndex, endIndex, notSelected_);
            break;
        }
    case GENERIC_SELECTED:
        {
            setFormat(startIndex, endIndex, selectedGeneric_);
            break;
        }
    case  GENERIC_NOT_SELECTED:
        {
            setFormat(startIndex, endIndex, notSelected_);
            break;
        }
    case OUTSIDE_ENTITY :
    default:
        {
            setFormat(startIndex, endIndex, outsideEntity_);
            break;
        }

    }
  
}