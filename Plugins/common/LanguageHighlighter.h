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

#ifndef LANGUAGEHIGHLIGHTER_H
#define LANGUAGEHIGHLIGHTER_H

#include <common/widgets/assistedTextEdit/HighlightStyleDesc.h>

#include <QSyntaxHighlighter>
#include <QTextEdit>
#include <QString>
#include <QVector>
#include <QTextCharFormat>
#include <QRegularExpression>

//-----------------------------------------------------------------------------
//! LanguageHighlighter class.
//-----------------------------------------------------------------------------
class LanguageHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:

    //-----------------------------------------------------------------------------
    //! StyleType enumeration.
    //-----------------------------------------------------------------------------
    enum StyleType
    {
        KEYWORD = 0,
        PREPROCESSOR,
        STRING,
        SINGLE_LINE_COMMENT,
        MULTI_LINE_COMMENT,
        API_DATA_TYPES,
        API_FUNCTIONS,
        LANGUAGE_SPECIFIC,
        STYLE_COUNT
    };

    //! Default styles.
    static HighlightStyleDesc const DEFAULT_STYLES[STYLE_COUNT];
   
    /*!
     *  Gets the name for the given style type.
     *
     *    @param [in] type   The type whose name to find.
     *
     *    @return The name for the type.
     */
    static QString getStyleName(StyleType type);

    /*!
     *  Constructor.
     *
     *    @param [in] parent The parent text document.
     */
    explicit LanguageHighlighter(QTextDocument* parent);

    /*!
     *  Destructor.
     */
    virtual ~LanguageHighlighter() = default;

    /*!
     *  Sets the highlight style as a global style for all MCAPI highlighters.
     *
     *    @param [in] type       The style to set.
     *    @param [in] styleDesc  The style description.
     *
     *      @remarks The styles must be taken into use by calling applyStyles().
     */
    void setStyle(StyleType type, HighlightStyleDesc const& styleDesc);
    
    /*!
     *  Add a highlighting rule.
     *
     *    @param [in] pattern     Pattern to highlight.
     *    @param [in] style       Style to apply to pattern.
     */
    void addRule(QRegularExpression const& pattern, LanguageHighlighter::StyleType style);

    /*!
     * Add highlighting rule for multiline comments. Formatting style for the comment is set with addRule().
     *
     *    @param [in] startPattern     Pattern for start of multiline comment.
     *    @param [in] endPattern       Pattern for end of multiline comment.
     */
     void addMultilineCommentRule(QRegularExpression const& startPattern,
        QRegularExpression const& endPattern);

protected:
    /*!
     *  Highlights a block of text.
     */
    void highlightBlock(QString const& text);

    //-----------------------------------------------------------------------------
    //! HighlightRule structure.
    //-----------------------------------------------------------------------------
    struct HighlightRule
    {
        QRegularExpression pattern;            //!< The pattern to which the rule is applied.
        QTextCharFormat* format;    //!< The formatting for the rule.
    };

private:

    // Disable copying.
    LanguageHighlighter(LanguageHighlighter const& rhs);
    LanguageHighlighter& operator=(LanguageHighlighter const& rhs);

    //! The rules for highlighting.
    QVector<HighlightRule> highlightRules_;

    //! Comment start expression.
    QRegularExpression commentStartExp_;

    //! Comment end expression.
    QRegularExpression commentEndExp_;

    //! Style formats.
    QTextCharFormat m_styleFormats[STYLE_COUNT];
};

//-----------------------------------------------------------------------------

#endif // LANGUAGEHIGHLIGHTER_H
