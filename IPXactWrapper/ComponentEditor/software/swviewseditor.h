/* 
 *  	Created on: 27.6.2012
 *      Author: Antti Kamppi
 * 		filename: swviewseditor.h
 *		Project: Kactus 2
 */

#ifndef SWVIEWSEDITOR_H
#define SWVIEWSEDITOR_H

#include <common/views/EditableTableView/editabletableview.h>
#include <IPXactWrapper/ComponentEditor/itemeditor.h>
#include "swviewsmodel.h"

#include <QSortFilterProxyModel>

class LibraryInterface;

/*! \brief The editor to add/remove/edit the software views of a component.
 *
 */
class SWViewsEditor : public ItemEditor {
	Q_OBJECT

public:

	//! \brief The width of the name column
	static const int NAME_COLUMN_WIDTH = 100;

	//! \brief The width of the bus definition column.
	static const int HIER_REF_COLUMN_WIDTH = 200;

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 * \param handler Pointer to the instance managing the library.
	 * \param parent Pointer to the parent of this editor.
	 *
	*/
	SWViewsEditor(QSharedPointer<Component> component,
		LibraryInterface* handler,
				QWidget* parent = 0);
	
	//! \brief The destructor
	virtual ~SWViewsEditor();

	/*! \brief Check for the validity of the editor
	* 
	* \return True if the editor is in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Reload the information from the model to the editor.
	*/
	virtual void refresh();

protected:

	//! \brief Handler for widget's show event
	virtual void showEvent(QShowEvent* event);

private:
	
	//! \brief No copying
	SWViewsEditor(const SWViewsEditor& other);

	//! \brief No assignment
	SWViewsEditor& operator=(const SWViewsEditor& other);

	//! \brief The view that displays the summary of the bus interfaces.
	EditableTableView view_;

	//! \brief The model that does the sorting of items.
	QSortFilterProxyModel proxy_;

	//! \brief The model that manages the software views.
	SWViewsModel model_;
};

#endif // SWVIEWSEDITOR_H
