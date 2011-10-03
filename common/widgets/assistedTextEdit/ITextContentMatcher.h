//-----------------------------------------------------------------------------
// File: ITextContentMatcher.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 11.1.2011
//
// Description:
// Content matcher interface for filling content assist with content.
//-----------------------------------------------------------------------------

#ifndef ITEXTCONTENTMATCHER_H
#define ITEXTCONTENTMATCHER_H

class TextContentAssistWidget;

//-----------------------------------------------------------------------------
//! ITextContentMatcher class.
//-----------------------------------------------------------------------------
class ITextContentMatcher
{
public:
    /*!
     *  Destructor.
     */
    virtual ~ITextContentMatcher() {}

    /*!
     *  Looks forward for matching contents in the given text.
     *
     *      @param [in] text The text.
     *
     *      @return True, if there is a match. Otherwise false.
     */
    virtual bool lookForwardMatch(QString const& text) = 0;

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
    virtual bool fillWithContent(QString const& text, TextContentAssistWidget& assist,
                                 int& startIndex, QString& toolTipText, int& toolTipIndex) = 0;
};

//-----------------------------------------------------------------------------

#endif // ITEXTCONTENTMATCHER_H
