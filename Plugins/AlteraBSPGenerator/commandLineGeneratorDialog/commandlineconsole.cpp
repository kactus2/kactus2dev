/* 
 *	Created on:	23.4.2013
 *	Author:		Antti Kamppi
 *	File name:	commandlineconsole.cpp
 *	Project:		Kactus2
*/

#include "commandlineconsole.h"

CommandLineConsole::CommandLineConsole(QWidget *parent):
QTextEdit(parent) {

	setReadOnly(true);
}

CommandLineConsole::~CommandLineConsole() {
}

void CommandLineConsole::printStandard( const QString& text ) {
	if (text.isEmpty()) {
		return;
	}

	moveCursor(QTextCursor::End);

	// notices are printed in blue
	setTextColor(KactusColors::REGULAR_MESSAGE);
	insertPlainText(text);

	// set cursor to the end of the text
	moveCursor(QTextCursor::End);
}

void CommandLineConsole::printError( const QString& text ) {
	if (text.isEmpty()) {
		return;
	}

	moveCursor(QTextCursor::End);

	// errors are printed in red color
	setTextColor(KactusColors::ERROR);
	insertPlainText(text);

	// set cursor to the end of the text
	moveCursor(QTextCursor::End);
}
