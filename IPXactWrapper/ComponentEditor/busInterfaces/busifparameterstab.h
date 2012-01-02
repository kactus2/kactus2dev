/* 
 *
 *  Created on: 5.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifparameterstab.h
 */

#ifndef BUSIFPARAMETERSTAB_H
#define BUSIFPARAMETERSTAB_H

#include <IPXactWrapper/ComponentEditor/parameters/parametersmodel.h>
#include <common/views/EditableTableView/editabletableview.h>

#include <QWidget>
#include <QSortFilterProxyModel>

class BusInterface;

/*! \brief Editor to edit parameters of a bus interface.
 * 
 */
class BusIfParametersTab : public QWidget {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param dataPointer Pointer to the bus interface being edited.
	 * \param parent Pointer to the owner of this editor.
	 *
	*/
	BusIfParametersTab(void* dataPointer, QWidget *parent);
	
	//! \brief The destructor
	virtual ~BusIfParametersTab();

	/*! \brief Check for the validity of the edited item.
	*
	* \return True if item is valid.
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

	//! \brief Emitted when contents of the model change
	void contentChanged();

	//! \brief Prints an error message to the user.
	void errorMessage(const QString& msg) const;

	//! \brief Prints a notification to user.
	void noticeMessage(const QString& msg) const;

private:
	
	//! \brief No copying
	BusIfParametersTab(const BusIfParametersTab& other);

	//! No assignment
	BusIfParametersTab& operator=(const BusIfParametersTab& other);

	//! \brief Pointer to the bus interface being edited.
	BusInterface* busif_;

	//! \brief The view that displays the parameters.
	EditableTableView view_;

	//! \brief The model that holds the data to be displayed to the user
	ParametersModel model_;

	//! \brief Pointer to the proxy that is used to sort the view
	QSortFilterProxyModel* proxy_;
};

#endif // BUSIFPARAMETERSTAB_H
