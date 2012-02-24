/* 
 *  	Created on: 21.2.2012
 *      Author: Antti Kamppi
 * 		filename: addressspaceparameterseditor.h
 *		Project: Kactus 2
 */

#ifndef ADDRESSSPACEPARAMETERSEDITOR_H
#define ADDRESSSPACEPARAMETERSEDITOR_H

#include <models/addressspace.h>
#include <common/models/ParameterModel/parametersmodel.h>
#include <common/views/EditableTableView/editabletableview.h>

#include <QGroupBox>
#include <QSortFilterProxyModel>

/*! \brief Editor to edit the parameters of an address space.
 *
 */
class AddressSpaceParametersEditor : public QGroupBox {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param addrSpace Pointer to the address space being edited.
	 * \param parent Pointer to the owner of this editor.
	 *
	*/
	AddressSpaceParametersEditor(AddressSpace* addrSpace, QWidget *parent);
	
	//! \brief The destructor
	virtual ~AddressSpaceParametersEditor();

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
	AddressSpaceParametersEditor(const AddressSpaceParametersEditor& other);

	//! \brief No assignment
	AddressSpaceParametersEditor& operator=(const AddressSpaceParametersEditor& other);

	//! \brief Pointer to the address space being edited.
	AddressSpace* addrSpace_;

	//! \brief The view that displays the parameters.
	EditableTableView view_;

	//! \brief The model that holds the data to be displayed to the user
	ParametersModel model_;

	//! \brief Pointer to the proxy that is used to sort the view
	QSortFilterProxyModel proxy_;
};

#endif // ADDRESSSPACEPARAMETERSEDITOR_H
