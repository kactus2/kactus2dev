/* 
 *
 *  Created on: 29.3.2011
 *      Author: Antti Kamppi
 * 		filename: modelparametereditor.h
 */

#ifndef MODELPARAMETEREDITOR_H
#define MODELPARAMETEREDITOR_H

#include <IPXactWrapper/ComponentEditor/itemeditor.h>
#include <models/component.h>
#include "modelparametermodel.h"
#include <common/views/EditableTableView/editabletableview.h>

#include <QString>
#include <QMap>
#include <QList>
#include <QSharedPointer>
#include <QSortFilterProxyModel>

class LibraryInterface;

/*! \brief Widget to edit component's model parameters.
 *
 */
class ModelParameterEditor : public ItemEditor {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the owner of this widget.
	 * 
	*/
	ModelParameterEditor(QSharedPointer<Component> component,
		LibraryInterface* handler, 
		QWidget *parent = 0);

	//! \brief The destructor
	virtual ~ModelParameterEditor();

	/*! \brief Check for the validity of the edited model parameters.
	*
	* \return True if all model parameters are in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Reload the information from the model to the editor.
	*/
	virtual void refresh();

	/*! \brief Enable/disable the import/export csv functionality for the view.
	 *
	 * Method: 		setAllowImportExport
	 * Full name:	ModelParameterEditor::setAllowImportExport
	 * Access:		public 
	 *
	 * \param allow If true then import/export is enabled.
	 *
	*/
	void setAllowImportExport(bool allow);

protected:

	//! \brief Handler for widget's show event
	virtual void showEvent(QShowEvent* event);

private:

	//! \brief No copying
	ModelParameterEditor(const ModelParameterEditor& other);

	//! No assignment
	ModelParameterEditor& operator=(const ModelParameterEditor& other);

	//! \brief The view that displays the parameters.
	EditableTableView view_;

	//! \brief The model that holds the data to be displayed to the user
	ModelParameterModel model_;

	//! \brief Pointer to the proxy that is used to sort the view
	QSortFilterProxyModel proxy_;
};

#endif // MODELPARAMETEREDITOR_H
