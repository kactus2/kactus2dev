//-----------------------------------------------------------------------------
// File: LanguageHighlighter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 15.02.2017
//
// Description:
// Base class for language-specific highlighters.
//-----------------------------------------------------------------------------

#include "LanguageHighlighter.h"

#include <QRegularExpressionMatch>

//-----------------------------------------------------------------------------
// Static member initializations.
//-----------------------------------------------------------------------------

//! Default style definitions.
HighlightStyleDesc const LanguageHighlighter::DEFAULT_STYLES[STYLE_COUNT] =
{
    HighlightStyleDesc(Qt::blue, false, false),
    HighlightStyleDesc(Qt::blue, false, false),
    HighlightStyleDesc(QColor(163, 21, 21), false, false),
    HighlightStyleDesc(Qt::darkGreen, false, false),
    HighlightStyleDesc(Qt::darkGreen, false, false),
    HighlightStyleDesc(Qt::blue, false, false),
    HighlightStyleDesc(Qt::darkMagenta, true, false),
    HighlightStyleDesc(Qt::darkMagenta, false, false)
};

//-----------------------------------------------------------------------------
// Function: LanguageHighlighter::getStyleName()
//-----------------------------------------------------------------------------
QString LanguageHighlighter::getStyleName(StyleType type)
{
    static const QStringList styles = QStringList() 
        << "Keywords"
        << "Preprocessor"
        << "Strings"
        << "SinglelineComments"
        << "MultilineComments"
        << "APITypes"
        << "APIFunc"
        << "LanguageSpecific";

    if (type > STYLE_COUNT)
    {
        return QString();
    }

    return styles.at(type);
}

//-----------------------------------------------------------------------------
// Function: LanguageHighlighter::LanguageHighlighter()
//-----------------------------------------------------------------------------
LanguageHighlighter::LanguageHighlighter(QTextDocument* parent): QSyntaxHighlighter(parent),
    highlightRules_(),
    commentStartExp_("/\\*"),
    commentEndExp_("\\*/")
{
    for (int i = 0; i < STYLE_COUNT; ++i)
    {
        setStyle(static_cast<StyleType>(i), DEFAULT_STYLES[i]);
    }
}

//-----------------------------------------------------------------------------
// Function: LanguageHighlighter::setStyle()
//-----------------------------------------------------------------------------
void LanguageHighlighter::setStyle(StyleType type, HighlightStyleDesc const& styleDesc)
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
// Function: LanguageHighlighter::addRule()
//-----------------------------------------------------------------------------
void LanguageHighlighter::addRule(QRegularExpression const& pattern, LanguageHighlighter::StyleType style)
{
    HighlightRule rule;
    rule.pattern = pattern;
    rule.format = &m_styleFormats[style];

    highlightRules_.append(rule);
}

//-----------------------------------------------------------------------------
// Function: LanguageHighlighter::addMultilineCommentRule()
//-----------------------------------------------------------------------------
void LanguageHighlighter::addMultilineCommentRule(QRegularExpression const& startPattern, 
    QRegularExpression const& endPattern)
{
    commentStartExp_ = startPattern;
    commentEndExp_ = endPattern;
}

//-----------------------------------------------------------------------------
// Function: LanguageHighlighter::highlightBlock()
//-----------------------------------------------------------------------------
void LanguageHighlighter::highlightBlock(QString const& text)
{
    for (HighlightRule const& rule : highlightRules_)
    {
        int index = text.indexOf(rule.pattern);

        while (index >= 0)
        {
            int length = rule.pattern.match(text).capturedLength();
            setFormat(index, length, *rule.format);
            index = text.indexOf(rule.pattern, index + length);
        }
    }

    setCurrentBlockState(0);

    int startIndex = 0;
    int startLength = 0;
    if (previousBlockState() != 1)
    {
        QRegularExpressionMatch match = commentStartExp_.match(text);
        startIndex = match.capturedStart();
        startLength = match.capturedLength();
    }

    while (startIndex >= 0)
    {
        QRegularExpressionMatch match = commentEndExp_.match(text, startIndex + startLength);
        int endIndex = match.capturedStart();
        int commentLength;

        if (endIndex == -1)
        {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }
        else
        {
            commentLength = endIndex - startIndex + match.capturedLength();
        }

        setFormat(startIndex, commentLength, m_styleFormats[MULTI_LINE_COMMENT]);
        startIndex = text.indexOf(commentStartExp_, startIndex + commentLength);
    }
}