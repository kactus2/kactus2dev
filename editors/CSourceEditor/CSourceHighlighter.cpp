//-----------------------------------------------------------------------------
// File: CSourceHighlighter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 21.12.2010
//
// Description:
// MCAPI/C syntax highlighter.
//-----------------------------------------------------------------------------

#include "CSourceHighlighter.h"

#include <IPXACTmodels/ApiDefinition.h>
#include <IPXACTmodels/ApiFunction.h>

//-----------------------------------------------------------------------------
// Static member initializations.
//-----------------------------------------------------------------------------

//! Default style definitions.
HighlightStyleDesc const CSourceHighlighter::DEFAULT_STYLES[STYLE_COUNT] =
{
    HighlightStyleDesc(Qt::blue, false, false),
    HighlightStyleDesc(Qt::blue, false, false),
    HighlightStyleDesc(QColor(163, 21, 21), false, false),
    HighlightStyleDesc(Qt::darkGreen, false, false),
    HighlightStyleDesc(Qt::darkGreen, false, false),
    HighlightStyleDesc(Qt::blue, false, false),
    HighlightStyleDesc(Qt::darkMagenta, true, false)
};

//! Style ids for storing the styles in the settings.
QString const CSourceHighlighter::STYLE_IDS[STYLE_COUNT] =
{
    "Keywords",
    "Preprocessor",
    "Strings",
    "SinglelineComments",
    "MultilineComments",
    "APITypes",
    "APIFunc"
};

//-----------------------------------------------------------------------------
// Function: CSourceHighlighter()
//-----------------------------------------------------------------------------
CSourceHighlighter::CSourceHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent),
      highlightRules_(),
      commentStartExp_("/\\*"),
      commentEndExp_("\\*/")
{
    for (int i = 0; i < STYLE_COUNT; ++i)
    {
        setStyle(static_cast<StyleType>(i), DEFAULT_STYLES[i]);
    }

    // Apply the styles.
    addBuiltinRules();
}

//-----------------------------------------------------------------------------
// Function: ~CSourceHighlighter()
//-----------------------------------------------------------------------------
CSourceHighlighter::~CSourceHighlighter()
{
}

//-----------------------------------------------------------------------------
// Function: setStyle()
//-----------------------------------------------------------------------------
void CSourceHighlighter::setStyle(StyleType type, HighlightStyleDesc const& styleDesc)
{
    Q_ASSERT(type < STYLE_COUNT);

    if (styleDesc.bold)
    {
        m_styleFormats[type].setFontWeight(QFont::Bold);
    }
    else
    {
        m_styleFormats[type].setFontWeight(QFont::Normal);
    }

    m_styleFormats[type].setFontItalic(styleDesc.italic);
    m_styleFormats[type].setForeground(QBrush(styleDesc.color));
}

//-----------------------------------------------------------------------------
// Function: highlightBlock()
//-----------------------------------------------------------------------------
void CSourceHighlighter::highlightBlock(QString const& text)
{
    foreach (HighlightRule const& rule, highlightRules_)
    {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);

        while (index >= 0)
        {
            int length = expression.matchedLength();
            setFormat(index, length, *rule.format);
            index = expression.indexIn(text, index + length);
        }
    }

    setCurrentBlockState(0);

    int startIndex = 0;

    if (previousBlockState() != 1)
    {
        startIndex = commentStartExp_.indexIn(text);
    }

    while (startIndex >= 0)
    {
        int endIndex = commentEndExp_.indexIn(text, startIndex);
        int commentLength;

        if (endIndex == -1)
        {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }
        else
        {
            commentLength = endIndex - startIndex + commentEndExp_.matchedLength();
        }

        setFormat(startIndex, commentLength, m_styleFormats[STYLE_MULTI_LINE_COMMENT]);
        startIndex = commentStartExp_.indexIn(text, startIndex + commentLength);
    }
}

//-----------------------------------------------------------------------------
// Function: addBuiltinRules()
//-----------------------------------------------------------------------------
void CSourceHighlighter::addBuiltinRules()
{
    HighlightRule rule;

    rule.pattern = QRegExp("^#include <.*>");
    rule.format = &m_styleFormats[STYLE_STRING];
    highlightRules_.append(rule);

    QStringList keywordPatterns;
    keywordPatterns << "\\bauto\\b" << "\\bbreak\\b" << "\\bcase\\b"
                    << "\\bchar\\b" << "\\bconst\\b" << "\\bcontinue\\b"
                    << "\\bdefault\\b" << "\\bdo\\b" << "\\bdouble\\b"
                    << "\\belse\\b" << "\\benum\\b" << "\\bextern\\b"
                    << "\\bfloat\\b" << "\\bfor\\b" << "\\bgoto\\b"
                    << "\\bif\\b" << "\\bint\\b" << "\\blong\\b" 
                    << "\\bregister\\b" << "\\breturn\\b" << "\\bshort\\b"
                    << "\\bsigned\\b" << "\\bsizeof\\b" << "\\bstatic\\b"
                    << "\\bstruct\\b" << "\\bswitch\\b" << "\\btypedef\\b"
                    << "\\bunion\\b" << "\\bunsigned\\b" << "\\bvoid\\b"
                    << "\\bvolatile\\b" << "\\bwhile\\b";

    foreach(const QString &pattern, keywordPatterns)
    {
        rule.pattern = QRegExp(pattern);
        rule.format = &m_styleFormats[STYLE_KEYWORD];
        highlightRules_.append(rule);
    }

    QStringList preprocPatterns;
    preprocPatterns << "^#include\\b" << "^#ifndef\\b" << "^#ifdef\\b"
                    << "^#if\\b" << "^#endif\\b" << "^#pragma\\b" << "^#define\\b";

    foreach(const QString &pattern, preprocPatterns)
    {
        rule.pattern = QRegExp(pattern);
        rule.format = &m_styleFormats[STYLE_PREPROCESSOR];
        highlightRules_.append(rule);
    }

    rule.pattern = QRegExp("\"([^\"]|(\\\\\"))*\"");
    rule.format = &m_styleFormats[STYLE_STRING];
    highlightRules_.append(rule);

    rule.pattern = QRegExp("\'([^\']|(\\\\\'))*\'");
    rule.format = &m_styleFormats[STYLE_STRING];
    highlightRules_.append(rule);

    rule.pattern = QRegExp("//[^\n]*");
    rule.format = &m_styleFormats[STYLE_SINGLE_LINE_COMMENT];
    highlightRules_.append(rule);
}

//-----------------------------------------------------------------------------
// Function: CSourceHighlighter::registerAPI()
//-----------------------------------------------------------------------------
void CSourceHighlighter::registerAPI(QSharedPointer<ApiDefinition const> apiDef)
{
    HighlightRule rule;
    rule.format = &m_styleFormats[STYLE_API_DATA_TYPES];

    // Register data type rules.
    foreach (QString const& dataType, apiDef->getDataTypes())
    {
        rule.pattern = QRegExp("\\b" + dataType + "\\b");
        highlightRules_.append(rule);
    }

    // Register function rules.
    rule.format = &m_styleFormats[STYLE_API_FUNCTIONS];

    for (int i = 0; i < apiDef->getFunctionCount(); ++i)
    {
        QSharedPointer<ApiFunction const> apiFunc = apiDef->getFunction(i);
        rule.pattern = QRegExp("\\b" + apiFunc->getName() + "\\b");
        highlightRules_.append(rule);
    }
}
