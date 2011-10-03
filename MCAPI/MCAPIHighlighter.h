//-----------------------------------------------------------------------------
// File: MCAPIHighlighter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 21.12.2010
//
// Description:
// MCAPI/C syntax highlighter.
//-----------------------------------------------------------------------------

#ifndef CPPHIGHLIGHTER_H
#define CPPHIGHLIGHTER_H

#include <common/widgets/assistedTextEdit/HighlightStyleDesc.h>

#include <QSyntaxHighlighter>
#include <QTextEdit>
#include <QString>
#include <QVector>
#include <QTextCharFormat>

//-----------------------------------------------------------------------------
//! MCAPIHighlighter class.
//-----------------------------------------------------------------------------
class MCAPIHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    //-----------------------------------------------------------------------------
    //! StyleType enumeration.
    //-----------------------------------------------------------------------------
    enum StyleType
    {
        STYLE_KEYWORD = 0,
        STYLE_PREPROCESSOR,
        STYLE_STRING,
        STYLE_SINGLE_LINE_COMMENT,
        STYLE_MULTI_LINE_COMMENT,
        STYLE_MCAPI_TYPES,
        STYLE_MCAPI_FUNCTIONS,
        STYLE_COUNT
    };

    //! Default styles.
    static HighlightStyleDesc const DEFAULT_STYLES[STYLE_COUNT];

    //! Style ids for storing the styles in the settings.
    static QString const STYLE_IDS[STYLE_COUNT];

    /*!
     *  Constructor.
     *
     *      @param [in] parent The parent text document.
     */
    MCAPIHighlighter(QTextDocument* parent);

    /*!
     *  Destructor.
     */
    ~MCAPIHighlighter();

    /*!
     *  Sets the highlight style as a global style for all MCAPI highlighters.
     *
     *      @param [in] type       The style to set.
     *      @param [in] styleDesc  The style description.
     *
     *      @remarks The styles must be taken into use by calling applyStyles().
     */
    void setStyle(StyleType type, HighlightStyleDesc const& styleDesc);

    /*!
     *  Applies new syntax highlighting styles to the document.
     */
    void applyStyles();

protected:
    /*!
     *  Highlights a block of text.
     */
    void highlightBlock(QString const& text);

private:
    /*!
     *  Applies C++ style to the highlighter.
     */
    void applyCStyle();

    /*!
     *  Applies MCAPI style to the highlighter.
     */
    void applyMCAPIStyle();

    //-----------------------------------------------------------------------------
    //! HighlightRule structure.
    //-----------------------------------------------------------------------------
    struct HighlightRule
    {
        QRegExp pattern;            //!< The pattern to which the rule is applied.
        QTextCharFormat format;     //!< The formatting for the rule.
    };

    //! The rules for highlighting.
    QVector<HighlightRule> m_highlightRules;

    //! Comment start expression.
    QRegExp m_commentStartExp;

    //! Comment end expression.
    QRegExp m_commentEndExp;

    //! Style formats.
    QTextCharFormat m_styleFormats[STYLE_COUNT];
};

//-----------------------------------------------------------------------------

#endif // CPPHIGHLIGHTER_H
