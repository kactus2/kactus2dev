//-----------------------------------------------------------------------------
// File: messageconsole.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 22.02.2011
//
// Description:
// This widget is used to print event information to the user.
//-----------------------------------------------------------------------------

#include "messageconsole.h"

#include <common/KactusColors.h>

#include <QMenu>

//-----------------------------------------------------------------------------
// Function: MessageConsole::MessageConsole()
//-----------------------------------------------------------------------------
MessageConsole::MessageConsole(QWidget *parent):
QPlainTextEdit(parent),
    copyAction_(tr("Copy"), this),
    selectAllAction_(tr("Select all"), this),
    clearAction_(tr("Clear"), this)
{
	setReadOnly(true);
    setUndoRedoEnabled(false);

	copyAction_.setDisabled(true);

	connect(&copyAction_, SIGNAL(triggered()), this, SLOT(copy()), Qt::UniqueConnection);
	connect(&selectAllAction_, SIGNAL(triggered()), this, SLOT(selectAll()), Qt::UniqueConnection);
	connect(&clearAction_, SIGNAL(triggered()),	this, SLOT(clear()), Qt::UniqueConnection);
	connect(this, SIGNAL(copyAvailable(bool)), &copyAction_, SLOT(setEnabled(bool)), Qt::UniqueConnection);
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
void MessageConsole::onErrorMessage(QString const& message)
{
	if (message.isEmpty())
    {
		return;
	}

	moveCursor(QTextCursor::End);

	// errors are printed in red color
	setTextColor(KactusColors::ERROR);
    appendPlainText(message);
}

//-----------------------------------------------------------------------------
// Function: MessageConsole::onNoticeMessage()
//-----------------------------------------------------------------------------
void MessageConsole::onNoticeMessage(QString const& message)
{
	if (message.isEmpty())
    {
		return;
	}

	moveCursor(QTextCursor::End);

	// notices are printed in blue
	setTextColor(KactusColors::REGULAR_MESSAGE);
	appendPlainText(message);
}

//-----------------------------------------------------------------------------
// Function: MessageConsole::contextMenuEvent()
//-----------------------------------------------------------------------------
void MessageConsole::contextMenuEvent(QContextMenuEvent* event)
{
	QMenu menu;
	menu.addAction(&copyAction_);
	menu.addAction(&selectAllAction_);
	menu.addAction(&clearAction_);

	menu.exec(event->globalPos());
}

//-----------------------------------------------------------------------------
// Function: MessageConsole::setTextColor()
//-----------------------------------------------------------------------------
void MessageConsole::setTextColor(QColor const& color)
{
    QTextCharFormat format = currentCharFormat();
    format.setForeground(QBrush(color));
    setCurrentCharFormat(format);
}
