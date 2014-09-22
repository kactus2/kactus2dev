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

#include <Plugins/PluginSystem/Highlighter.h>

#include <QObject>
#include <QTextCharFormat>
#include <QPlainTextEdit>

//-----------------------------------------------------------------------------
//! Source file highlighter for VHDL files.
//-----------------------------------------------------------------------------
class VHDLHighlighter : public QObject, public Highlighter
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

    //! Called when the text should be highlighted with the given color.
    virtual void applyHighlight(QString const& text, QColor const& highlightColor);

    //! Called when the font color of a text should be changed to the given color.
    virtual void applyFontColor(QString const& text, QColor const& color);

private:

	// Disable copying.
	VHDLHighlighter(VHDLHighlighter const& rhs);
	VHDLHighlighter& operator=(VHDLHighlighter const& rhs);
 
    //! Display widget for the source file content.
    QPlainTextEdit* display_;
};

#endif // VHDLHIGHLIGHTER_H
