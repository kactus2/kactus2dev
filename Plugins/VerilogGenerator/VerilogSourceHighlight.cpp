//-----------------------------------------------------------------------------
// File: VerilogHighlighter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 15.02.2017
//
// Description:
// Verilog language-specific highlight rules.
//-----------------------------------------------------------------------------

#include "VerilogSourceHighlight.h"

//-----------------------------------------------------------------------------
// Function: VerilogSourceHighlight::VerilogSourceHighlight()
//-----------------------------------------------------------------------------
VerilogSourceHighlight::VerilogSourceHighlight(): SourceHighlightStyle()
{

}

//-----------------------------------------------------------------------------
// Function: VerilogSourceHighlight::~VerilogSourceHighlight()
//-----------------------------------------------------------------------------
VerilogSourceHighlight::~VerilogSourceHighlight()
{

}

//-----------------------------------------------------------------------------
// Function: VerilogSourceHighlight::preprocessorDirectives()
//-----------------------------------------------------------------------------
QStringList VerilogSourceHighlight::preprocessorDirectives() const
{
    QStringList directives;
    directives << "`include" << "`ifndef" << "`ifdef"
        << "`if" << "`endif" << "`elsif" << "`else" << "`undef" << "`define";

    return directives;
}

//-----------------------------------------------------------------------------
// Function: VerilogSourceHighlight::keywords()
//-----------------------------------------------------------------------------
QStringList VerilogSourceHighlight::keywords() const
{
    QStringList keywordList;
    keywordList << "assign" << "and" << "always" << "begin" 
        << "case" << "deassign" << "default" 
        << "defparam" << "end" << "edge"
        << "else" << "endcase" << "endfunction"
        << "endgenerate" << "endmodule" << "for" 
        << "if" << "function" << "generate" 
        << "integer" << "inout" << "input"
        << "localparam" << "macromodule" << "module"
        << "negedge" << "output"  << "or" << "parameter"
        << "posedge" << "real" << "realtime"
        << "reg" << "signed" << "time"
        << "tri" << "tri0" << "tri1"
        << "triand" << "wand" << "trior"
        << "wor" << "trireg" << "while"
        << "wire";

    return keywordList;
}

//-----------------------------------------------------------------------------
// Function: VerilogSourceHighlight::applyLanguageSpecificRules()
//-----------------------------------------------------------------------------
void VerilogSourceHighlight::applyLanguageSpecificRules(LanguageHighlighter* highlighter) const
{
    HighlightStyleDesc stringStyle;
    stringStyle.color = KactusAttribute::STRING_HIGHLIGHT;

    highlighter->setStyle(LanguageHighlighter::STRING, stringStyle);

    HighlightStyleDesc preprocessorStyle;
    preprocessorStyle.color = KactusAttribute::PREPROCESSOR_HIGHLIGHT;

    highlighter->setStyle(LanguageHighlighter::PREPROCESSOR, preprocessorStyle);

    HighlightStyleDesc functionStyle;
    functionStyle.color = KactusAttribute::FUNCTION_HIGHLIGHT;

    highlighter->setStyle(LanguageHighlighter::LANGUAGE_SPECIFIC, functionStyle);

    QRegularExpression function("[$]\\w+");
    highlighter->addRule(function, LanguageHighlighter::LANGUAGE_SPECIFIC);
}
