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

#include "CSourceHighlighter.h"
#include "CSourceContentMatcher.h"

//-----------------------------------------------------------------------------
// Function: CSourceTextEdit()
//-----------------------------------------------------------------------------
CSourceTextEdit::CSourceTextEdit(QWidget* mainWnd, QWidget* parent /* = 0 */)
    : AssistedTextEdit(QSharedPointer<CSourceContentMatcher>(new CSourceContentMatcher()), mainWnd, parent),
      m_highlighter(new CSourceHighlighter(document()))
{
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
CSourceContentMatcher& CSourceTextEdit::getMatcher()
{
    return static_cast<CSourceContentMatcher&>(AssistedTextEdit::getMatcher());
}

//-----------------------------------------------------------------------------
// Function: getHighlighter()
//-----------------------------------------------------------------------------
CSourceHighlighter& CSourceTextEdit::getHighlighter()
{
    return *m_highlighter;
}
