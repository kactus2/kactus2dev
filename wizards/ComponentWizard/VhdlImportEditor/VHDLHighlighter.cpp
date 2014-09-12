//-----------------------------------------------------------------------------
// File: VHDLHighlighter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 11.09.2014
//
// Description:
// Source file highlighter for VHDL files.
//-----------------------------------------------------------------------------

#include "VHDLHighlighter.h"

#include <wizards/ComponentWizard/VhdlImportEditor/SourceFileDisplayer.h>

#include <common/KactusColors.h>
#include <common/widgets/vhdlParser/VhdlSyntax.h>

//-----------------------------------------------------------------------------
// Function: VHDLHighlighter::VHDLHighlighter()
//-----------------------------------------------------------------------------
VHDLHighlighter::VHDLHighlighter(SourceFileDisplayer* display, QObject *parent)
    : QObject(parent),
    display_(display),
    insideEntityFormat_(),
    outsideEntityFormat_(),
    selectedPortFormat_(),
    selectedGenericFormat_(),
    notSelectedFormat_()
{
    insideEntityFormat_.setForeground(QColor("black"));

    outsideEntityFormat_.setForeground(QColor("gray"));

    selectedPortFormat_.setForeground(QColor("black"));
    selectedPortFormat_.setBackground(QBrush(KactusColors::SW_COMPONENT));

    selectedGenericFormat_.setForeground(QColor("black"));
    selectedGenericFormat_.setBackground(QBrush(KactusColors::HW_BUS_COMPONENT));

    notSelectedFormat_.setForeground(QColor("black"));
    notSelectedFormat_.setBackground(QBrush(KactusColors::MISSING_COMPONENT));
}

//-----------------------------------------------------------------------------
// Function: VHDLHighlighter::~VHDLHighlighter()
//-----------------------------------------------------------------------------
VHDLHighlighter::~VHDLHighlighter()
{

}

//-----------------------------------------------------------------------------
// Function: VHDLHighlighter::unSelect()
//-----------------------------------------------------------------------------
void VHDLHighlighter::unSelect(int beginPos, int endPos) const
{
    QTextCursor cursor = display_->textCursor();
    cursor.setPosition(beginPos);
    cursor.setPosition(endPos, QTextCursor::KeepAnchor);
    cursor.setCharFormat(notSelectedFormat_);
}

//-----------------------------------------------------------------------------
// Function: VHDLHighlighter::selectPort()
//-----------------------------------------------------------------------------
void VHDLHighlighter::selectPort(int beginPos, int endPos) const
{
    QTextCursor cursor = display_->textCursor();
    cursor.setPosition(beginPos);
    cursor.setPosition(endPos, QTextCursor::KeepAnchor);
    cursor.setCharFormat(selectedPortFormat_);
}

//-----------------------------------------------------------------------------
// Function: VHDLHighlighter::selectModelParameter()
//-----------------------------------------------------------------------------
void VHDLHighlighter::selectModelParameter(int beginPos, int endPos) const
{
    QTextCursor cursor = display_->textCursor();
    cursor.setPosition(beginPos);
    cursor.setPosition(endPos, QTextCursor::KeepAnchor);
    cursor.setCharFormat(selectedGenericFormat_);
}

//-----------------------------------------------------------------------------
// Function: VHDLHighlighter::onSourceFileChanged()
//-----------------------------------------------------------------------------
void VHDLHighlighter::onSourceFileChanged() const
{
    QTextCursor cursor = display_->textCursor();   

    cursor.movePosition(QTextCursor::Start);
    cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    cursor.setCharFormat(outsideEntityFormat_);

    QString fileContent = display_->toPlainText();

    if (canHighlightEntity(fileContent))
    {
        const int entityBeginIndex = VhdlSyntax::ENTITY_BEGIN_EXP.indexIn(fileContent);  
        const int entityEndIndex = VhdlSyntax::ENTITY_END_EXP.indexIn(fileContent, entityBeginIndex);  
        const int entityLength = VhdlSyntax::ENTITY_END_EXP.matchedLength();

        cursor.setPosition(entityBeginIndex);
        cursor.setPosition(entityEndIndex + entityLength, QTextCursor::KeepAnchor);
        cursor.setCharFormat(insideEntityFormat_);
    }
}

//-----------------------------------------------------------------------------
// Function: canHighlightEntity()
//-----------------------------------------------------------------------------
bool VHDLHighlighter::canHighlightEntity(QString const& fileString) const
{
    QRegExp entityExp = QRegExp("ENTITY\\s+(\\w+)\\s+IS\\s+.*END\\s+(?:ENTITY\\s+)?(\\1)?\\s*[;]", 
        Qt::CaseInsensitive);

    int firstOccurrence = entityExp.indexIn(fileString);
    bool hasEntity = firstOccurrence != -1;
    bool hasOnlyOneEntity = firstOccurrence == entityExp.lastIndexIn(fileString);

    return hasEntity && hasOnlyOneEntity;
}