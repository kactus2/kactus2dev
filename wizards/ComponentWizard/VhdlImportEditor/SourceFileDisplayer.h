//-----------------------------------------------------------------------------
// File: SourceFileDisplayer.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 11.09.2014
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#ifndef SOURCEFILEDISPLAYER_H
#define SOURCEFILEDISPLAYER_H

#include <QObject>
#include <QPlainTextEdit>

class SourceFileDisplayer : public QPlainTextEdit
{
    Q_OBJECT
public:

	//! The constructor.
	SourceFileDisplayer(QWidget* parent = 0);

	//! The destructor.
	~SourceFileDisplayer();

signals:

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
