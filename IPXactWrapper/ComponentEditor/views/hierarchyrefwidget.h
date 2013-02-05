/* 
 *
 *  Created on: 25.5.2011
 *      Author: Antti Kamppi
 * 		filename: hierarchyrefwidget.h
 */

#ifndef HIERARCHYREFWIDGET_H
#define HIERARCHYREFWIDGET_H

#include <common/widgets/vlnvEditor/vlnveditor.h>
#include <models/view.h>

#include <QWidget>
#include <QSharedPointer>
#include <QComboBox>
#include <QSharedPointer>

class Component;
class LibraryInterface;

/*! \brief Editor to set the hierarchy reference for a hierarchical view.
 *
 */
class HierarchyRefWidget : public QWidget {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param view Pointer to the view being edited.
	 * \param component Pointer to the component being edited.
	 * \param libHandler Pointer to the library handler instance.
	 * \param parent Pointer to the owner of this editor.
	 *
	*/
	HierarchyRefWidget(QSharedPointer<View> view, 
		QSharedPointer<Component> component,
		LibraryInterface* libHandler,
		QWidget *parent);
	
	//! \brief The destructor
	virtual ~HierarchyRefWidget();

	/*! \brief Check for the validity of the edited view.
	*
	* \return True if all model parameters are in valid state.
	*/
	bool isValid() const;

public slots:

	//! \brief Refresh the list of available views.
	void refresh();

	//! \brief Clear the hierarchy reference and top level ref
	void clear();

signals:

	//! \brief Emitted when contents of the editor change.
	void contentChanged();

	//! Emitted when a help page should be changed in the context help window.
	void helpUrlRequested(const QString& url);

protected:

	//! \brief Handler for widget's show event
	virtual void showEvent(QShowEvent* event);

private slots:

	//! \brief Handler for changes in hierarchy reference.
	void onVLNVChanged();

	//! \brief Handler for changes in top level reference.
	void onTopRefChanged(const QString& newRef);

private:

	//! \brief No copying
	HierarchyRefWidget(const HierarchyRefWidget& other);

	//! \brief No assignment
	HierarchyRefWidget& operator=(const HierarchyRefWidget& other);
	
	//! \brief Pointer to the view being edited.
	QSharedPointer<View> view_;

	//! \brief Pointer to the component being edited.
	QSharedPointer<Component> component_;

	//! \brief The editor to set the hierarchy reference for hierarchical views.
	VLNVEditor* hierarchyRef_;

	//! \brief The editor to set the reference to a top level implementation view.
	QComboBox topLevelRef_;
};

#endif // HIERARCHYREFWIDGET_H
