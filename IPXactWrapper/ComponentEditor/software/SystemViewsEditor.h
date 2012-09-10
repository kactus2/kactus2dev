//-----------------------------------------------------------------------------
// File: SystemViewsEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 13.7.2012
//
// Description:
// The editor to add/remove/edit the system views of a component.
//-----------------------------------------------------------------------------

#ifndef SYSTEMVIEWSEDITOR_H
#define SYSTEMVIEWSEDITOR_H

#include "SystemViewsModel.h"

#include <common/views/EditableTableView/editabletableview.h>
#include <IPXactWrapper/ComponentEditor/itemeditor.h>

#include <QSortFilterProxyModel>

/*! \brief The editor to add/remove/edit the system views of a component.
 *
 */
class SystemViewsEditor : public ItemEditor {
	Q_OBJECT

public:

	//! \brief The width of the name column
	static const int NAME_COLUMN_WIDTH = 100;

	//! \brief The width of the bus definition column.
	static const int HIER_REF_COLUMN_WIDTH = 200;

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the parent of this editor.
	 *
	*/
	SystemViewsEditor(QSharedPointer<Component> component,
				QWidget* parent = 0);
	
	//! \brief The destructor
	virtual ~SystemViewsEditor();

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
	SystemViewsEditor(const SystemViewsEditor& other);

	//! \brief No assignment
	SystemViewsEditor& operator=(const SystemViewsEditor& other);

	//! \brief The view that displays the summary of the bus interfaces.
	EditableTableView view_;

	//! \brief The model that does the sorting of items.
	QSortFilterProxyModel proxy_;

	//! \brief The model that manages the system views.
	SystemViewsModel model_;
};

#endif // SYSTEMVIEWSEDITOR_H
