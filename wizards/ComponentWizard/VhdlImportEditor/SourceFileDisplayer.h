//-----------------------------------------------------------------------------
// File: SourceFileDisplayer.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 11.09.2014
//
// Description:
// Display widget for source files.
//-----------------------------------------------------------------------------

#ifndef SOURCEFILEDISPLAYER_H
#define SOURCEFILEDISPLAYER_H

#include <QObject>
#include <QPlainTextEdit>

//-----------------------------------------------------------------------------
//! Display widget for source files.
//-----------------------------------------------------------------------------
class SourceFileDisplayer : public QPlainTextEdit
{
    Q_OBJECT
public:

	//! The constructor.
	SourceFileDisplayer(QWidget* parent = 0);

	//! The destructor.
	~SourceFileDisplayer();

    /*!
     *  Scrolls the text to the row containing the given character position.
     *
     *      @param [in] characterPosition   The character position to scroll to.
     */
    void scrollToCharacterPosition(int characterPosition);

signals:

    //! Emitted when the text is double clicked.
    void doubleClicked(int characterPosition);

protected:

    //! Called when mouse is double clicked.
    virtual void mouseDoubleClickEvent(QMouseEvent *e);

private:

	// Disable copying.
	SourceFileDisplayer(SourceFileDisplayer const& rhs);
	SourceFileDisplayer& operator=(SourceFileDisplayer const& rhs);
    
};

#endif // SOURCEFILEDISPLAYER_H
