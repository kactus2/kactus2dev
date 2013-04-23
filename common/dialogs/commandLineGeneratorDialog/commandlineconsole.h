/* 
 *	Created on:	23.4.2013
 *	Author:		Antti Kamppi
 *	File name:	commandlineconsole.h
 *	Project:		Kactus 2
*/

#ifndef COMMANDLINECONSOLE_H
#define COMMANDLINECONSOLE_H

#include <common/Global.h>

#include <QTextEdit>

/*! \brief Used to print standard and error output from other processes run through Kactus2.
 * 
 */
class KACTUS2_API CommandLineConsole : public QTextEdit {
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * Method: 		CommandLineConsole
	 * Full name:	CommandLineConsole::CommandLineConsole
	 * Access:		private 
	 *
	 * \param parent Pointer to the owner of the console.
	 *
	*/
	CommandLineConsole(QWidget *parent);
	
	//! \brief The destructor.
	virtual ~CommandLineConsole();

public slots:

	//! \brief Print the text as normal.
	void printStandard(const QString& text);

	//! \brief Print the text as error.
	void printError(const QString& text);

private:
	
	//! \brief No copying
	CommandLineConsole(const CommandLineConsole& other);

	//! \brief No assignment
	CommandLineConsole& operator=(const CommandLineConsole& other);
};

#endif // COMMANDLINECONSOLE_H
