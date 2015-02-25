//-----------------------------------------------------------------------------
// File: GeneratorConfigurationDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 23.02.2015
//
// Description:
// Dialog for setting file generation options.
//-----------------------------------------------------------------------------

#ifndef GENERATORCONFIGURATIONDIALOG_H
#define GENERATORCONFIGURATIONDIALOG_H

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QSharedPointer>
#include <QLineEdit>

class GeneratorConfiguration;

//-----------------------------------------------------------------------------
//! Dialog for setting file generation options.
//-----------------------------------------------------------------------------
class GeneratorConfigurationDialog : public QDialog
{
    Q_OBJECT
public:

	//! The constructor.
	GeneratorConfigurationDialog(QSharedPointer<GeneratorConfiguration> configuration, QWidget *parent);

	//! The destructor.
	~GeneratorConfigurationDialog();

    void loadConfiguration(QSharedPointer<GeneratorConfiguration> configuration);

    void setViewNames(QStringList const& viewNames);

public slots:

    virtual void accept();

private slots:

    void onFileSetStateChanged(int state);

    void onViewChanged(QString const& selectedView);

    void onPathEdited();

    void onBrowse();

private:

    // Disable copying.
    GeneratorConfigurationDialog(GeneratorConfigurationDialog const& rhs);
    GeneratorConfigurationDialog& operator=(GeneratorConfigurationDialog const& rhs);
  

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    QSharedPointer<GeneratorConfiguration> configuration_;

    QCheckBox* addToFileset_;

    QComboBox* viewSelection_;

    QLineEdit* pathEditor_;
};

#endif //GENERATORCONFIGURATIONDIALOG_H