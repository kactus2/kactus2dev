/* 
 *  	Created on: 12.6.2012
 *      Author: Antti Kamppi
 * 		filename: viewseditor.h
 *		Project: Kactus 2
 */

#ifndef VIEWSEDITOR_H
#define VIEWSEDITOR_H

#include <common/views/EditableTableView/editabletableview.h>
#include <editors/ComponentEditor/itemeditor.h>
#include "viewsmodel.h"

#include <QSortFilterProxyModel>

class LibraryInterface;

/*! \brief The editor to add/remove/edit views of a component.
 *
 */
class ViewsEditor : public ItemEditor {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 * \param handler Pointer to the instance managing the library.
	 * \param parent Pointer to the parent of this editor.
	 *
	*/
	ViewsEditor(QSharedPointer<Component> component,
		LibraryInterface* handler,
		QWidget* parent = 0);
	
	//! \brief The destructor
	virtual ~ViewsEditor();

	/*! \brief Check for the validity of the editor
	* 
	* \return True if the editor is in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Reload the information from the model to the editor.
	*/
	virtual void refresh();

public slots:

	//! \brief Called to update the view sorting when the model changes.
    virtual void onItemChanged();

protected:

	//! \brief Handler for widget's show event
	virtual void showEvent(QShowEvent* event);

private:
	//! \brief No copying
	ViewsEditor(const ViewsEditor& other);

	//! \brief No assignment
	ViewsEditor& operator=(const ViewsEditor& other);

	//! \brief The view to display the view info.
	EditableTableView view_;

	//! \brief The proxy that does the sorting.
	QSortFilterProxyModel proxy_;

	//! \brief The model to manage the views summary.
	ViewsModel model_;
};

#endif // VIEWSEDITOR_H
