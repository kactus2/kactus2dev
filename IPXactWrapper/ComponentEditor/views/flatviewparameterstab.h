/* 
 *
 *  Created on: 18.4.2011
 *      Author: Antti Kamppi
 * 		filename: flatviewparameterstab.h
 */

#ifndef FLATVIEWPARAMETERSTAB_H
#define FLATVIEWPARAMETERSTAB_H

#include <IPXactWrapper/ComponentEditor/parameters/parametersmodel.h>

#include <common/views/EditableTableView/editabletableview.h>

#include <QWidget>
#include <QSortFilterProxyModel>
#include <QTableView>

class View;

/*! \brief Editor to edit the parameters of a view.
 *
 */
class FlatViewParametersTab : public QWidget {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param view Pointer to the view being edited.
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	FlatViewParametersTab(View* view, QWidget *parent);
	
	//! \brief The destructor
	virtual ~FlatViewParametersTab();

	/*! \brief Check for the validity of the edited parameters.
	*
	* \return True if all parameters are in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Restore the changes made in the editor back to ones in the model.
	*
	*/
	virtual void restoreChanges();

	/*! \brief Applies the changes made with the editor to the model.
	*
	* After calling this function it is no longer possible to automatically 
	* restore the previous state of the model.
	* 
	* Note: if the editor is not in valid state nothing is changed.
	*/
	virtual void applyChanges();

signals:

	//! \brief Emit an error message to user.
	void errorMessage(const QString& msg);

	//! \brief Emit a notice to user.
	void noticeMessage(const QString& msg);

	//! \brief Inform that contents of this editor have changed.
	void contentChanged();

private:
	
	//! \brief No copying
	FlatViewParametersTab(const FlatViewParametersTab& other);

	//! No assignment
	FlatViewParametersTab& operator=(const FlatViewParametersTab& other);

	//! \brief Pointer to the view being edited.
	View* viewModel_;

	//! \brief The view that displays the parameters.
	EditableTableView view_;

	//! \brief The model that holds the data to be displayed to the user
	ParametersModel model_;

	//! \brief Pointer to the proxy that is used to sort the view
	QSortFilterProxyModel proxy_;
};

#endif // FLATVIEWPARAMETERSTAB_H
