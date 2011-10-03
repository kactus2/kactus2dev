/* 
 *
 *  Created on: 29.3.2011
 *      Author: Antti Kamppi
 * 		filename: modelparametereditor.h
 */

#ifndef MODELPARAMETEREDITOR_H
#define MODELPARAMETEREDITOR_H

#include <IPXactWrapper/ComponentEditor/itemeditor.h>

#include <models/modelparameter.h>
#include "modelparametermodel.h"

#include <QTableView>
#include <QPushButton>
#include <QString>
#include <QMap>
#include <QList>
#include <QSharedPointer>
#include <QSortFilterProxyModel>

class Component;

/*! \brief Widget to edit component's model parameters.
 *
 */
class ModelParameterEditor : public ItemEditor {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 * \param dataPointer Pointer to the QMap containing the model parameters.
	 * \param parent Pointer to the owner of this widget.
	 * 
	*/
	ModelParameterEditor(QSharedPointer<Component> component, 
		void* dataPointer,
		QWidget *parent);

	//! \brief The destructor
	virtual ~ModelParameterEditor();

	/*! \brief Check for the validity of the edited model parameters.
	*
	* \return True if all model parameters are in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Make the changes from the widgets editors to the IPXact model.
	 *
	*/
	virtual void makeChanges();

private slots:

	//! \brief Remove the selected row from the model
	void onRemove();

private:

	//! \brief No copying
	ModelParameterEditor(const ModelParameterEditor& other);

	//! No assignment
	ModelParameterEditor& operator=(const ModelParameterEditor& other);
	
	// TODO this could be added later
	//! \brief The button to start editing the details of a single model parameter.
	//QPushButton detailButton_;

	//! \brief The button to add a new model parameter
	QPushButton addRowButton_;

	//! \brief The button to remove the selected model parameter
	QPushButton removeRowButton_;

	//! \brief The view that displays the parameters.
	QTableView view_;

	//! \brief The model that holds the data to be displayed to the user
	ModelParameterModel model_;

	//! \brief Pointer to the proxy that is used to sort the view
	QSortFilterProxyModel* proxy_;
};

#endif // MODELPARAMETEREDITOR_H
