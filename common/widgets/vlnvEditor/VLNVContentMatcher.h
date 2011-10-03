//-----------------------------------------------------------------------------
// File: VLNVContentMatcher.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 11.2.2011
//
// Description:
// VLNV content matcher.
//-----------------------------------------------------------------------------

#ifndef VLNVCONTENTMATCHER_H
#define VLNVCONTENTMATCHER_H

#include <common/widgets/assistedLineEdit/ILineContentMatcher.h>

class LibraryItem;

//-----------------------------------------------------------------------------
//! VLNVContentMatcher class.
//-----------------------------------------------------------------------------
class VLNVContentMatcher : public ILineContentMatcher
{
public:
    /*!
     *	Constructor.
     *
     *      @param [in] item The library item to use for finding content.
     */
    VLNVContentMatcher(LibraryItem const* item);

    /*!
     *  Destructor.
     */
    virtual ~VLNVContentMatcher();

    /*!
     *  Sets the library item to use for finding content.
     */
    void setLibraryItem(LibraryItem const* item);

    /*!
     *  Returns the library item associated with the content matcher.
     */
    LibraryItem const* getLibraryItem() const;

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
    VLNVContentMatcher(VLNVContentMatcher const& rhs);
    VLNVContentMatcher& operator=(VLNVContentMatcher const& rhs);

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

    //! The library item.
    LibraryItem const* m_item;
};

//-----------------------------------------------------------------------------

#endif // VLNVCONTENTMATCHER_H
