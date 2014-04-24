/* 
 *  	Created on: 19.6.2012
 *      Author: Antti Kamppi
 * 		filename: addressspaceselector.h
 *		Project: Kactus 2
 */

#ifndef ADDRESSSPACESELECTOR_H
#define ADDRESSSPACESELECTOR_H

#include <IPXACTmodels/component.h>

#include <QComboBox>
#include <QSharedPointer>

class AddressSpaceSelector : public QComboBox {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the owner of the editor.
	 *
	*/
	AddressSpaceSelector(QSharedPointer<Component> component,
		QWidget *parent);
	
	//! \brief The destructor
	virtual ~AddressSpaceSelector();

public slots:

	//! \brief Refresh the items to be selected in the combo box.
	void refresh();

	/*! \brief Set the specified address space as selected.
	 *
	 * \param addrSpaceName The name of the address space to select.
	 *
	*/
	void selectAddressSpace(const QString& addrSpaceName);

signals:

	//! \brief Emitted when address space was selected by user.
	void addressSpaceSelected(const QString& addrSpaceName);

private slots:

	//! \brief Handler for selected index changes.
	void onIndexChange(int newIndex);

private:
	
	//! \brief No copying
	AddressSpaceSelector(const AddressSpaceSelector& other);

	//! \brief No assignment
	AddressSpaceSelector& operator=(const AddressSpaceSelector& other);

	//! \brief Pointer to the component being edited.
	QSharedPointer<Component> component_;
};

#endif // ADDRESSSPACESELECTOR_H
