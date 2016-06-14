//-----------------------------------------------------------------------------
// File: ViewSelectionWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 14.06.2016
//
// Description:
// Dialog for selecting a view, component instantiation and file set for generation.
//-----------------------------------------------------------------------------

#ifndef VIEWSELECTIONWIDGET_H
#define VIEWSELECTIONWIDGET_H

#include <QComboBox>
#include <QSharedPointer>
#include <QLineEdit>
#include <QGroupBox>
#include <QLabel>

class GeneratorConfiguration;
class View;
class ComponentInstantiation;

class ViewSelectionWidget : public QWidget
{
    Q_OBJECT
public:

	//! The constructor.
	ViewSelectionWidget(QSharedPointer<GeneratorConfiguration> configuration,
		QSharedPointer<QList<QSharedPointer<View> > > views, 
		QSharedPointer<QList<QSharedPointer<ComponentInstantiation> > > instantiations);

	//! The destructor.
	~ViewSelectionWidget();

	bool validSelections() const;

private slots:

    void onFileSetStateChanged(bool on);

	void onViewChanged(QString const& selectedViewName);

	void onInstantiationInserted(QString const& selectedInstantiationName);

	void onInstantiationChanged(QString const& selectedInstantiationName);

	void onFileSetChanged(QString const& fileSetName);

private:

    // Disable copying.
    ViewSelectionWidget(ViewSelectionWidget const& rhs);
    ViewSelectionWidget& operator=(ViewSelectionWidget const& rhs);
  

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	QSharedPointer<GeneratorConfiguration> configuration_;

	QMap<QString,QSharedPointer<View> > views_;

	QMap<QString,QSharedPointer<ComponentInstantiation> > instantiations_;

	QComboBox* viewSelection_;

	QGroupBox* addToFileset_;

	QComboBox* instantiationSelection_;

	QComboBox* fileSetSelection_;

	QLabel* instantiationWarningLabel_;

	QLabel* fileSetWarningLabel_;
};

#endif //VIEWSELECTIONWIDGET_H