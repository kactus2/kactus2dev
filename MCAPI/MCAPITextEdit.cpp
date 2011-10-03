//-----------------------------------------------------------------------------
// File: MCAPITextEdit.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 21.2.2011
//
// Description:
// MCAPI text edit widget.
//-----------------------------------------------------------------------------

#include "MCAPITextEdit.h"

#include "MCAPIHighlighter.h"
#include "MCAPIContentMatcher.h"

//-----------------------------------------------------------------------------
// Function: MCAPITextEdit()
//-----------------------------------------------------------------------------
MCAPITextEdit::MCAPITextEdit(QSharedPointer<MCAPIContentMatcher> matcher,
                             QWidget* mainWnd, QWidget* parent /* = 0 */) :
    AssistedTextEdit(matcher, mainWnd, parent), m_highlighter(0), m_matcher(matcher)
{
    m_highlighter = new MCAPIHighlighter(document());
}

//-----------------------------------------------------------------------------
// Function: ~MCAPITextEdit()
//-----------------------------------------------------------------------------
MCAPITextEdit::~MCAPITextEdit()
{
}

//-----------------------------------------------------------------------------
// Function: getMatcher()
//-----------------------------------------------------------------------------
MCAPIContentMatcher& MCAPITextEdit::getMatcher()
{
    return *m_matcher;
}

//-----------------------------------------------------------------------------
// Function: getHighlighter()
//-----------------------------------------------------------------------------
MCAPIHighlighter& MCAPITextEdit::getHighlighter()
{
    return *m_highlighter;
}
