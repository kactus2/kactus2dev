//-----------------------------------------------------------------------------
// File: GeneratorConfigurationDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 23.02.2015
//
// Description:
// Dialog for setting file generation options.
//-----------------------------------------------------------------------------

#ifndef GENERATORCONFIGURATIONDIALOG_H
#define GENERATORCONFIGURATIONDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPlainTextEdit>

#include "ViewSelectionWidget.h"
#include "FileOutputWidget.h"

class GeneratorConfiguration;

//-----------------------------------------------------------------------------
//! Interface model for setting file generation options.
//-----------------------------------------------------------------------------
class GeneratorConfigurationDialog : public QDialog
{
    Q_OBJECT

public:

	//! The constructor.
	GeneratorConfigurationDialog(QSharedPointer<GeneratorConfiguration> configuration, QWidget *parent);

	//! The destructor.
	~GeneratorConfigurationDialog();

public slots:

    // Received when the OK button is pressed. NOTICE: Overrides slot accept() of the parent class.
    virtual void accept();

private slots:

    void onViewChanged();

    void onSelectedFileChanged(QSharedPointer<GenerationFile> newSelection);

    void onInterfaceGenerationStateChanged(int state);

    void onMemoryGenerationStateChanged(int state);

private:

    // Disable copying.
    GeneratorConfigurationDialog(GeneratorConfigurationDialog const& rhs);
    GeneratorConfigurationDialog& operator=(GeneratorConfigurationDialog const& rhs);
  

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The "model" for the dialog.
	QSharedPointer<GeneratorConfiguration> configuration_;

	//! Widget for selecting view-component instantiation-file set -tuple.
    ViewSelectionWidget* viewSelection_;

    //! Widget for output file information.
    FileOutputWidget* fileOutput_;

    //! General warnings are displayed here.
	QLabel* generalWarningLabel_;

    //! Preview for the generation output.
    QPlainTextEdit* previewer_;
};

#endif //GENERATORCONFIGURATIONDIALOG_H