/* 
 *
 *  Created on: 22.2.2011
 *      Author: Antti Kamppi
 * 		filename: messageconsole.cpp
 */

#include "messageconsole.h"

#include <QColor>
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QMenu>

MessageConsole::MessageConsole(QWidget *parent):
QTextEdit(parent),
copyAction_(tr("Copy"), this),
selectAllAction_(tr("Select all"), this),
clearAction_(tr("Clear"), this),
copyAvailable_(false) {

	setReadOnly(true);

	copyAction_.setDisabled(true);

	connect(&copyAction_, SIGNAL(triggered()),
		this, SLOT(copy()), Qt::UniqueConnection);
	connect(&selectAllAction_, SIGNAL(triggered()),
		this, SLOT(selectAll()), Qt::UniqueConnection);
	connect(&clearAction_, SIGNAL(triggered()),
		this, SLOT(clear()), Qt::UniqueConnection);
	connect(this, SIGNAL(copyAvailable(bool)),
		this, SLOT(onCopyAvailable(bool)), Qt::UniqueConnection);
}

MessageConsole::~MessageConsole() {
}

void MessageConsole::onErrorMessage( const QString& message ) {

	if (message.isEmpty()) {
		return;
	}

	moveCursor(QTextCursor::End);

	QString errorText(tr("Error: "));

	// errors are printed in red color
	setTextColor(QColor("red"));
	insertPlainText(errorText + message);

	// add endline and set cursor to the end of the text
	insertPlainText("\n");
	moveCursor(QTextCursor::End);
}

void MessageConsole::onNoticeMessage( const QString& message ) {

	if (message.isEmpty()) {
		return;
	}

	moveCursor(QTextCursor::End);

	// notices are printed in blue
	setTextColor(QColor("blue"));
	insertPlainText(message);

	// add endline and set cursor to the end of the text
	insertPlainText("\n");
	moveCursor(QTextCursor::End);
}

void MessageConsole::printLog(const QString& path /*= QString()*/ ) {

	QString filePath = path;

	// if path was not defined then print log to the same location as the 
	// executable
	if (path.isEmpty()) {
		filePath = QCoreApplication::applicationDirPath();
		filePath += QString("/");
		filePath += QCoreApplication::applicationName();
		filePath += QString("_log.txt");
	}

	QFile logFile(filePath);

	// if the file can not be opened for writing
	if (!logFile.open(QFile::WriteOnly | QFile::Append)) {
		onErrorMessage(tr("Could not open log file for writing"));
		return;
	}

	// create the text stream that writes into file
	QTextStream logStream(&logFile);

	// write the contents of the message console
	logStream << toPlainText();

	// close the file
	logFile.close();
}

void MessageConsole::contextMenuEvent( QContextMenuEvent* event ) {

	copyAction_.setEnabled(copyAvailable_);

	QMenu menu;
	menu.addAction(&copyAction_);
	menu.addAction(&selectAllAction_);
	menu.addAction(&clearAction_);
	menu.exec(event->globalPos());
}

void MessageConsole::onCopyAvailable( bool yes ) {
	copyAvailable_ = yes;
}
