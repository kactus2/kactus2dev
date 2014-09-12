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

#include <QTextCharFormat>
#include <QObject>

class SourceFileDisplayer;

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
	VHDLHighlighter(SourceFileDisplayer* display, QObject* parent);

	//! The destructor.
	~VHDLHighlighter();

    /*!
     *  Highlights an unselected section from the source file.
     *
     *      @param [in] beginPos    The starting character index of the section.
     *      @param [in] endPos      The ending character index of the section.
     */
    void unSelect(int beginPos, int endPos) const;

    /*!
     *  Highlights a selected port section from the source file.
     *
     *      @param [in] beginPos    The starting character index of the section.
     *      @param [in] endPos      The ending character index of the section.
     */
    void selectPort(int beginPos, int endPos) const;

    /*!
     *  Highlights a selected model parameter section from the source file.
     *
     *      @param [in] beginPos    The starting character index of the section.
     *      @param [in] endPos      The ending character index of the section.
     */
    void selectModelParameter(int beginPos, int endPos) const;

    /*!
     *  Called when the selected source file has been changed to apply initial highlighting.
     */
    void onSourceFileChanged() const;

private:

	// Disable copying.
	VHDLHighlighter(VHDLHighlighter const& rhs);
	VHDLHighlighter& operator=(VHDLHighlighter const& rhs);

    /*!
     *  Checks if the selected source file is valid for entity highlighting.
     *
     *      @param [in] fileString   The file content.
     *
     *      @return True, if highlight can be applied to entity, otherwise false.
     */
    bool canHighlightEntity(QString const& fileString) const;

    //! Display widget for the source file content.
    SourceFileDisplayer* display_;

    //! Formatting for text inside entity.
    QTextCharFormat insideEntityFormat_;

    //! Formatting for text outside entity.
    QTextCharFormat outsideEntityFormat_;

    //! Formatting for selected ports.
    QTextCharFormat selectedPortFormat_;

    //! Formatting for selected model parameters.
    QTextCharFormat selectedGenericFormat_;

    //! Formatting for unselected ports and model parameters.
    QTextCharFormat notSelectedFormat_;
};

#endif // VHDLHIGHLIGHTER_H
