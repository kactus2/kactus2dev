//-----------------------------------------------------------------------------
// File: CSourceHighlight.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 15.02.2017
//
// Description:
// C source language-specific highlight rules.
//-----------------------------------------------------------------------------

#include "CSourceHighlight.h"

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: CSourceHighlight::CSourceHighlight()
//-----------------------------------------------------------------------------
CSourceHighlight::CSourceHighlight()
{
    
}

//-----------------------------------------------------------------------------
// Function: CSourceHighlight::~CSourceHighlight()
//-----------------------------------------------------------------------------
CSourceHighlight::~CSourceHighlight()
{

}


//-----------------------------------------------------------------------------
// Function: CSourceHighlight::keywords()
//-----------------------------------------------------------------------------
QStringList CSourceHighlight::keywords() const
{
    QStringList keywordPatterns;
    keywordPatterns << "auto" << "break" << "case"
        << "char" << "const" << "continue"
        << "default" << "do" << "double"
        << "else" << "enum" << "extern"
        << "float" << "for" << "goto"
        << "if" << "int" << "long" 
        << "register" << "return" << "short"
        << "signed" << "sizeof" << "static"
        << "struct" << "switch" << "typedef"
        << "union" << "unsigned" << "void"
        << "volatile" << "while";

    return keywordPatterns;
}

//-----------------------------------------------------------------------------
// Function: CSourceHighlight::preprocessorDirectives()
//-----------------------------------------------------------------------------
QStringList CSourceHighlight::preprocessorDirectives() const
{
    QStringList preprocPatterns;
    preprocPatterns << "#include" << "#ifndef" << "#ifdef" << "#if" << "#endif" << "#pragma" << "#define";

    return preprocPatterns;
}

//-----------------------------------------------------------------------------
// Function: CSourceHighlight::applyLanguageSpecificRules()
//-----------------------------------------------------------------------------
void CSourceHighlight::applyLanguageSpecificRules(LanguageHighlighter* highlighter) const
{
    QRegularExpression pattern = QRegularExpression("^#include <.*>");
    highlighter->addRule(pattern, LanguageHighlighter::STRING);
}
