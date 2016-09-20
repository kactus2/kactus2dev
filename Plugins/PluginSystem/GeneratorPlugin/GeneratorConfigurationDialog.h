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

#include <QComboBox>
#include <QDialog>
#include <QSharedPointer>
#include <QLineEdit>
#include <QGroupBox>
#include <QLabel>
#include <QTableWidget>

#include "ViewSelectionWidget.h"

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

    void onOutputFilesChanged(QStringList vlvns);

    void onInterfaceGenerationStateChanged(int state);

    void onPathEdited(const QString &text);

    void onBrowse();

    void onItemChanged(QTableWidgetItem *item);

private:

    // Goes through the fileTable_ and checks if there are files that already exists.
    // Will set appropriate warnings.
    void checkExistence();

    // Disable copying.
    GeneratorConfigurationDialog(GeneratorConfigurationDialog const& rhs);
    GeneratorConfigurationDialog& operator=(GeneratorConfigurationDialog const& rhs);
  

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The "model" for the dialog.
	QSharedPointer<GeneratorConfiguration> configuration_;

	//! Editor for the path of the generated file.
	QLineEdit* pathEditor_;

	//! Widget for selecting view-component instantiation-file set -tuple.
	ViewSelectionWidget* viewSelection_;

	//! Warning is display here, namely for an existing file being overwritten.
	QLabel* generalWarningLabel_;

    //! Table used to display all needed files.
    QTableWidget* fileTable_;
};

#endif //GENERATORCONFIGURATIONDIALOG_H