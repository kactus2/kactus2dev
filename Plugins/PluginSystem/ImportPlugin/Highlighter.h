//-----------------------------------------------------------------------------
// File: Highlighter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 18.09.2014
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QString>
#include <QColor>

class Highlighter
{
public:

	//! The destructor.
    virtual ~Highlighter() {};

    /*!
     *  Called when the text should be highlighted with the given color.
     *
     *      @param [in] text             The text to highlight.
     *      @param [in] highlightColor   The color of the highlight.
     */
    virtual void applyHighlight(QString const& text, QColor const& highlightColor) = 0;

    /*!
     *  Called when a section should be highlighted with the given color>.
     *
     *      @param [in] beginPosition   The beginning character index.
     *      @param [in] endPosition     The end character index.
     *      @param [in] highlightColor  The color of the highlight.
     */
    virtual void applyHighlight(int beginPosition, int endPosition, QColor const& highlightColor) = 0;

    /*!
     *  Called when the font color of a text should be changed to the given color.
     *
     *      @param [in] text    The text whose font to change.
     *      @param [in] color   The color of the text.     
     */
    virtual void applyFontColor(QString const& text, QColor const& color) = 0;
};

#endif // HIGHLIGHTER_H
