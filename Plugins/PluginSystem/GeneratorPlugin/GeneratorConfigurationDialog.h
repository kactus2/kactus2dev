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

#include <QComboBox>
#include <QDialog>
#include <QSharedPointer>
#include <QLineEdit>
#include <QGroupBox>
#include <QLabel>

#include "ViewSelectionWidget.h"

class GeneratorConfiguration;
class View;
class ComponentInstantiation;

//-----------------------------------------------------------------------------
//! Dialog for setting file generation options.
//-----------------------------------------------------------------------------
class GeneratorConfigurationDialog : public QDialog
{
    Q_OBJECT
public:

	//! The constructor.
	GeneratorConfigurationDialog(QSharedPointer<GeneratorConfiguration> configuration,
		QSharedPointer<QList<QSharedPointer<View> > > views, 
		QSharedPointer<QList<QSharedPointer<ComponentInstantiation> > > instantiations, QWidget *parent);

	//! The destructor.
	~GeneratorConfigurationDialog();

public slots:

    virtual void accept();

private slots:

    void onPathEdited(const QString &text);

    void onBrowse();

private:

    // Disable copying.
    GeneratorConfigurationDialog(GeneratorConfigurationDialog const& rhs);
    GeneratorConfigurationDialog& operator=(GeneratorConfigurationDialog const& rhs);
  

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	QSharedPointer<GeneratorConfiguration> configuration_;

	QLineEdit* pathEditor_;

	ViewSelectionWidget* viewSelection_;

	QLabel* generalWarningLabel_;
};

#endif //GENERATORCONFIGURATIONDIALOG_H