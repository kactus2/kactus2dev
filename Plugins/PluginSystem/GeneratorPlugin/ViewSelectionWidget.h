//-----------------------------------------------------------------------------
// File: ViewSelectionWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus2
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

class ViewSelection;

class ViewSelectionWidget : public QWidget
{
    Q_OBJECT
public:

	//! The constructor.
	ViewSelectionWidget(QSharedPointer<ViewSelection> configuration);

	//! The destructor.
	~ViewSelectionWidget();

	// Returns true, if the selections are fine by this widget. Will also update texts if needed.
	bool validSelections() const;

signals:
	
    /*!
    *  Emitted when the view is changed.
     */
	void viewChanged() const;

private slots:

    void onFileSetStateChanged(bool on);

	void onViewChanged(QString const& selectedViewName);

	void onInstantiationChanged(QString const& selectedInstantiationName);

	void setLanguage(QString selectedLanguage);

	void onFileSetChanged(QString const& fileSetName);

private:

    // Disable copying.
    ViewSelectionWidget(ViewSelectionWidget const& rhs);
    ViewSelectionWidget& operator=(ViewSelectionWidget const& rhs);
  

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	// The "model" for the widget.
	QSharedPointer<ViewSelection> configuration_;

	// The combobox to select the desired view.
	QComboBox* viewSelection_;

	// The group box for file set selection
	QGroupBox* addToFileset_;

	// The combobox to select the desired component instantiation.
	QComboBox* instantiationSelection_;

	// The label informing the known language of the currently select instantiation.
	QLabel* instantiationLanguage_;

	// The combobox to select the desired component instantiation.
	QComboBox* fileSetSelection_;

	// The label to give warnings about instantiation selection.
	QLabel* instantiationWarningLabel_;

	// The label to give warnings about file set selection.
	QLabel* fileSetWarningLabel_;
};

#endif //VIEWSELECTIONWIDGET_H