//-----------------------------------------------------------------------------
// File: ImportHighlighter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 11.09.2014
//
// Description:
// Source file highlighter for import files.
//-----------------------------------------------------------------------------

#ifndef VHDLHIGHLIGHTER_H
#define VHDLHIGHLIGHTER_H

#include <Plugins/PluginSystem/ImportPlugin/Highlighter.h>

#include <QObject>
#include <QTextCharFormat>
#include <QPlainTextEdit>

//-----------------------------------------------------------------------------
//! Source file highlighter for import files.
//-----------------------------------------------------------------------------
class ImportHighlighter : public QObject, public Highlighter
{
    Q_OBJECT

public:
    /*!
     * The constructor.
     *
     *      @param [in] display   The display widget for the parsed source file content.
     *      @param [in] parent    The parent object.
     */
	ImportHighlighter(QPlainTextEdit* display, QObject* parent);

	//! The destructor.
	~ImportHighlighter();

    /*!
     *  Called when the text should be highlighted with the given color.
     *
     *      @param [in] text             The text to highlight.
     *      @param [in] highlightColor   The color of the highlight.
     */
    virtual void applyHighlight(QString const& text, QColor const& highlightColor);
    
    /*!
     *  Called when a section should be highlighted with the given color>.
     *
     *      @param [in] beginPosition   The beginning character index.
     *      @param [in] endPosition     The end character index.
     *      @param [in] highlightColor  The color of the highlight.
     */
    virtual void applyHighlight(int beginPosition, int endPosition, QColor const& highlightColor);

    /*!
     *  Called when the font color of a text should be changed to the given color.
     *
     *      @param [in] text    The text whose font to change.
     *      @param [in] color   The color of the text.     
     */
    virtual void applyFontColor(QString const& text, QColor const& color);

private:

	// Disable copying.
	ImportHighlighter(ImportHighlighter const& rhs);
	ImportHighlighter& operator=(ImportHighlighter const& rhs);
 
    //! Display widget for the source file content.
    QPlainTextEdit* display_;
};

#endif // VHDLHIGHLIGHTER_H
