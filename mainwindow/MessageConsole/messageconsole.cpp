//-----------------------------------------------------------------------------
// File: messageconsole.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 22.02.2011
//
// Description:
// This widget is used to print event information to the user.
//-----------------------------------------------------------------------------

#include "messageconsole.h"

#include <QColor>
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QMenu>

//-----------------------------------------------------------------------------
// Function: MessageConsole::MessageConsole()
//-----------------------------------------------------------------------------
MessageConsole::MessageConsole(QWidget *parent):
QTextEdit(parent),
copyAction_(tr("Copy"), this),
selectAllAction_(tr("Select all"), this),
clearAction_(tr("Clear"), this),
copyAvailable_(false)
{
	setReadOnly(true);

	copyAction_.setDisabled(true);

	connect(&copyAction_, SIGNAL(triggered()), this, SLOT(copy()), Qt::UniqueConnection);
	connect(&selectAllAction_, SIGNAL(triggered()), this, SLOT(selectAll()), Qt::UniqueConnection);
	connect(&clearAction_, SIGNAL(triggered()),	this, SLOT(clear()), Qt::UniqueConnection);
	connect(this, SIGNAL(copyAvailable(bool)), this, SLOT(onCopyAvailable(bool)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: MessageConsole::~MessageConsole()
//-----------------------------------------------------------------------------
MessageConsole::~MessageConsole() 
{

}

//-----------------------------------------------------------------------------
// Function: MessageConsole::onErrorMessage()
//-----------------------------------------------------------------------------
void MessageConsole::onErrorMessage( const QString& message )
{
	if (message.isEmpty())
    {
		return;
	}

	moveCursor(QTextCursor::End);

	// errors are printed in red color
	setTextColor(QColor("red"));
	//insertPlainText(tr("Error: "));
    insertPlainText(message);

	// add endline and set cursor to the end of the text
	insertPlainText("\n");
	//moveCursor(QTextCursor::End);
}

//-----------------------------------------------------------------------------
// Function: MessageConsole::onNoticeMessage()
//-----------------------------------------------------------------------------
void MessageConsole::onNoticeMessage( const QString& message )
{
	if (message.isEmpty())
    {
		return;
	}

	moveCursor(QTextCursor::End);

	// notices are printed in blue
	setTextColor(QColor("blue"));
	insertPlainText(message);

	// add endline and set cursor to the end of the text
	insertPlainText("\n");
	//moveCursor(QTextCursor::End);
}

//-----------------------------------------------------------------------------
// Function: MessageConsole::contextMenuEvent()
//-----------------------------------------------------------------------------
void MessageConsole::contextMenuEvent( QContextMenuEvent* event )
{
	copyAction_.setEnabled(copyAvailable_);

	QMenu menu;
	menu.addAction(&copyAction_);
	menu.addAction(&selectAllAction_);
	menu.addAction(&clearAction_);
	menu.exec(event->globalPos());
}

//-----------------------------------------------------------------------------
// Function: MessageConsole::onCopyAvailable()
//-----------------------------------------------------------------------------
void MessageConsole::onCopyAvailable( bool yes )
{
	copyAvailable_ = yes;
}
