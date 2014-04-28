//-----------------------------------------------------------------------------
// File: CSourceHighlighter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 21.12.2010
//
// Description:
// MCAPI/C syntax highlighter.
//-----------------------------------------------------------------------------

#ifndef CSOURCEHIGHLIGHTER_H
#define CSOURCEHIGHLIGHTER_H

#include <common/widgets/assistedTextEdit/HighlightStyleDesc.h>

#include <QSyntaxHighlighter>
#include <QTextEdit>
#include <QString>
#include <QVector>
#include <QTextCharFormat>

class ApiDefinition;

//-----------------------------------------------------------------------------
//! CSourceHighlighter class.
//-----------------------------------------------------------------------------
class CSourceHighlighter : public QSyntaxHighlighter
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
        STYLE_API_DATA_TYPES,
        STYLE_API_FUNCTIONS,
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
    CSourceHighlighter(QTextDocument* parent);

    /*!
     *  Destructor.
     */
    ~CSourceHighlighter();

    /*!
     *  Registers data types and functions from the given API definition for syntax highlight.
     *
     *      @param [in] apiDef The API definition.
     */
    void registerAPI(QSharedPointer<ApiDefinition const> apiDef);

    /*!
     *  Sets the highlight style as a global style for all MCAPI highlighters.
     *
     *      @param [in] type       The style to set.
     *      @param [in] styleDesc  The style description.
     *
     *      @remarks The styles must be taken into use by calling applyStyles().
     */
    void setStyle(StyleType type, HighlightStyleDesc const& styleDesc);

protected:
    /*!
     *  Highlights a block of text.
     */
    void highlightBlock(QString const& text);

private:
    /*!
     *  Applies C++ style to the highlighter.
     */
    void addBuiltinRules();

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
        QTextCharFormat* format;    //!< The formatting for the rule.
    };

    //! The rules for highlighting.
    QVector<HighlightRule> highlightRules_;

    //! Comment start expression.
    QRegExp commentStartExp_;

    //! Comment end expression.
    QRegExp commentEndExp_;

    //! Style formats.
    QTextCharFormat m_styleFormats[STYLE_COUNT];
};

//-----------------------------------------------------------------------------

#endif // CSOURCEHIGHLIGHTER_H
