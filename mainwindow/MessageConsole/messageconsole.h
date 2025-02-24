//-----------------------------------------------------------------------------
// File: messageconsole.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 22.02.2011
//
// Description:
// This widget is used to print event information to the user.
//-----------------------------------------------------------------------------

#ifndef MESSAGECONSOLE_H
#define MESSAGECONSOLE_H

#include <QPlainTextEdit>
#include <QAction>
#include <QContextMenuEvent>

//-----------------------------------------------------------------------------
//! This widget is used to print event information to the user.
//-----------------------------------------------------------------------------
class MessageConsole : public QPlainTextEdit
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *    @param [in] parent     The owner of this widget
	 *
	*/
	explicit MessageConsole(QWidget *parent);

	//! The destructor
	virtual ~MessageConsole() = default;

    //! No copying
    MessageConsole(const MessageConsole& other) = delete;

    //! No assignment
    MessageConsole& operator=(const MessageConsole& other) = delete;

public slots:

	/*! Prints an error message to the text edit.
	 *
	 *    @param [in] message     The message to print.
	*/
	void onErrorMessage(QString const& message);

	/*! Prints a notice message to the text edit.
	 *
	 *    @param [in] message  The message to print.
	*/
	void onNoticeMessage(QString const& message);

protected:

	//! Event handler for context menu requests.
	virtual void contextMenuEvent(QContextMenuEvent* event);

private:

    /*! Set the color for text to be printed.
     *
     *    @param [in] color  The color to apply to text.
    */
    void setTextColor(QColor const& color);
	
    //! Copy the selected text to the clip board.
	QAction copyAction_;

	//! Select all text in the text edit.
	QAction selectAllAction_;

	//! Clear all text from the text edit.
	QAction clearAction_;
};

#endif // MESSAGECONSOLE_H
