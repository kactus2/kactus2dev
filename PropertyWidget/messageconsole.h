/* 
 *
 *  Created on: 22.2.2011
 *      Author: Antti Kamppi
 * 		filename: messageconsole.h
 */

#ifndef MESSAGECONSOLE_H
#define MESSAGECONSOLE_H

#include <QTextEdit>
#include <QAction>
#include <QContextMenuEvent>

/*! \brief This widget is used to print event information to the user.
 *
 */
class MessageConsole : public QTextEdit {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget
	 *
	*/
	MessageConsole(QWidget *parent);

	//! \brief The destructor
	virtual ~MessageConsole();

	/*! \brief Print the contents of the message console to a log file
	*
	* \param path Specifies the file path of the log file.
	*
	*/
	void printLog(const QString& path = QString());

public slots:

	/*! \brief Prints an error message to the text edit.
	 *
	 * \param message The message to print
	 *
	*/
	void onErrorMessage(const QString& message);

	/*! \brief Prints a notice message to the text edit.
	 *
	 * \param message The message to print
	 *
	*/
	void onNoticeMessage(const QString& message);

protected:

	//! \brief Event handler for context menu requests.
	virtual void contextMenuEvent(QContextMenuEvent* event);

private slots:

	//! \brief Set copy action to be available or not.
	void onCopyAvailable(bool yes);

private:
	
	//! \brief No copying
	MessageConsole(const MessageConsole& other);

	//! No assignment
	MessageConsole& operator=(const MessageConsole& other);

	//! \brief Copy the selected text to the clip board.
	QAction copyAction_;

	//! \brief Select all text in the text edit.
	QAction selectAllAction_;

	//! \brief Clear all text from the text edit.
	QAction clearAction_;

	//! \brief Defines if copy is available or not.
	bool copyAvailable_;
};

#endif // MESSAGECONSOLE_H
