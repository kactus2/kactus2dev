/* 
 *
 *  Created on: 4.4.2011
 *      Author: Antti Kamppi
 * 		filename: parameterseditor.h
 */

#ifndef PARAMETERSEDITOR_H
#define PARAMETERSEDITOR_H

#include <IPXactWrapper/ComponentEditor/itemeditor.h>

#include <common/models/ParameterModel/parametersmodel.h>

#include <common/views/EditableTableView/editabletableview.h>

#include <QSortFilterProxyModel>
#include <QSharedPointer>

class Component;

/*! \brief Editor to add/edit/remove parameters of a component.
 *
 */
class ParametersEditor : public ItemEditor {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	ParametersEditor(QSharedPointer<Component> component, 
		QWidget *parent = 0);
	
	//! \brief The destructor
	virtual ~ParametersEditor();

	/*! \brief Check for the validity of the edited parameters.
	*
	* \return True if all parameters are in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Reload the information from the model to the editor.
	*/
	virtual void refresh();

private:

	//! \brief No copying
	ParametersEditor(const ParametersEditor& other);

	//! No assignment
	ParametersEditor& operator=(const ParametersEditor& other);

	//! \brief The view that displays the parameters.
	EditableTableView view_;

	//! \brief The model that holds the data to be displayed to the user
	ParametersModel model_;

	//! \brief Pointer to the proxy that is used to sort the view
	QSortFilterProxyModel* proxy_;
};

#endif // PARAMETERSEDITOR_H
