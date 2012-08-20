//-----------------------------------------------------------------------------
// File: BasicLineContentMatcher.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 20.08.2012
//
// Description:
// Line content matcher for simple list-based suggestions.
//-----------------------------------------------------------------------------

#ifndef BASICLINECONTENTMATCHER_H
#define BASICLINECONTENTMATCHER_H

#include <common/widgets/assistedLineEdit/ILineContentMatcher.h>

//-----------------------------------------------------------------------------
//! Line content matcher for simple list-based suggestions.
//-----------------------------------------------------------------------------
class BasicLineContentMatcher : public ILineContentMatcher
{
public:
    /*!
     *	Constructor.
     */
    BasicLineContentMatcher();

    /*!
     *  Destructor.
     */
    virtual ~BasicLineContentMatcher();

    /*!
     *  Sets the items that will be suggested by the content assist.
     *
     *      @param [in] items A list of string items.
     */
    void setItems(QStringList const& items);

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
     *      @param [in]  text          The text.
     *      @param [in]  assist        The content assist to fill with content.
     *      @param [out] startIndex    The start index of the selection to which the content applies.
     *      @param [out] toolTipText   The tool tip hint text. Empty if no tool tip available.
     *      @param [out] toolTipIndex  The index which specifies there start position for the tool tip hint.
     *
     *      @return True, if content was found; otherwise false.
     */
    bool fillWithContent(QString const& text, LineContentAssistWidget& assist,
                         int& startIndex, QString& toolTipText, int& toolTipIndex);

private:
    // Disable copying.
    BasicLineContentMatcher(BasicLineContentMatcher const& rhs);
    BasicLineContentMatcher& operator=(BasicLineContentMatcher const& rhs);

    /*!
     *  Enumerates matches based on the string. If the content assist is specified, the matching
     *  contents is also added to it.
     *
     *      @param [in] text    The text to which to match content.
     *      @param [in] assist  The content assist to fill with content. Can be null.
     *
     *      @return True, if there was content found; otherwise false.
     */
    bool enumerateMatches(QString const& text, LineContentAssistWidget* assist) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The suggestions items list.
    QStringList items_;
};


#endif // BASICLINECONTENTMATCHER_H
