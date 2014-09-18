//-----------------------------------------------------------------------------
// File: VHDLHighlighter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 11.09.2014
//
// Description:
// Source file highlighter for VHDL files.
//-----------------------------------------------------------------------------

#ifndef VHDLHIGHLIGHTER_H
#define VHDLHIGHLIGHTER_H

#include <QObject>
#include <QTextCharFormat>
#include <QPlainTextEdit>

//-----------------------------------------------------------------------------
//! Source file highlighter for VHDL files.
//-----------------------------------------------------------------------------
class VHDLHighlighter : public QObject
{
    Q_OBJECT

public:
    /*!
     * The constructor.
     *
     *      @param [in] display   The display widget for the parsed source file content.
     *      @param [in] parent    The parent object.
     */
	VHDLHighlighter(QPlainTextEdit* display, QObject* parent);

	//! The destructor.
	~VHDLHighlighter();

    /*!
     *  Registers a new highlight source for the highlighter.
     *
     *      @param [in] highlightSource   The highlight source to register.
     *
     *      @remark The source must have and emit a signal with the following signature:
     *              highlight(QString const& textToHighlight, QColor const& highlightColor);
     */
    void registerHighlightSource(QObject* highlightSource);

public slots:
    
    //! Called when the font color of a text should be changed to the given color.
    void applyFontColor(QString const& text, QColor const& color);

    //! Called when the text should be highlighted with the given color.
    void applyHighlight(QString const& text, QColor const& highlightColor) const;

private:

	// Disable copying.
	VHDLHighlighter(VHDLHighlighter const& rhs);
	VHDLHighlighter& operator=(VHDLHighlighter const& rhs);
 
    //! Display widget for the source file content.
    QPlainTextEdit* display_;
};

#endif // VHDLHIGHLIGHTER_H
