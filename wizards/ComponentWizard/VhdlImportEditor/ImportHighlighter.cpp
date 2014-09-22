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

#include <Plugins/PluginSystem/HighlightSource.h>

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
void ImportHighlighter::applyHighlight(QString const& text, QColor const& highlightColor)
{
    int beginIndex = display_->toPlainText().indexOf(text);
    if (beginIndex != -1)
    {
        QTextCharFormat highlighFormat;
        highlighFormat.setBackground(QBrush(highlightColor));

        QTextCursor cursor = display_->textCursor();
        cursor.setPosition(beginIndex);
        cursor.setPosition(beginIndex + text.length(), QTextCursor::KeepAnchor);
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