/* 
 *
 *  Created on: 25.5.2011
 *      Author: Antti Kamppi
 * 		filename: hierarchyrefwidget.h
 */

#ifndef HIERARCHYREFWIDGET_H
#define HIERARCHYREFWIDGET_H

#include <common/widgets/vlnvEditor/vlnveditor.h>

#include <QWidget>
#include <QSharedPointer>
#include <QComboBox>

class View;
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
	HierarchyRefWidget(View* view, 
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

	/*! \brief Restore the changes made in the editor back to ones in the model.
	*
	*/
	void restoreChanges();

	/*! \brief Applies the changes made with the editor to the model.
	*
	* After calling this function it is no longer possible to automatically 
	* restore the previous state of the model.
	* 
	* Note: if the editor is not in valid state nothing is changed.
	*/
	void applyChanges();

public slots:

	//! \brief Refresh the list of available views.
	void refresh();

signals:

	//! \brief Emitted when contents of the editor change.
	void contentChanged();

private:

	//! \brief No copying
	HierarchyRefWidget(const HierarchyRefWidget& other);

	//! \brief No assignment
	HierarchyRefWidget& operator=(const HierarchyRefWidget& other);
	
	//! \brief Pointer to the view being edited.
	View* view_;

	//! \brief Pointer to the component being edited.
	QSharedPointer<Component> component_;

	//! \brief The editor to set the hierarchy reference for hierarchical views.
	VLNVEditor hierarchyRef_;

	//! \brief The editor to set the reference to a top level implementation view.
	QComboBox topLevelRef_;
};

#endif // HIERARCHYREFWIDGET_H
