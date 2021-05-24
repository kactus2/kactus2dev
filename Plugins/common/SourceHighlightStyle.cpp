//-----------------------------------------------------------------------------
// File: SourceHighlightStyle.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 15.02.2017
//
// Description:
// Base class for language-specific highlighters.
//-----------------------------------------------------------------------------

#include "SourceHighlightStyle.h"

//-----------------------------------------------------------------------------
// Function: SourceHighlightStyle::SourceHighlightStyle()
//-----------------------------------------------------------------------------
SourceHighlightStyle::SourceHighlightStyle()
{
    
}

//-----------------------------------------------------------------------------
// Function: SourceHighlightStyle::apply()
//-----------------------------------------------------------------------------
void SourceHighlightStyle::apply(LanguageHighlighter* highlighter) const
{
    foreach(QString const& pattern, keywords())
    {
        highlighter->addRule(QRegularExpression("\\b" + pattern + "\\b"), LanguageHighlighter::KEYWORD);
    }

    foreach(QString const& pattern, preprocessorDirectives())
    {
        highlighter->addRule(QRegularExpression(pattern), LanguageHighlighter::PREPROCESSOR);
    }

    QRegularExpression pattern = QRegularExpression("\"([^\"]|(\\\\\"))*\"");
    highlighter->addRule(pattern, LanguageHighlighter::STRING);

    pattern = QRegularExpression("\'([^\']|(\\\\\'))*\'");

    highlighter->addRule(pattern, LanguageHighlighter::STRING);

    pattern = QRegularExpression("//[^\n]*");
    highlighter->addRule(pattern, LanguageHighlighter::SINGLE_LINE_COMMENT);

    applyLanguageSpecificRules(highlighter);
}

//-----------------------------------------------------------------------------
// Function: SourceHighlightStyle::applyLanguageSpecificRules()
//-----------------------------------------------------------------------------
void SourceHighlightStyle::applyLanguageSpecificRules(LanguageHighlighter* /*highlighter*/) const
{
    //! Nothing to do in base class.
}
