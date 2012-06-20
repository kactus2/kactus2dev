//-----------------------------------------------------------------------------
// File: CSourceContentMatcher.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 11.1.2011
//
// Description:
// MCAPI-specific content matcher.
//-----------------------------------------------------------------------------

#ifndef CSOURCECONTENTMATCHER_H
#define CSOURCECONTENTMATCHER_H

#include <QIcon>
#include <QList>
#include <QStringList>

#include "MCAPIDesignerTypes.h"
#include "common/widgets/assistedTextEdit/ITextContentMatcher.h"

class ApiDefinition;
class ApiFunction;
class ComDefinition;
class Component;

//-----------------------------------------------------------------------------
//! CSourceContentMatcher class.
//-----------------------------------------------------------------------------
class CSourceContentMatcher : public ITextContentMatcher
{
public:
    /*!
     *  Constructor.
     */
    CSourceContentMatcher();

    /*!
     *  Destructor.
     */
    ~CSourceContentMatcher();

    /*!
     *  Sets the component that owns this content matcher.
     *
     *      @param [in] component The component to set.
     */
    void setOwner(QSharedPointer<Component> component);

    /*!
     *  Adds a source API definition for content matching.
     *
     *      @param [in] apiDef The API definition to add.
     */
    void addSourceApiDefinition(QSharedPointer<ApiDefinition const> apiDef);

    /*!
     *  Adds a source COM definition for content matching.
     *
     *      @param [in] comDef The COM definition to add.
     */
    void addSourceComDefinition(QSharedPointer<ComDefinition const> comDef);

    /*!
     *  Looks forward for matching contents in the given text.
     *
     *      @param [in] text The text.
     *
     *      @return True, if there is a match. Otherwise false.
     */
    bool lookForwardMatch(QString const& text);

    /*!
     *  Fills the content assist with content based on the given text.
     *
     *      @param [in]  text The text.
     *      @param [in]  assist        The content assist to fill with content.
     *      @param [out] startIndex    The start index of the selection to which the content applies.
     *      @param [out] toolTipText   The tool tip hint text. Empty if no tool tip available.
     *      @param [out] toolTipIndex  The index which specifies there start position for the tool tip hint.
     *
     *      @return The content state (what kind of content was detected).
     */
    bool fillWithContent(QString const& text, TextContentAssistWidget& assist,
                         int& startIndex, QString& toolTipText, int& toolTipIndex);

private:
    //! MatchExecFunc definition.
    typedef void (*MatchExecFunc)(TextContentAssistWidget& assist, QString const& match, QIcon const& icon);

    //-----------------------------------------------------------------------------
    //! MatchType enumeration.
    //-----------------------------------------------------------------------------
    enum MatchType
    {
        MATCH_NONE = 0,
        MATCH_FUNCTION,
        MATCH_PARAM
    };

    // Disable copying.
    CSourceContentMatcher(CSourceContentMatcher const& rhs);
    CSourceContentMatcher& operator=(CSourceContentMatcher const& rhs);

    /*!
     *  Searches for matches in the given text.
     *
     *      @param [in]  text          The text to search.
     *      @param [in]  func          A function that is executed for each match. Can be null.
     *      @param [out] startIndex    The start index of the part of the text to which the matches apply.
     *      @param [out] toolTipText   The tool tip hint text. Empty if no tool tip available.
     *      @param [out] toolTipIndex  The cursor position in the text to indicate correct
     *                                 placement for the tool tip hint.
     *
     *      @return The content state (what kind of content was detected).
     */
    MatchType enumerateMatches(QString const& text, MatchExecFunc func, int& startIndex,
                               QString* toolTipText, int& toolTipIndex);

    /*!
     *  Enumerates matching names (function or type) in the given text.
     *
     *      @param [in]  text        The text to search.
     *      @param [in]  func        A function that is executed for each match. Can be null.
     *      @param [out] startIndex  The start index of the part of the text to which the matches apply.
     *      @param [out] exactMatch  If true, there was an exact match. Otherwise false.
     *
     *      @return The number of matches found.
     */
    int enumerateNames(QString const &text, MatchExecFunc func, int &startIndex, bool& exactMatch);

    /*!
     *  Enumerates matching function parameters in the given text.
     *
     *      @param [in]  text          The text to search.
     *      @param [in]  func          A function that is executed for each match. Can be null.
     *      @param [out] startIndex    The start index of the part of the text to which the matches apply.
     *      @param [out] exactMatch    If true, there was an exact match. Otherwise false.
     *      @param [out] toolTipText   The tool tip hint text. Empty if no tool tip available.
     *      @param [out] toolTipIndex  The cursor position in the text to indicate correct
     *                                 placement for the tool tip hint.
     *
     *      @return The number of matches found.
     *
     *      @remarks Assumes that there is valid function call up to the cursor.
     */
    int enumerateFunctionParams(QString const &text, MatchExecFunc func, int &startIndex,
                                bool& exactMatch, QString* toolTipText, int& toolTipIndex);

    /*!
     *  Adds the given match to the content assist.
     *
     *      @param [in] assist  The content assist.
     *      @param [in] match   The match to add.
     *      @param [in] type    The icon for the match.
     */
    static void addMatchToAssist(TextContentAssistWidget& assist, QString const& match, QIcon const& icon);

    /*!
     *  Extracts parameter names to a list from a parameter list string.
     *
     *      @param [in]  paramsListStr  The parameter list string.
     *      @param [out] params         The extracted parameter names.
     */
    static void extractParams(QString const& paramsListStr, QStringList& params);

    /*!
     *  Tries to match all available content to the parameter.
     *
     *      @param [in] apiFuncDesc  The API function corresponding to the function name.
     *      @param [in] params       The previous parameters for the function.
     *      @param [in] word         The partial content of the parameter to match.
     *      @param [in] func         The match execution function.
     *      @param [in,out] count    The accumulated number of matches.
     */
    void tryMatchParam(ApiFunction const* apiFuncDesc, QStringList const& params, QString const& word,
                       MatchExecFunc func, int& count);

    /*!
     *  Tries to match an identifier to the regular expression.
     *
     *      @param [in]     identifier The identifier.
     *      @param [in]     type       The identifier's MCAPI content type.
     *      @param [in]     exp        The regular expression to use for matching.
     *      @param [in]     func       The match execution function.
     *      @param [in/out] count      The accumulated number of matches.
     *
     *      @return True, if the identifier matched with the regular expression. Otherwise false.
     */
    bool tryMatchIdentifier(QString const& identifier, MCAPIContentType type,
                            QRegExp& exp, MatchExecFunc func, int& count);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Content icons.
    QIcon icons_[MCAPI_CONTENT_TYPE_COUNT];

    //! The owner component.
    QSharedPointer<Component> ownerComponent_;

    //! Source API definitions.
    QList< QSharedPointer<ApiDefinition const> > sourceApiDefinitions_;

    //! Source COM definitions.
    QList< QSharedPointer<ComDefinition const> > sourceComDefinitions_;

    //! Last found match type.
    MatchType lastMatchType_;

    //! The current assist that is being filled with content.
    TextContentAssistWidget* curAssist_;

    //! The generated tooltip text.
    QString tooltipText_;
};

//-----------------------------------------------------------------------------

#endif // CSOURCECONTENTMATCHER_H
