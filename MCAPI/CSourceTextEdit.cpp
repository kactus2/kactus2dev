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

#include "MCAPIHighlighter.h"
#include "MCAPIContentMatcher.h"

//-----------------------------------------------------------------------------
// Function: CSourceTextEdit()
//-----------------------------------------------------------------------------
CSourceTextEdit::CSourceTextEdit(QSharedPointer<MCAPIContentMatcher> matcher,
                                 QWidget* mainWnd, QWidget* parent /* = 0 */) :
    AssistedTextEdit(matcher, mainWnd, parent), m_highlighter(0), m_matcher(matcher)
{
    m_highlighter = new MCAPIHighlighter(document());
}

//-----------------------------------------------------------------------------
// Function: ~CSourceTextEdit()
//-----------------------------------------------------------------------------
CSourceTextEdit::~CSourceTextEdit()
{
}

//-----------------------------------------------------------------------------
// Function: getMatcher()
//-----------------------------------------------------------------------------
MCAPIContentMatcher& CSourceTextEdit::getMatcher()
{
    return *m_matcher;
}

//-----------------------------------------------------------------------------
// Function: getHighlighter()
//-----------------------------------------------------------------------------
MCAPIHighlighter& CSourceTextEdit::getHighlighter()
{
    return *m_highlighter;
}
