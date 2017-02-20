//-----------------------------------------------------------------------------
// File: CSourceTextEdit.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 21.2.2011
//
// Description:
// C source text edit widget.
//-----------------------------------------------------------------------------

#include "CSourceTextEdit.h"

#include "CSourceHighlight.h"
#include "CSourceHighlighter.h"
#include "CSourceContentMatcher.h"

//-----------------------------------------------------------------------------
// Function: CSourceTextEdit::CSourceTextEdit()
//-----------------------------------------------------------------------------
CSourceTextEdit::CSourceTextEdit(QWidget* mainWnd, QWidget* parent):
AssistedTextEdit(QSharedPointer<CSourceContentMatcher>(new CSourceContentMatcher()), mainWnd, parent),
    highlighter_(new CSourceHighlighter(document()))
{
    CSourceHighlight style;
    style.apply(highlighter_);
}

//-----------------------------------------------------------------------------
// Function: CSourceTextEdit::~CSourceTextEdit()
//-----------------------------------------------------------------------------
CSourceTextEdit::~CSourceTextEdit()
{
}

//-----------------------------------------------------------------------------
// Function: CSourceTextEdit::getMatcher()
//-----------------------------------------------------------------------------
CSourceContentMatcher& CSourceTextEdit::getMatcher()
{
    return static_cast<CSourceContentMatcher&>(AssistedTextEdit::getMatcher());
}

//-----------------------------------------------------------------------------
// Function: CSourceTextEdit::getHighlighter()
//-----------------------------------------------------------------------------
CSourceHighlighter& CSourceTextEdit::getHighlighter()
{
    return *highlighter_;
}
