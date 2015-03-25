//-----------------------------------------------------------------------------
// File: hierarchyrefwidget.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 25.5.2011
//
// Description:
// Editor to set the hierarchy reference for a hierarchical view.
//-----------------------------------------------------------------------------

#ifndef HIERARCHYREFWIDGET_H
#define HIERARCHYREFWIDGET_H

#include <common/widgets/vlnvEditor/vlnveditor.h>
#include <IPXACTmodels/view.h>

#include <QWidget>
#include <QSharedPointer>
#include <QComboBox>
#include <QSharedPointer>

class LibraryInterface;

//-----------------------------------------------------------------------------
//! Editor to set the hierarchy reference for a hierarchical view.
//-----------------------------------------------------------------------------
class HierarchyRefWidget : public QWidget
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *        @param [in] view          The view being edited.
	 *        @param [in] libHandler    The library handler instance to use.
	 *        @param [in] parent        The owner of this editor.
	 *
	*/
	HierarchyRefWidget(QSharedPointer<View> view, LibraryInterface* libHandler, QWidget* parent);
	
	//! The destructor
	virtual ~HierarchyRefWidget();

	/*! Check for the validity of the edited view.
	*
	*       @return True, if the hierarchical reference is valid, otherwise false.
	*/
	bool isValid() const;

public slots:

    /*!
     *  Refresh the list of available views.
     *
     *      @param [in] availableViews   The available view names.
     */
    void refresh(QStringList const& availableViews);

	//! Clear the hierarchy reference and top level view reference.
	void clear();

signals:

	//! Emitted when contents of the editor change.
	void contentChanged();

	//! Emitted when a help page should be changed in the context help window.
	void helpUrlRequested(QString const& url);

protected:

	//! Handler for widget's show event
	virtual void showEvent(QShowEvent* event);

private slots:

	//! Handler for changes in hierarchy reference.
	void onVLNVChanged();

	//! Handler for changes in top level reference.
    void onTopViewChanged(QString const& newViewName);

private:

	//! No copying
	HierarchyRefWidget(const HierarchyRefWidget& other);

	//! No assignment
	HierarchyRefWidget& operator=(const HierarchyRefWidget& other);
	
    //! Updates the referenced design according to the view hierarchy reference.
    void updateDesignReference();

    //! Sets the widget layout.
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The library in use.
    LibraryInterface* library_;

	//! Pointer to the view being edited.
	QSharedPointer<View> view_;

	//! The editor to set the design configuration as the hierarchical reference.
	VLNVEditor* designConfigurationEditor_;

    //! The display for referenced design.
    VLNVEditor* designReferenceDisplay_;

	//! The editor to set the reference to a top level implementation view.
	QComboBox* topLevelRef_;
};

#endif // HIERARCHYREFWIDGET_H
