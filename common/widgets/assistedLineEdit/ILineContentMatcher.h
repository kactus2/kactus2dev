//-----------------------------------------------------------------------------
// File: ILineContentMatcher.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 11.1.2011
//
// Description:
// Content matcher interface for assisted line edits.
//-----------------------------------------------------------------------------

#ifndef ILINECONTENTMATCHER_H
#define ILINECONTENTMATCHER_H

#include <QPlainTextEdit>

class LineContentAssistWidget;

//-----------------------------------------------------------------------------
//! ILineContentMatcher class.
//-----------------------------------------------------------------------------
class ILineContentMatcher
{
public:
    /*!
     *  Destructor.
     */
    virtual ~ILineContentMatcher() {}

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
     *      @return True, if content was found; otherwise false.
     */
    virtual bool fillWithContent(QString const& text, LineContentAssistWidget& assist,
                                 int& startIndex, QString& toolTipText, int& toolTipIndex) = 0;
};

//-----------------------------------------------------------------------------

#endif // ILINECONTENTMATCHER_H
