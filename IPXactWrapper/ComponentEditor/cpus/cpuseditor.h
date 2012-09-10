/* 
 *  	Created on: 14.6.2012
 *      Author: Antti Kamppi
 * 		filename: cpuseditor.h
 *		Project: Kactus 2
 */

#ifndef CPUSEDITOR_H
#define CPUSEDITOR_H

#include <common/views/EditableTableView/editabletableview.h>
#include <IPXactWrapper/ComponentEditor/itemeditor.h>
#include <models/component.h>
#include "cpusmodel.h"

#include <QSortFilterProxyModel>
#include <QSharedPointer>

/*! \brief Editor to add/remove/edit the cpu-elements of a component.
 *
 */
class CpusEditor : public ItemEditor {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the parent of this editor.
	 *
	*/
	CpusEditor(QSharedPointer<Component> component,
				QWidget* parent = 0);
	
	//! \brief The destructor
	virtual ~CpusEditor();

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
	CpusEditor(const CpusEditor& other);

	//! \brief No assignment
	CpusEditor& operator=(const CpusEditor& other);

	//! \brief The view to display the cpus
	EditableTableView view_;

	//! \brief The model that does the sorting.
	QSortFilterProxyModel proxy_;

	//! \brief The model that manages the cpus of the editor.
	CpusModel model_;
};

#endif // CPUSEDITOR_H
