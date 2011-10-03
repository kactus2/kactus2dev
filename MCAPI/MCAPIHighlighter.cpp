//-----------------------------------------------------------------------------
// File: MCAPIHighlighter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 21.12.2010
//
// Description:
// MCAPI/C syntax highlighter.
//-----------------------------------------------------------------------------

#include "MCAPIHighlighter.h"

//-----------------------------------------------------------------------------
// Static member initializations.
//-----------------------------------------------------------------------------

//! Default style definitions.
HighlightStyleDesc const MCAPIHighlighter::DEFAULT_STYLES[STYLE_COUNT] =
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
QString const MCAPIHighlighter::STYLE_IDS[STYLE_COUNT] =
{
    "keywords",
    "preprocessor",
    "strings",
    "singleline_comments",
    "multiline_comments",
    "mcapi_types",
    "mcapi_func"
};

//-----------------------------------------------------------------------------
// Function: MCAPIHighlighter()
//-----------------------------------------------------------------------------
MCAPIHighlighter::MCAPIHighlighter(QTextDocument* parent) : QSyntaxHighlighter(parent),
                                                            m_highlightRules(),
                                                            m_commentStartExp("/\\*"),
                                                            m_commentEndExp("\\*/")
{
    for (int i = 0; i < STYLE_COUNT; ++i)
    {
        setStyle(static_cast<StyleType>(i), DEFAULT_STYLES[i]);
    }

    // Apply the styles.
    applyStyles();
}

//-----------------------------------------------------------------------------
// Function: ~MCAPIHighlighter()
//-----------------------------------------------------------------------------
MCAPIHighlighter::~MCAPIHighlighter()
{
}

//-----------------------------------------------------------------------------
// Function: setStyle()
//-----------------------------------------------------------------------------
void MCAPIHighlighter::setStyle(StyleType type, HighlightStyleDesc const& styleDesc)
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
void MCAPIHighlighter::highlightBlock(QString const& text)
{
    foreach (HighlightRule const& rule, m_highlightRules)
    {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);

        while (index >= 0)
        {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }

    setCurrentBlockState(0);

    int startIndex = 0;

    if (previousBlockState() != 1)
    {
        startIndex = m_commentStartExp.indexIn(text);
    }

    while (startIndex >= 0)
    {
        int endIndex = m_commentEndExp.indexIn(text, startIndex);
        int commentLength;

        if (endIndex == -1)
        {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }
        else
        {
            commentLength = endIndex - startIndex + m_commentEndExp.matchedLength();
        }

        setFormat(startIndex, commentLength, m_styleFormats[STYLE_MULTI_LINE_COMMENT]);
        startIndex = m_commentStartExp.indexIn(text, startIndex + commentLength);
    }
}

//-----------------------------------------------------------------------------
// Function: applyStyles()
//-----------------------------------------------------------------------------
void  MCAPIHighlighter::applyStyles()
{
    m_highlightRules.clear();

    applyMCAPIStyle();
    applyCStyle();

    rehighlight();
}

//-----------------------------------------------------------------------------
// Function: applyCStyle()
//-----------------------------------------------------------------------------
void MCAPIHighlighter::applyCStyle()
{
    HighlightRule rule;

    rule.pattern = QRegExp("^#include <.*>");
    rule.format = m_styleFormats[STYLE_STRING];
    m_highlightRules.append(rule);

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
        rule.format = m_styleFormats[STYLE_KEYWORD];
        m_highlightRules.append(rule);
    }

    QStringList preprocPatterns;
    preprocPatterns << "^#include\\b" << "^#ifndef\\b" << "^#ifdef\\b"
                    << "^#if\\b" << "^#endif\\b" << "^#pragma\\b";

    foreach(const QString &pattern, preprocPatterns)
    {
        rule.pattern = QRegExp(pattern);
        rule.format = m_styleFormats[STYLE_PREPROCESSOR];
        m_highlightRules.append(rule);
    }

    rule.pattern = QRegExp("\"([^\"]|(\\\\\"))*\"");
    rule.format = m_styleFormats[STYLE_STRING];
    m_highlightRules.append(rule);

    rule.pattern = QRegExp("\'([^\']|(\\\\\'))*\'");
    rule.format = m_styleFormats[STYLE_STRING];
    m_highlightRules.append(rule);

    rule.pattern = QRegExp("//[^\n]*");
    rule.format = m_styleFormats[STYLE_SINGLE_LINE_COMMENT];
    m_highlightRules.append(rule);
}

//-----------------------------------------------------------------------------
// Function: applyMCAPIStyle()
//-----------------------------------------------------------------------------
void MCAPIHighlighter::applyMCAPIStyle()
{
    HighlightRule rule;

    QStringList mcapiTypePatterns;
    mcapiTypePatterns << "\\bmcapi_int_t\\b" << "\\bmcapi_uint_t\\b"
        << "\\bmcapi_uint8_t\\b" << "\\bmcapi_uint16_t\\b"
        << "\\bmcapi_uint32_t\\b" << "\\bmcapi_uint64_t\\b"                    
        << "\\bmcapi_boolean_t\\b" << "\\bmcapi_endpoint_t\\b"
        << "\\bmcapi_node_t\\b" << "\\bmcapi_port_t\\b"
        << "\\bmcapi_version_t\\b" << "\\bmcapi_status_t\\b"
        << "\\bmcapi_request_t\\b" << "\\bmcapi_priority_t\\b"
        << "\\bmcapi_timeout_t\\b" << "\\bmcapi_pktchan_recv_hndl_t\\b"
        << "\\bmcapi_pktchan_send_hndl_t\\b" << "\\bmcapi_sclchan_recv_hndl_t\\b"
        << "\\bmcapi_sclchan_send_hndl_t\\b";

    foreach (QString const& pattern, mcapiTypePatterns)
    {
        rule.pattern = QRegExp(pattern);
        rule.format = m_styleFormats[STYLE_MCAPI_TYPES];
        m_highlightRules.append(rule);
    }

    QStringList mcapiPatterns;
    mcapiPatterns << "\\bmcapi_initialize(?=\\()" << "\\bmcapi_finalize(?=\\()"
                  << "\\bmcapi_get_node_id(?=\\()" << "\\bmcapi_create_endpoint(?=\\()"
                  << "\\bmcapi_get_endpoint_i(?=\\()" << "\\bmcapi_get_endpoint(?=\\()"
                  << "\\bmcapi_delete_endpoint(?=\\()" << "\\bmcapi_get_endpoint_attribute(?=\\()"
                  << "\\bmcapi_set_endpoint_attribute(?=\\()" << "\\bmcapi_msg_send_i(?=\\()"
                  << "\\bmcapi_msg_send(?=\\()" << "\\bmcapi_msg_recv_i(?=\\()"
                  << "\\bmcapi_msg_recv(?=\\()" << "\\bmcapi_msg_available(?=\\()"
                  << "\\bmcapi_connect_pktchan_i(?=\\()" << "\\bmcapi_open_pktchan_recv_i(?=\\()"
                  << "\\bmcapi_open_pktchan_send_i(?=\\()" << "\\bmcapi_pktchan_send_i(?=\\()"
                  << "\\bmcapi_pktchan_send(?=\\()" << "\\bmcapi_pktchan_recv_i(?=\\()"
                  << "\\bmcapi_pktchan_recv(?=\\()" << "\\bmcapi_pktchan_available(?=\\()"
                  << "\\bmcapi_pktchan_free(?=\\()" << "\\bmcapi_packetchan_recv_close_i(?=\\()"
                  << "\\bmcapi_packetchan_send_close_i(?=\\()" << "\\bmcapi_connect_sclchan_i(?=\\()"
                  << "\\bmcapi_open_sclchan_recv_i(?=\\()" << "\\bmcapi_open_sclchan_send_i(?=\\()"
                  << "\\bmcapi_sclchan_send_uint64(?=\\()" << "\\bmcapi_sclchan_send_uint32(?=\\()"
                  << "\\bmcapi_sclchan_send_uint16(?=\\()" << "\\bmcapi_sclchan_send_uint8(?=\\()"
                  << "\\bmcapi_sclchan_recv_uint64(?=\\()" << "\\bmcapi_sclchan_recv_uint32(?=\\()"
                  << "\\bmcapi_sclchan_recv_uint16(?=\\()" << "\\bmcapi_sclchan_recv_uint8(?=\\()"
                  << "\\bmcapi_sclchan_available(?=\\()" << "\\bmcapi_sclchan_recv_close_i(?=\\()"
                  << "\\bmcapi_sclchan_send_close_i(?=\\()" << "\\bmcapi_test(?=\\()"
                  << "\\bmcapi_wait(?=\\()" << "\\bmcapi_wait_any(?=\\()"
                  << "\\bmcapi_cancel(?=\\()";

    foreach (const QString &pattern, mcapiPatterns)
    {
        rule.pattern = QRegExp(pattern);
        rule.format = m_styleFormats[STYLE_MCAPI_FUNCTIONS];
        m_highlightRules.append(rule);
    }
}
