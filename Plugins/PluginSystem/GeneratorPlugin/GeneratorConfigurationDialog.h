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

    virtual void accept();

private slots:

    void onViewChanged();

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

	//! Warning is display here, namely for an existing file being overwritten.
	QLabel* generalWarningLabel_;
};

#endif //GENERATORCONFIGURATIONDIALOG_H