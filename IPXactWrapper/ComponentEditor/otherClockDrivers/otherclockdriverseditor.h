/* 
 *
 *  Created on: 4.4.2011
 *      Author: Antti Kamppi
 * 		filename: otherclockdriverseditor.h
 */

#ifndef OTHERCLOCKDRIVERSEDITOR_H
#define OTHERCLOCKDRIVERSEDITOR_H

#include <IPXactWrapper/ComponentEditor/itemeditor.h>
#include <common/views/EditableTableView/editabletableview.h>
#include <models/component.h>
#include "otherclockdriversmodel.h"

#include <QSortFilterProxyModel>
#include <QSharedPointer>

class LibraryInterface;

/*! \brief Widget to edit the otherClockDrivers of a component.
 *
 */
class OtherClockDriversEditor : public ItemEditor {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 * \param handler Pointer to the instance managing the library.
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	OtherClockDriversEditor(QSharedPointer<Component> component,
		LibraryInterface* handler, 
		QWidget *parent = 0);
	
	//! \brief The destructor
	virtual ~OtherClockDriversEditor();

	/*! \brief Check for the validity of the edited model parameters.
	*
	* \return True if all model parameters are in valid state.
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
	OtherClockDriversEditor(const OtherClockDriversEditor& other);

	//! No assignment
	OtherClockDriversEditor& operator=(const OtherClockDriversEditor& other);

	//! \brief The view that displays the parameters.
	EditableTableView view_;

	//! \brief The model that holds the data to be displayed to the user
	OtherClockDriversModel model_;

	//! \brief Pointer to the proxy that is used to sort the view
	QSortFilterProxyModel* proxy_;
};

#endif // OTHERCLOCKDRIVERSEDITOR_H
