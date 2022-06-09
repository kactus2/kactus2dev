//-----------------------------------------------------------------------------
// File: PythonSourceHighlight.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 08.06.2022
//
// Description:
// Python language-specific highlight rules.
//-----------------------------------------------------------------------------

#include "PythonSourceHighlight.h"

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: PythonSourceHighlight::PythonSourceHighlight()
//-----------------------------------------------------------------------------
PythonSourceHighlight::PythonSourceHighlight():
    SourceHighlightStyle()
{
    
}

//-----------------------------------------------------------------------------
// Function: PythonSourceHighlight::keywords()
//-----------------------------------------------------------------------------
QStringList PythonSourceHighlight::keywords() const
{
    QStringList keywordPatterns{
        "and", "as", "assert",
        "break", "class", "continue",
        "def", "del", "elif",
        "else", "except", "False",
        "finally", "for", "from",
        "global", "if", "import",
        "in", "is", "lambda",
        "None", "nonlocal", "not",
        "or", "pass", "raise", 
        "return", "True", "try",
        "while", "with", "yield" };

    return keywordPatterns;
}

//-----------------------------------------------------------------------------
// Function: PythonSourceHighlight::preprocessorDirectives()
//-----------------------------------------------------------------------------
QStringList PythonSourceHighlight::preprocessorDirectives() const
{
    return QStringList();
}

//-----------------------------------------------------------------------------
// Function: PythonSourceHighlight::applyLanguageSpecificRules()
//-----------------------------------------------------------------------------
void PythonSourceHighlight::applyLanguageSpecificRules(LanguageHighlighter* highlighter) const
{
    auto stringPattern = QRegularExpression("[\"][^\\r\\n]*[\"]");
    highlighter->addRule(stringPattern, LanguageHighlighter::STRING);

    auto stringPattern2 = QRegularExpression("[\'][^\\r\\n]*[\']");
    highlighter->addRule(stringPattern2, LanguageHighlighter::STRING);

    auto multilineStringPattern = QRegularExpression("[\"]{3}.*[\"]{3}",
        QRegularExpression::InvertedGreedinessOption);
    highlighter->addRule(multilineStringPattern, LanguageHighlighter::STRING);

    auto multilineStringPattern2 = QRegularExpression("[\']{3}.*[\']{3}",
        QRegularExpression::InvertedGreedinessOption);
    highlighter->addRule(multilineStringPattern2, LanguageHighlighter::STRING);

    auto commentPattern = QRegularExpression("#([^\\r\\n]*)(?=\\r?\\n|$)*");
    highlighter->addRule(commentPattern, LanguageHighlighter::SINGLE_LINE_COMMENT);
}
