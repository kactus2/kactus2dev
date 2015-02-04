//-----------------------------------------------------------------------------
// File: messageconsole.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 22.02.2011
//
// Description:
// This widget is used to print event information to the user.
//-----------------------------------------------------------------------------

#ifndef MESSAGECONSOLE_H
#define MESSAGECONSOLE_H

#include <common/Global.h>

#include <QTextEdit>
#include <QAction>
#include <QContextMenuEvent>

//-----------------------------------------------------------------------------
//! This widget is used to print event information to the user.
//-----------------------------------------------------------------------------
class MessageConsole : public QTextEdit
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 * @param [in] parent     Pointer to the owner of this widget
	 *
	*/
	MessageConsole(QWidget *parent);

	//! The destructor
	virtual ~MessageConsole();

	/*! Print the contents of the message console to a log file
	*
	* @param [in] path    Specifies the file path of the log file.
	*
	*/
	void printLog(const QString& path = QString());

public slots:

	/*! Prints an error message to the text edit.
	 *
	 * @param [in] message     The message to print.
	 *
	*/
	void onErrorMessage(const QString& message);

	/*! Prints a notice message to the text edit.
	 *
	 * @param [in] message  The message to print.
	 *
	*/
	void onNoticeMessage(const QString& message);

protected:

	//! Event handler for context menu requests.
	virtual void contextMenuEvent(QContextMenuEvent* event);

private slots:

	//! Set copy action to be available or not.
	void onCopyAvailable(bool yes);

private:
	
	//! No copying
	MessageConsole(const MessageConsole& other);

	//! No assignment
	MessageConsole& operator=(const MessageConsole& other);

	//! Copy the selected text to the clip board.
	QAction copyAction_;

	//! Select all text in the text edit.
	QAction selectAllAction_;

	//! Clear all text from the text edit.
	QAction clearAction_;

	//! Defines if copy is available or not.
	bool copyAvailable_;
};

#endif // MESSAGECONSOLE_H
