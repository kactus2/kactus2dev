/* 
 *	Created on:	19.4.2013
 *	Author:		Antti Kamppi
 *	File name:	commandlinegeneratordialog.h
 *	Project:		Kactus 2
*/

#ifndef COMMANDLINEGENERATORDIALOG_H
#define COMMANDLINEGENERATORDIALOG_H

#include "commandlineconsole.h"

#include <QDialog>
#include <QProcess>
#include <QPushButton>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>

/*! \brief Dialog which can be used as base class for generators which run other programs in separate process.
 * 
 * This class must be sub-classed and can not be used as such.
 * Sub-classes must set up the top layout for the dialog.
 */
class CommandLineGeneratorDialog : public QDialog {
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * Method: 		CommandLineGeneratorDialog
	 * Full name:	CommandLineGeneratorDialog::CommandLineGeneratorDialog
	 * Access:		private 
	 *
	 * \param parent Pointer to the owner of the dialog.
	 *
	*/
	CommandLineGeneratorDialog(QWidget *parent);
	
	//! \brief The destructor.
	virtual ~CommandLineGeneratorDialog();

protected slots:

	//! \brief Handler for clicks on run button, this must be implemented in sub-classes.
	virtual void onRunClicked() = 0;

protected:

	//! \brief Connects the signals from process to the output message console. Create the process before calling this.
	void connectProcessToOutput();

	//! \brief The message box displaying the output from the process.
	CommandLineConsole* output_;

	//! \brief The group box which contains the output message console.
	QGroupBox* outputBox_;

	//! \brief The process which is run. This must be set in the sub-class.
	QProcess* process_;

	//! \brief Layout for the buttons on the lower right corner.
	QHBoxLayout* buttonLayout_;

	//! \brief The layout for the status name and label.
	QHBoxLayout* statusLayout_;

	//! \brief The label displaying the status of the process.
	QLabel* statusLabel_;

private slots:

	//! \brief When process has data to read from standard output (stdout) this prints it to output console.
	void onStandardOutputRead();

	//! \brief When process has data to read from standard error (stderr) this prints it to output console.
	void onStandardErrorRead();

	//! \brief Handler for process' started()-signal.
	void onProcessStarted();

	//! \brief Handler for process' finished()-signal.
	void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
	
	//! \brief No copying
	CommandLineGeneratorDialog(const CommandLineGeneratorDialog& other);

	//! \brief No assignment
	CommandLineGeneratorDialog& operator=(const CommandLineGeneratorDialog& other);

	//! \brief Button to run the generator.
	QPushButton* runButton_;

	//! \brief Button to accept changes.
	QPushButton* okButton_;

	//! \brief Button to cancel changes.
	QPushButton* cancelButton_;
};

#endif // COMMANDLINEGENERATORDIALOG_H
