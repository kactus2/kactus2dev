//-----------------------------------------------------------------------------
// File: HDLGenerationDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 26.01.2017
//
// Description:
// Dialog for setting file generation options.
//-----------------------------------------------------------------------------

#ifndef HDLGENERATIONDIALOG_H
#define HDLGENERATIONDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPlainTextEdit>

#include "ViewSelectionWidget.h"
#include "FileOutputWidget.h"
#include <mainwindow/MessageConsole/messageconsole.h>

class GenerationControl;

//-----------------------------------------------------------------------------
//! Interface model for setting file generation options.
//-----------------------------------------------------------------------------
class HDLGenerationDialog : public QDialog
{
    Q_OBJECT

public:

	//! The constructor.
	HDLGenerationDialog(QSharedPointer<GenerationControl> configuration, QWidget *parent);

	//! The destructor.
    ~HDLGenerationDialog();

    //! Output window for the messages.
    MessageConsole* console_;

public slots:

    // Received when the OK button is pressed. NOTICE: Overrides slot accept() of the parent class.
    virtual void accept();

    void onViewChanged();

private slots:


    void onSelectedFileChanged(QSharedPointer<GenerationFile> newSelection);

private:

    // Disable copying.
    HDLGenerationDialog(HDLGenerationDialog const& rhs);
    HDLGenerationDialog& operator=(HDLGenerationDialog const& rhs);
  

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The "model" for the dialog.
	QSharedPointer<GenerationControl> configuration_;

	//! Widget for selecting view-component instantiation-file set -tuple.
    ViewSelectionWidget* viewSelection_;

    //! Widget for output file information.
    FileOutputWidget* fileOutput_;

    //! General warnings are displayed here.
	QLabel* generalWarningLabel_;

    //! Preview for the generation output.
    QPlainTextEdit* previewer_;
};

#endif //HDLGENERATIONDIALOG_H