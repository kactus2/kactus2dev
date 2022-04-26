//-----------------------------------------------------------------------------
// File: ImportHighlighter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 11.09.2014
//
// Description:
// Source file highlighter for import files.
//-----------------------------------------------------------------------------

#include "ImportHighlighter.h"

#include <KactusAPI/include/HighlightSource.h>

//-----------------------------------------------------------------------------
// Function: VHDLHighlighter::VHDLHighlighter()
//-----------------------------------------------------------------------------
ImportHighlighter::ImportHighlighter(QPlainTextEdit* display, QObject *parent)
    : QObject(parent),
    display_(display)
{

}

//-----------------------------------------------------------------------------
// Function: VHDLHighlighter::~VHDLHighlighter()
//-----------------------------------------------------------------------------
ImportHighlighter::~ImportHighlighter()
{

}

//-----------------------------------------------------------------------------
// Function: VHDLHighlighter::highlight()
//-----------------------------------------------------------------------------
void ImportHighlighter::applyHighlight(QString const& text, QColor const& highlightColor,
    QString const& subSection /* = "" */)
{
    QString displayText = display_->toPlainText();

    int beginIndex = 0;
    if (!subSection.isEmpty())
    {
        int subSectionIndex = displayText.indexOf(subSection);
        int textIndex = subSection.indexOf(text);
        beginIndex = subSectionIndex + textIndex;
    }
    else
    {
        beginIndex = displayText.indexOf(text);
    }

    int endIndex = beginIndex + text.length();

    applyHighlight(beginIndex, endIndex, highlightColor);
}

//-----------------------------------------------------------------------------
// Function: ImportHighlighter::applyHighlight()
//-----------------------------------------------------------------------------
void ImportHighlighter::applyHighlight(int beginPosition, int endPosition, QColor const& highlightColor)
{
    if (beginPosition != -1 && endPosition != -1 && endPosition <= display_->toPlainText().length())
    {
        QTextCursor cursor = display_->textCursor();
        cursor.setPosition(beginPosition);

        QTextCharFormat highlighFormat = cursor.charFormat();        
        highlighFormat.setBackground(QBrush(highlightColor));

        cursor.setPosition(endPosition, QTextCursor::KeepAnchor);        
        cursor.setCharFormat(highlighFormat);
    }
}

//-----------------------------------------------------------------------------
// Function: VHDLHighlighter::setFontColor()
//-----------------------------------------------------------------------------
void ImportHighlighter::applyFontColor(QString const& text, QColor const& color)
{
    int beginIndex = display_->toPlainText().indexOf(text);
    if (beginIndex != -1)
    {
        QTextCursor cursor = display_->textCursor();
        cursor.setPosition(beginIndex);
        cursor.setPosition(beginIndex + text.length(), QTextCursor::KeepAnchor);

        QTextCharFormat highlighFormat;
        highlighFormat.setForeground(QBrush(color));
        cursor.setCharFormat(highlighFormat);
    }
}