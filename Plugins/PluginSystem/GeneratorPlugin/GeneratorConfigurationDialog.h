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
		QSharedPointer<QMap<QString,QSharedPointer<ComponentInstantiation> > > instantiations, QWidget *parent);

	//! The destructor.
	~GeneratorConfigurationDialog();

	void setViews(QSharedPointer<QList<QSharedPointer<View> > > const views);

	void setInstantiationNames(QString const& names);

public slots:

    virtual void accept();

private slots:

    void onFileSetStateChanged(bool on);

	void onViewChanged(QString const& selectedViewName);

	void onInstantiationInserted(QString const& selectedInstantiationName);

	void onInstantiationChanged(QString const& selectedInstantiationName);

	void onFileSetChanged(QString const& fileSetName);

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

	QSharedPointer<QMap<QString,QSharedPointer<ComponentInstantiation> > > instantiations_;

	QComboBox* viewSelection_;

	QGroupBox* addToFileset_;

	QComboBox* instantiationSelection_;

	QComboBox* fileSetSelection_;

    QLineEdit* pathEditor_;

	QLabel* instantiationWarningLabel_;

	QLabel* fileSetWarningLabel_;

	QMap<QString,QSharedPointer<View> > views_;
};

#endif //GENERATORCONFIGURATIONDIALOG_H